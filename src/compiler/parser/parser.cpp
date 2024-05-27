#include "parser.hpp"

#include <iostream>

Parser::Parser() : allocator(1 * 1024 * 1024)
{
}

ProgramNode Parser::parse(std::vector<Token> tokensVector)
{
    ProgramNode programNode = {.nodes = std::vector<StatementNode*>()};

    std::queue<Token> tokens;
    for (Token token : tokensVector)
    {
        tokens.push(token);
    }

    while (!tokens.empty())
    {
        auto error = ParsingStatementError { .type = ParsingStatementErrorType::None };
        auto statement = parseStatement(tokens, error);
        if (error.type != ParsingStatementErrorType::None)
        {
            auto errorMessage = parsingErrorToString.find(error.type);
            if (errorMessage != parsingErrorToString.end())
            {
                std::cerr << "At line " << error.metadata.lineNumber << ", column " << error.metadata.lineNumber << " there's this error: " << errorMessage->second << std::endl;
                if(!error.hint.empty())
                    std::cerr << "Hint: " << error.hint;
            }
            return {};
        }
        if(statement.has_value())
            programNode.nodes.push_back(statement.value());
    }

    return programNode;
}

static std::optional<Operator> tokenTypeToOperator(TokenType tokenType)
{
    switch(tokenType)
    {
        case TokenType::PlusSign:
            return Operator::Add;
        case TokenType::MinusSign:
            return Operator::Sub;
        case TokenType::MultiplicationSign:
            return Operator::Mul;
        case TokenType::DivisionSign:
            return Operator::Div;
        case TokenType::GreaterThanSign:
            return Operator::GreaterThan;
        case TokenType::LessThanSign:
            return Operator::LessThan;
        case TokenType::DoubleEqualSign:
            return Operator::EqualTo;
        case TokenType::NotEqualSign:
            return Operator::NotEqualTo;
        default:
            return std::nullopt;
    }
}

std::optional<ExpressionNode*> Parser::parseExpression(std::queue<Token> &tokens)
{
    return parseExpressionInternal(tokens, 1);
}

std::optional<ExpressionAtomNode*> Parser::parseExpressionAtom(std::queue<Token> &tokens)
{
    if (!tokens.empty())
    {
        Token token = tokens.front();

        if (token.type == TokenType::LiteralNumber || token.type == TokenType::LiteralString)
        {
            tokens.pop();
            return allocator.allocate_and_initialize<ExpressionAtomNode>(allocator.allocate_and_initialize<ExpressionLiteralNode>(token));
        }
        else if (token.type == TokenType::Ident)
        {
            tokens.pop();
            if(tokens.empty() || tokens.front().type != TokenType::OpenRoundBracket)
                return allocator.allocate_and_initialize<ExpressionAtomNode>(allocator.allocate_and_initialize<ExpressionIdentNode>(token));
            else
            {
                tokens.pop();

                std::optional<ExpressionFunctionCallNode*> functionCall = parseExpressionFunctionCall(tokens, token);
                if(functionCall.has_value())
                    return allocator.allocate_and_initialize<ExpressionAtomNode>(functionCall.value());
            }
        }
        else if (token.type == TokenType::OpenRoundBracket)
        {
            tokens.pop();
            std::optional<ExpressionNode*> expression = parseExpression(tokens);
            if(!expression.has_value())
            {
                std::cerr << "Expected expression after `(`" << std::endl;
                return std::nullopt;
            }
            if(tokens.empty() || tokens.front().type != TokenType::CloseRoundBracket)
            {
                std::cerr << "Expected `)` after `(`" << std::endl;
                return std::nullopt;
            }
            tokens.pop();

            return allocator.allocate_and_initialize<ExpressionAtomNode>(
                allocator.allocate_and_initialize<ExpressionBracketsNode>(expression.value()));
        }
    }

    return std::nullopt;
}

std::optional<ExpressionNode*> Parser::parseExpressionInternal(std::queue<Token> &tokens, unsigned char minPrecedence)
{
    std::optional<ExpressionAtomNode*> atomLhs = parseExpressionAtom(tokens);
    if (!atomLhs.has_value())
        return std::nullopt;

    ExpressionNode* expressionLhs = allocator.allocate_and_initialize<ExpressionNode>(atomLhs.value());

    while(!tokens.empty())
    {
        Token token = tokens.front();
        std::optional<Operator> operation = tokenTypeToOperator(token.type);
        if(!operation.has_value() || precedenceOfOperator(operation.value()) < minPrecedence)
            break;

        auto nextMinPrecedence = precedenceOfOperator(operation.value()) + 1;
        tokens.pop();
        auto expressionRhs = parseExpressionInternal(tokens, nextMinPrecedence);
        if(!expressionRhs.has_value())
        {
            std::cerr << "Expected expression";
            return std::nullopt;
        }
        auto expression = allocator.allocate_and_initialize<ExpressionBinaryOperatorNode>(
            allocator.allocate_and_initialize<ExpressionNode>(expressionLhs->variant),
            expressionRhs.value(),
            operation.value()
        );
        expressionLhs->variant = expression;
    }

    return expressionLhs;
}

