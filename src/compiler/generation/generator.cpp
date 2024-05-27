#include "generator.hpp"

#include <iostream>
#include <variant>

#include "special/consts.hpp"
#include "utils.hpp"

Generator::Generator() {}

std::string Generator::generate(const ProgramNode &program)
{
    GenerateData generation = GenerateData();

    generation.dataSection << TAB << "stdout dq 0" << NEW_LINE;
    generation.dataSection << TAB << "stdin dq 0" << NEW_LINE;
    generation.dataSection << TAB << "bytesWritten dw 0" << NEW_LINE;

    generation.output << "section .text" << NEW_LINE;
    generation.output << TAB << "global " << START << NEW_LINE;
    //generation.output << TAB << "default rel;     Tells NASM to use RIP-rel addressing by default" << NEW_LINE;
    generation.output << TAB << "extern " << EXIT_PROCESS_WINDOWS << NEW_LINE;
    utils::useStdout(generation);
    utils::useStdin(generation);
    utils::useHeapAllocation(generation);
    generation.output << START << ":" << NEW_LINE;
    utils::getStdoutHandle(generation);
    utils::getStdinHandle(generation);
    utils::getHeapHandle(generation);

    for (StatementNode* statement : program.nodes)
    {
        generateStatement(statement, generation);

        for (auto error : generation.errors)
            std::cerr << codeGenerationErrorToString(error) << std::endl;
        generation.errors.clear();
    }

    generation.output << NEW_LINE << TAB << "; Default return" << NEW_LINE;
    utils::generateExitCode("0", generation.output);

    std::optional<Function> invalidFunctionCall = generation.checkIfFunctionCallsAreValid();
    if(invalidFunctionCall.has_value())
    {
        std::cerr << "The function call `" << invalidFunctionCall.value().name << "` is invalid!" << std::endl;
        return "";
    }

    return generation.convertToProgram();
}

// Size of the return address from a function
const int RETURN_ADDRESS_SIZE = 1;