std::optional<ExpressionFunctionCallNode*> Parser::parseExpressionFunctionCall(std::queue<Token>& tokens, Token functionName)
{
    std::vector<ExpressionNode*> arguments;
    while(true)
    {
        if(!tokens.empty() && tokens.front().type == TokenType::CloseRoundBracket)
        {
            tokens.pop();

            return allocator.allocate_and_initialize<ExpressionFunctionCallNode>(
                allocator.allocate_and_initialize<ExpressionIdentNode>(functionName), arguments);
        }
        std::optional<ExpressionNode*> argument = parseExpression(tokens);
        if(argument.has_value())
        {
            arguments.push_back(argument.value());
            
            if (!tokens.empty() && tokens.front().type == TokenType::Comma)
                tokens.pop();
        }
        else
        {
            std::cerr << "Invalid argument to function" << std::endl;
            return std::nullopt;
        }
    }
}

std::optional<StatementDeclareVariableNode*> Parser::parseVariableDeclaration(std::queue<Token> &tokens, ParsingStatementError &error)
{
    auto variableType = tokens.front();

    if (variableType.type == TokenType::KeywordInt || variableType.type == TokenType::KeywordString)
    {
        tokens.pop();

        if (tokens.empty() || tokens.front().type != TokenType::Ident)
        {    
            error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidVariableDeclaration, .metadata = variableType.metadata, .hint = variableType.format() };
            return std::nullopt;
        }
        else
        {
            return allocator.allocate_and_initialize<StatementDeclareVariableNode>(
                    allocator.allocate_and_initialize<ExpressionIdentNode>(variableType),
                    allocator.allocate_and_initialize<ExpressionIdentNode>(tokens.front()));
        }
    }
    return std::nullopt;
}