void Generator::generateStatement(const StatementNode* statement, GenerateData &generation)
{
    struct Visitor
    {
        GenerateData &generation;
        Generator &generator;
        Visitor(Generator &generator, GenerateData &generation)
            : generator(generator), generation(generation) {}

        void operator()(const StatementReturnNode* statement)
        {
            generation.output << NEW_LINE << TAB << "; Return" << NEW_LINE;
            generator.generateExpression("rax", statement->expression, generation);

            auto currentFunctionDefinition = generation.currentFunctionDefinition;
            if(currentFunctionDefinition.has_value())
            {
                auto variablesToRemove = 8 * (generation.stackSize - currentFunctionDefinition.value().scope->startStackPtr - RETURN_ADDRESS_SIZE);
                generation.output << TAB << "mov rcx, QWORD [rsp + " << (variablesToRemove - currentFunctionDefinition.value().parametersCount * 8) << "]" << NEW_LINE;
                if(variablesToRemove != 0)
                    generation.output << TAB << "add rsp, " << (variablesToRemove - 8) << NEW_LINE;
                generation.output << TAB << "mov QWORD [rsp + 8], rax" << NEW_LINE;
                generation.output << TAB << "mov QWORD [rsp], rcx" << NEW_LINE;
                generation.output << TAB << "ret" << NEW_LINE;

            }
            else
                utils::generateExitCode("rax", generation.output);
        }
        void operator()(const StatementMacroNode* statement)
        {
            std::string macroName = statement->macroName->ident.value.value();
            if(macroName == "asm!")
            {
                if(statement->arguments.size() == 1)
                {
                    std::visit([&](auto arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ExpressionAtomNode*>)
                        {
                            std::visit([&](auto insideArg)
                            {
                                using V = std::decay_t<decltype(insideArg)>;
                                if constexpr (std::is_same_v<V, ExpressionLiteralNode*>)
                                {
                                    std::string assemblyCode = insideArg->literal.value.value();
                                    generation.output << assemblyCode << NEW_LINE;
                                }
                                else
                                    std::cerr << "The asm! macro should have only a string literal argument" << std::endl;

                            }, arg->variant);
                        }
                        else
                            std::cerr << "The asm! macro should have only a string literal argument" << std::endl;

                    }, statement->arguments[0]->variant);
                }
                else
                    std::cerr << "The asm! macro should have only 1 argument, but " << statement->arguments.size() << " were found!" << std::endl;
            }
            else if(macroName == "include!")
            {
                if(statement->arguments.size() == 1)
                {
                    std::visit([&](auto arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, ExpressionAtomNode*>)
                        {
                            std::visit([&](auto insideArg)
                            {
                                using V = std::decay_t<decltype(insideArg)>;
                                if constexpr (std::is_same_v<V, ExpressionLiteralNode*>)
                                {
                                    std::string filePath = insideArg->literal.value.value();
                                    
                                    std::cerr << "This feature isn't supported yet!" << std::endl;
                                    /*std::string includedFileContent = "";
                                    generation.output << includedFileContent << NEW_LINE;*/
                                }
                                else
                                    std::cerr << "The include! macro should have only a string literal argument" << std::endl;

                            }, arg->variant);
                        }
                        else
                            std::cerr << "The include! macro should have only a string literal argument" << std::endl;

                    }, statement->arguments[0]->variant);
                }
                else
                    std::cerr << "The include! macro should have only 1 argument, but " << statement->arguments.size() << " were found!" << std::endl;
            }
            else
                std::cerr << "Macro `" << macroName << "` not supported!" << std::endl;
        }
        void operator()(const StatementDeclareVariableNode* statement)
        {
            const std::string &variableName = statement->name->ident.value.value();
            if (generation.doesVariableExist(variableName))
            {
                generation.errors.push_back(CodeGenerationError
                {
                    .type = CodeGenerationErrorType::VariableAlreadyDefined,
                    .hint = variableName
                });
                return;
            }

            generation.defineVariable(variableName);
            generation.output << TAB << "mov rax, 0 ; Declaring variable named `" << variableName << "`" << NEW_LINE;
            generation.pushOnStack("rax");
        }
        void operator()(const StatementScopeNode* statement)
        {
            generator.generateStatementScope(statement, generation);
        }
        void operator()(const StatementIfNode* statement)
        {
            auto hasElse = statement->elseScope.has_value();
            auto endIfLabel = generation.generateLabel();
            auto startElseLabel = generation.generateLabel();
            generator.generateExpression("rax", statement->condition, generation);
            generation.output << TAB << "test rax, rax" << NEW_LINE;
            generation.output << TAB << "jz " << (hasElse ? startElseLabel : endIfLabel) << NEW_LINE;
            generator.generateStatementScope(statement->scope, generation);
            if(hasElse)
            {
                generation.output << TAB << "jmp " << endIfLabel << NEW_LINE;
                generation.output << startElseLabel << ":" << NEW_LINE;
                generator.generateStatementScope(statement->elseScope.value(), generation);
            }
            generation.output << endIfLabel << ":" << NEW_LINE;
        }
        void operator()(const StatementWhileNode* statement)
        {
            auto startLoopLabel = generation.generateLabel();
            auto endLoopLabel = generation.generateLabel();
            generation.output << startLoopLabel << ":" << NEW_LINE;
            generator.generateExpression("rax", statement->condition, generation);
            generation.output << TAB << "test rax, rax" << NEW_LINE;
            generation.output << TAB << "jz " << endLoopLabel << NEW_LINE;
            generator.generateStatementScope(statement->scope, generation);
            generation.output << TAB << "jmp " << startLoopLabel << NEW_LINE;
            generation.output << endLoopLabel << ":" << NEW_LINE;
        }
        void operator()(const StatementAssignVariableNode* statement)
        {
            const std::string &variableName = statement->name->ident.value.value();
            if (!generation.doesVariableExist(variableName))
            {
                generation.errors.push_back(CodeGenerationError
                {
                    .type = CodeGenerationErrorType::UndeclaredVariable,
                    .hint = variableName
                });
                return;
            }

            Variable variable = generation.getVariableByName(variableName).value();
            std::visit(
                [&](auto arg)
                {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, ExpressionAtomNode*>)
                    {
                        std::visit([&](auto arg)
                        {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, ExpressionLiteralNode*>)
                            {
                                if(arg->literal.type == TokenType::LiteralString)
                                {
                                    std::string stringLiteralName = generation.defineStringLiteral(arg->literal.value.value());
                                    generation.output << TAB << "mov rdx"// << utils::accessVariable(generation, variable) 
                                        << ", " << stringLiteralName << NEW_LINE;
                                    generation.output << TAB << "mov " << utils::accessVariable(generation, variable) 
                                        << ", rdx" << NEW_LINE;
                                }
                                else
                                {
                                    generation.output << TAB << "mov " << utils::accessVariable(generation, variable) 
                                        << ", " << arg->literal.value.value() << NEW_LINE;
                                }
                            }
                            else if constexpr (std::is_same_v<T, ExpressionIdentNode*>)
                            {
                                auto identNode = ExpressionIdentNode
                                {
                                    .ident = arg->ident
                                };
                                auto atomNode = ExpressionAtomNode
                                {
                                    .variant = &identNode
                                };
                                auto node = ExpressionNode {
                                    .variant = &atomNode
                                };
                                generator.generateExpression("rax", &node, generation);
                                generation.output << TAB << "mov " << utils::accessVariable(generation, variable) 
                                    << ", rax" << NEW_LINE;
                            }
                            else if constexpr (std::is_same_v<T, ExpressionFunctionCallNode*>)
                            {
                                auto atomNode = ExpressionAtomNode
                                {
                                    .variant = arg
                                };
                                auto node = ExpressionNode {
                                    .variant = &atomNode
                                };
                                generator.generateExpression("rax", &node, generation);
                                generation.output << TAB << "mov " << utils::accessVariable(generation, variable) 
                                    << ", rax" << NEW_LINE;
                            }
                            else
                                std::cerr << "Unsupported type!" << std::endl;
                        }, arg->variant);
                    }
                    else if constexpr (std::is_same_v<T, ExpressionBinaryOperatorNode*>)
                    {
                        auto node = ExpressionNode {
                            .variant = arg
                        };
                        generator.generateExpression("rax", &node, generation);
                        generation.output << TAB << "mov " << utils::accessVariable(generation, variable) 
                            << ", rax" << NEW_LINE;
                    }
                    else
                        std::cerr << "Unsupported type!" << std::endl;
                },
                statement->value->variant
            );
        }
        void operator()(const StatementFunctionDefinitionNode* statement)
        {
            if(generation.currentFunctionDefinition.has_value())
            {
                std::cerr << "You can't define a function inside a function" << std::endl;
                return;
            }
            auto endFunctionLabel = generation.generateLabel();
            generation.output << NEW_LINE << TAB << "jmp " << endFunctionLabel << ";  Skip the function definition" << NEW_LINE;

            auto functionName = statement->functionName->ident.value.value();
            
            generation.defineFunction(Function {
                .name = functionName,
                .parameters = statement->parameters
            });

            generation.enterScope();

            generation.output << functionName << ":" << NEW_LINE;

            generation.stackSize += RETURN_ADDRESS_SIZE + statement->parameters.size();
            for(int i = 0; i < statement->parameters.size(); i++)
            {
                generation.defineVariableWithOffsetStack(statement->parameters[statement->parameters.size() - i - 1]->name->ident.value.value(), 2 + i);
            }
            generation.currentFunctionDefinition = FunctionDefinition { .scope = generation.currentScope, .parametersCount = statement->parameters.size() };
            generator.generateStatementScopeWithoutEntering(statement->implementation, generation);
            generation.currentFunctionDefinition = std::nullopt;
            
            generation.output << TAB << "ret" << NEW_LINE;

            generation.output << endFunctionLabel << ":" << NEW_LINE;
        }
        void operator()(ExpressionFunctionCallNode* expression)
        {
            auto atom = ExpressionAtomNode
            {
                .variant = expression
            };
            auto node = ExpressionNode
            {
                .variant = &atom
            };
            generator.generateExpression("rax", &node, generation);
        }
    };

    Visitor visitor(*this, generation);
    std::visit(visitor, statement->variant);
}

void Generator::generateStatementScopeWithoutEntering(const StatementScopeNode* statement, GenerateData &generation)
{
    for(auto statement : statement->statements)
        generateStatement(statement, generation);

    generation.exitScope();
}

void Generator::generateStatementScope(const StatementScopeNode* statement, GenerateData &generation)
{
    generation.enterScope();

    generateStatementScopeWithoutEntering(statement, generation);
}

void Generator::generateExpressionAtom(const std::string& registerName, const ExpressionAtomNode* expression, GenerateData& generation)
{
    struct Visitor
    {
        GenerateData &generation;
        Generator &generator;
        const std::string& registerName;
        Visitor(Generator &generator, GenerateData &generation, const std::string& registerName) : 
            generator(generator), generation(generation), registerName(registerName) {}

        void operator()(const ExpressionLiteralNode* expression)
        {
            if(expression->literal.type == TokenType::LiteralString)
            {
                std::string stringLiteralName = generation.defineStringLiteral(expression->literal.value.value());
                generation.output << TAB << "mov " << registerName << ", " << stringLiteralName << NEW_LINE;
            }
            else
                generation.output << TAB << "mov " << registerName << ", " << expression->literal.value.value() << NEW_LINE;
        }
        void operator()(const ExpressionIdentNode* expression)
        {
            const std::string& variableName = expression->ident.value.value();
            std::optional<Variable> variable = generation.getVariableByName(variableName);
            if (!variable.has_value())
            {
                generation.errors.push_back(CodeGenerationError
                {
                    .type = CodeGenerationErrorType::UndeclaredVariable,
                    .hint = variableName
                });
                return;
            }
            
            generation.output << TAB << "mov " << registerName << ", " << utils::accessVariable(generation, variable.value()) << NEW_LINE;
        }
        void operator()(const ExpressionBracketsNode* expression)
        {
            generator.generateExpression(registerName, expression->expression, generation);
        }
        void operator()(const ExpressionFunctionCallNode* expression)
        {
            auto functionName = expression->functionName->ident.value.value();
            for(int i = 0; i < expression->arguments.size(); i++)
            {
                //generation.output << TAB << "; Passing the " << (i + 1) << (i == 0 ? "st" : i == 1 ? "nd" :  i == 2 ? "rd" : "th") << " argument to the function `" << functionName << "`" << NEW_LINE;
                generator.generateExpression("rax", expression->arguments[i], generation);
                generation.pushOnStack("rax");
            }
            generation.output << TAB << "call " << functionName << NEW_LINE;
            generation.popFromStack("rax");
            generation.stackSize -= expression->arguments.size() - 1;
            
            // TODO:
            // Also the memory of `temporary` is never deallocated
            StatementDeclareVariableNode* temporary = new StatementDeclareVariableNode(StatementDeclareVariableNode
            {
                .type = new ExpressionIdentNode(ExpressionIdentNode {.ident = Token { .type = TokenType::Unknown, .value = std::nullopt }}),
                .name = new ExpressionIdentNode(ExpressionIdentNode {.ident = Token { .type = TokenType::Unknown, .value = std::nullopt }}),
            });
            auto parameters = std::vector<StatementDeclareVariableNode*>(expression->arguments.size(), temporary);
            generation.callFunction(Function 
            {
                .name = functionName,
                .parameters = parameters
            });
        }
    };

    Visitor visitor(*this, generation, registerName);
    std::visit(visitor, expression->variant);
}