std::optional<StatementNode*> Parser::parseStatement(std::queue<Token> &tokens, ParsingStatementError &error)
{
    if (!tokens.empty())
    {
        Token firstToken = tokens.front();

        if (firstToken.type == TokenType::KeywordAsm || firstToken.type == TokenType::KeywordInclude)
        {
            tokens.pop();
            
            if (!tokens.empty() && tokens.front().type == TokenType::OpenRoundBracket)
            {
                tokens.pop();

                auto functionCall = parseExpressionFunctionCall(tokens, firstToken);
                if(functionCall.has_value())
                {
                    if(!tokens.empty() && tokens.front().type == TokenType::Semicolon)
                    {
                        tokens.pop();
    
                        auto macroCall = allocator.allocate_and_initialize<StatementNode>(
                            allocator.allocate_and_initialize<StatementMacroNode>(
                                functionCall.value()->functionName, functionCall.value()->arguments
                            )
                        );

                        //delete functionCall.value();
                        return macroCall;
                    }
                    else
                    {
                        //delete functionCall.value();
                        error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidMacroCall, .metadata = firstToken.metadata, .hint = firstToken.format() };
                        return std::nullopt;
                    }
                }
                else
                {
                    error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidMacroCall, .metadata = firstToken.metadata, .hint = firstToken.format() };
                    return std::nullopt;
                }
            }
            else
            {
                error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidMacroCall, .metadata = firstToken.metadata, .hint = firstToken.format() };
                return std::nullopt;
            }
        }
        else if (auto variableDeclaration = parseVariableDeclaration(tokens, error))
        {
            return allocator.allocate_and_initialize<StatementNode>(variableDeclaration.value());
        }
        else if (firstToken.type == TokenType::Ident)
        {
            auto variableIdent = firstToken;

            tokens.pop();

            if(!tokens.empty())
            {
                if(tokens.front().type == TokenType::Semicolon)
                {
                    tokens.pop();
                    return std::nullopt;
                }
                else if (tokens.front().type == TokenType::OpenRoundBracket)
                {
                    tokens.pop();

                    std::optional<ExpressionFunctionCallNode*> functionCall = parseExpressionFunctionCall(tokens, firstToken);
                    if(functionCall.has_value())
                    {
                        if (!tokens.empty() && tokens.front().type == TokenType::Semicolon)
                        {
                            tokens.pop();
                            
                            return allocator.allocate_and_initialize<StatementNode>(functionCall.value());
                        }
                        else
                        {   
                            std::cout << "AAA"; 
                            error = ParsingStatementError { .type = ParsingStatementErrorType::MissingSemicolon, .metadata = firstToken.metadata, .hint = firstToken.format() };
                            return std::nullopt;
                        }
                    }
                    else
                    {
                        error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidFunctionCall, .metadata = firstToken.metadata, .hint = firstToken.format() };
                        return std::nullopt;
                    }
                }
                else if (tokens.front().type == TokenType::EqualSign)
                {
                    tokens.pop();

                    auto expressionNode = parseExpression(tokens);
                    if (expressionNode.has_value())
                    {
                        if (!tokens.empty() && tokens.front().type == TokenType::Semicolon)
                        {
                            tokens.pop();
                            
                            return allocator.allocate_and_initialize<StatementNode>(
                                allocator.allocate_and_initialize<StatementAssignVariableNode>(
                                    allocator.allocate_and_initialize<ExpressionIdentNode>(variableIdent), expressionNode.value()
                                )
                            );
                        }
                        else
                        {   
                            std::cout << "BBB"; 
                            error = ParsingStatementError { .type = ParsingStatementErrorType::MissingSemicolon, .metadata = firstToken.metadata, .hint = firstToken.format() };
                            return std::nullopt;
                        }
                    }
                    else
                    {    
                        error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidExpression, .metadata = firstToken.metadata, .hint = firstToken.format() };
                        return std::nullopt;
                    }
                }
            }
        }
        else if (firstToken.type == TokenType::KeywordIf)
        {
            tokens.pop();

            std::optional<ExpressionNode*> condition = parseExpression(tokens);
            if(condition.has_value())
            {
                std::optional<StatementScopeNode*> ifScope = parseScope(tokens, error);
                
                if(ifScope.has_value())
                {
                    std::optional<StatementScopeNode*> elseScope = std::nullopt;
                    if (!tokens.empty() && tokens.front().type == TokenType::KeywordElse)
                    {
                        tokens.pop();

                        elseScope = parseScope(tokens, error);
                        if(!elseScope.has_value())
                            return std::nullopt;
                    }
                    return allocator.allocate_and_initialize<StatementNode>(
                        allocator.allocate_and_initialize<StatementIfNode>(condition.value(), ifScope.value(), elseScope)
                    );
                }
                else
                {    
                    error = ParsingStatementError { .type = ParsingStatementErrorType::IfStatementDoesntHaveAValidScope, .metadata = firstToken.metadata, .hint = (std::stringstream() << *condition.value()).str() };
                    return std::nullopt;
                }
            }
            else
            {    
                error = ParsingStatementError { .type = ParsingStatementErrorType::IfStatementDoesntHaveAValidCondition, .metadata = firstToken.metadata };
                return std::nullopt;
            }
        }
        else if (firstToken.type == TokenType::KeywordWhile)
        {
            tokens.pop();
            
            std::optional<ExpressionNode*> condition = parseExpression(tokens);
            if(condition.has_value())
            {
                std::optional<StatementScopeNode*> scope = parseScope(tokens, error);
                
                if(scope.has_value())
                {
                    return allocator.allocate_and_initialize<StatementNode>(
                        allocator.allocate_and_initialize<StatementWhileNode>(condition.value(), scope.value()));
                }
                else
                {    
                    error = ParsingStatementError { .type = ParsingStatementErrorType::WhileStatementDoesntHaveAValidScope, .metadata = firstToken.metadata, .hint = (std::stringstream() << *condition.value()).str() };
                    return std::nullopt;
                }
            }
            else
            {    
                error = ParsingStatementError { .type = ParsingStatementErrorType::WhileStatementDoesntHaveAValidCondition, .metadata = firstToken.metadata };
                return std::nullopt;
            }
        }
        else if (firstToken.type == TokenType::KeywordFn)
        {
            tokens.pop();

            if (!tokens.empty() && (tokens.front().type == TokenType::KeywordInt || tokens.front().type == TokenType::KeywordString))
            {
                auto returnIdent = tokens.front();
                tokens.pop();

                if (!tokens.empty() && tokens.front().type == TokenType::Ident)
                {
                    auto nameIdent = tokens.front();
                    tokens.pop();

                    if (!tokens.empty() && tokens.front().type == TokenType::OpenRoundBracket)
                    {
                        tokens.pop();

                        std::vector<StatementDeclareVariableNode*> parameters;
                        while(true)
                        {
                            if(!tokens.empty() && tokens.front().type == TokenType::CloseRoundBracket)
                            {
                                tokens.pop();

                                std::optional<StatementScopeNode*> scope = parseScope(tokens, error);
                                if(scope.has_value())
                                {
                                    return allocator.allocate_and_initialize<StatementNode>(allocator.allocate_and_initialize<StatementFunctionDefinitionNode>(
                                        allocator.allocate_and_initialize<ExpressionIdentNode>(returnIdent),
                                        allocator.allocate_and_initialize<ExpressionIdentNode>(nameIdent), parameters,
                                        scope.value()));
                                }
                                else
                                {
                                    std::cerr << "Invalid scope of function" << std::endl;
                                    return std::nullopt;
                                }
                            }
                            std::optional<StatementDeclareVariableNode*> parameter = parseVariableDeclaration(tokens, error);
                            if(parameter.has_value())
                            {
                                tokens.pop();

                                parameters.push_back(parameter.value());
                                
                                if (!tokens.empty() && tokens.front().type == TokenType::Comma)
                                    tokens.pop();
                            }
                            else
                            {
                                std::cerr << "Invalid parameter of function" << std::endl;
                                return std::nullopt;
                            }
                        }
                    }
                    else
                    {
                        error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidFunctionDefinitionOpenBracket, .metadata = firstToken.metadata, .hint = nameIdent.format() };
                        return std::nullopt;
                    }
                }
                else
                {
                    error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidFunctionDefinitionName, .metadata = firstToken.metadata, .hint = returnIdent.format() };
                    return std::nullopt;
                }
            }
            else
            {
                error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidFunctionDefinitionReturn, .metadata = firstToken.metadata };
                return std::nullopt;
            }
        }
        else if (firstToken.type == TokenType::KeywordReturn)
        {
            tokens.pop();

            auto expressionNode = parseExpression(tokens);
            if (expressionNode.has_value())
            {
                if (!tokens.empty() && tokens.front().type == TokenType::Semicolon)
                {
                    tokens.pop();
                    return allocator.allocate_and_initialize<StatementNode>(
                        allocator.allocate_and_initialize<StatementReturnNode>(expressionNode.value()));
                }
                else
                {    
                    error = ParsingStatementError { .type = ParsingStatementErrorType::MissingSemicolon, .metadata = firstToken.metadata, .hint = (std::stringstream() << *expressionNode.value()).str() };
                    return std::nullopt;
                }
            }
            else
            {
                error = ParsingStatementError { .type = ParsingStatementErrorType::InvalidExpression, .metadata = firstToken.metadata };
                return std::nullopt;
            }
        }
        else if (firstToken.type == TokenType::OpenCurlyBracket)
        {
            auto scope = parseScope(tokens, error);
            if(scope.has_value())
                return allocator.allocate_and_initialize<StatementNode>(scope.value());
            else
                return std::nullopt;
        }

        if (tokens.front().type == TokenType::Comment)
            tokens.pop();

        if (tokens.front().type == TokenType::Semicolon)
            tokens.pop();
    }

    return std::nullopt;
}

std::optional<StatementScopeNode*> Parser::parseScope(std::queue<Token> &tokens, ParsingStatementError &error)
{
    if (!tokens.empty() && tokens.front().type == TokenType::OpenCurlyBracket)
    {
        auto firstToken = tokens.front();
        tokens.pop();
        std::vector<StatementNode*> statementsInsideScope;
        while(!tokens.empty() && tokens.front().type != TokenType::CloseCurlyBracket)
        {
            std::optional<StatementNode*> statement = parseStatement(tokens, error);
            if(statement.has_value())
                statementsInsideScope.push_back(statement.value());
        }
        if(tokens.empty() || tokens.front().type != TokenType::CloseCurlyBracket)
        {
            error = ParsingStatementError { .type = ParsingStatementErrorType::ScopeNotClosed, .metadata = firstToken.metadata, .hint = (std::stringstream() << *allocator.allocate_and_initialize<StatementScopeNode>(statementsInsideScope)).str() };
            return std::nullopt;
        }

        tokens.pop();

        return allocator.allocate_and_initialize<StatementScopeNode>(statementsInsideScope);
    }

    return std::nullopt;
}