static void generateComparison(const std::string& jumpInstruction, GenerateData &generation)
{
    auto trueLabel = generation.generateLabel();
    auto endLabel = generation.generateLabel();
    generation.output << TAB << "cmp rax, rbx" << NEW_LINE;
    generation.output << TAB << jumpInstruction << " " << trueLabel << NEW_LINE;
    generation.output << TAB << "mov rax, 0" << NEW_LINE;
    generation.output << TAB << "jmp " << endLabel << NEW_LINE;
    generation.output << trueLabel << ": " << NEW_LINE;
    generation.output << TAB << "mov rax, 1" << NEW_LINE;
    generation.output << endLabel << ": " << NEW_LINE;
}

void Generator::generateExpression(const std::string& registerName, const ExpressionNode* expression,
                                   GenerateData &generation)
{
    struct Visitor
    {
        GenerateData &generation;
        Generator &generator;
        const std::string& registerName;
        Visitor(const std::string& registerName, Generator &generator, GenerateData &generation)
            : registerName(registerName), generator(generator), generation(generation) {}

        void operator()(const ExpressionAtomNode* expression)
        {
            generator.generateExpressionAtom(registerName, expression, generation);
        }
        void operator()(const ExpressionBinaryOperatorNode* expression)
        {
            generator.generateExpression("rax", expression->lhs, generation);
            generation.pushOnStack("rax");
            generator.generateExpression("rbx", expression->rhs, generation);
            generation.popFromStack("rax");

            switch(expression->operation)
            {
                case Operator::Add:
                    generation.output << TAB << "add rax, rbx" << NEW_LINE;
                    break;
                case Operator::Sub:
                    generation.output << TAB << "sub rax, rbx" << NEW_LINE;
                    break;
                case Operator::Mul:
                    generation.output << TAB << "mul rbx" << NEW_LINE;
                    break;
                case Operator::Div:
                    generation.output << TAB << "div rbx" << NEW_LINE;
                    break;
                case Operator::GreaterThan:
                    generateComparison("jg", generation);
                    break;
                case Operator::LessThan:
                    generateComparison("jl", generation);
                    break;
                case Operator::EqualTo:
                    generateComparison("je", generation);
                    break;
                case Operator::NotEqualTo:
                    generateComparison("jne", generation);
                    break;
            }

            generation.output << TAB << "mov rbx, rax" << NEW_LINE;
        }
    };

    Visitor visitor(registerName, *this, generation);
    std::visit(visitor, expression->variant);
}