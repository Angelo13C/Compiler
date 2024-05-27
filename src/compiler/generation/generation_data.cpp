#include "generation_data.hpp"

#include <algorithm>

std::string codeGenerationErrorToString(CodeGenerationError error)
{
    switch (error.type)
    {
    case CodeGenerationErrorType::VariableAlreadyDefined:
        return "VariableAlreadyDefined: " + error.hint;
    case CodeGenerationErrorType::UndeclaredVariable:
        return "UndeclaredVariable: " + error.hint;
    }

    return "";
}

std::string GenerateData::convertToProgram()
{
    for(auto stringLiteral : stringLiterals)
    {
        auto stringLiteralName = getNameOfStringLiteral(stringLiteral).value();
        roDataSection << TAB << stringLiteralName << " db `" << stringLiteral << "`, 0" << NEW_LINE;
        roDataSection << TAB << stringLiteralName << "_len equ $-" << stringLiteralName << NEW_LINE;
    }

    auto dataSectionStr = dataSection.str();
    auto roDataSectionStr = roDataSection.str();

    std::string program = "";
    if(!dataSectionStr.empty())
        program += "section .data" + NEW_LINE + dataSectionStr + NEW_LINE;
    if(!roDataSectionStr.empty())
        program += "section .rodata" + NEW_LINE + roDataSectionStr + NEW_LINE;
        
    return program + output.str();
}

void GenerateData::enterScope()
{
    Scope* newScope = new Scope
    {
        .startStackPtr = stackSize,
        .parentScope = currentScope
    };
    currentScope->innerScopes.push_back(newScope);
    currentScope = newScope;
}

void GenerateData::exitScope()
{
    auto variablesToRemove = stackSize - currentScope->startStackPtr;
    stackSize = currentScope->startStackPtr;
    output << TAB << "add rsp, " << variablesToRemove * 8 << NEW_LINE;
    currentScope = currentScope->parentScope;
}

void GenerateData::defineVariable(const std::string &variableName)
{
    defineVariableWithOffsetStack(variableName, 0);
}

void GenerateData::defineVariableWithOffsetStack(const std::string &variableName, size_t offsetStack)
{
    currentScope->definedVariables[variableName] = Variable{.stackPtr = stackSize - offsetStack};
}

std::optional<Variable> GenerateData::getVariableByName(const std::string &variableName)
{
    Scope* scopeBeingSearched = currentScope;
    while(scopeBeingSearched != nullptr)
    {
        auto variable = scopeBeingSearched->definedVariables.find(variableName);
        if(variable != scopeBeingSearched->definedVariables.end())
            return variable->second;

        scopeBeingSearched = scopeBeingSearched->parentScope;
    }

    return std::nullopt;
}

bool GenerateData::doesVariableExist(const std::string &variableName)
{
    return getVariableByName(variableName).has_value();
}

void GenerateData::defineFunction(Function function)
{
    currentScope->definedFunctions[function.name] = std::move(function);
}
void GenerateData::callFunction(Function function)
{
    currentScope->calledFunctions.push_back(std::move(function));
}
bool isFunctioCallValid(Function functionCall, Scope* scopeBeingSearched)
{
    while(scopeBeingSearched != nullptr)
    {
        auto functionDefinitionPair = scopeBeingSearched->definedFunctions.find(functionCall.name);
        if(functionDefinitionPair != scopeBeingSearched->definedFunctions.end())
        {
            auto functionDefinition = functionDefinitionPair->second;
            // Do the function call and definition have the same number of parameters?
            if(functionDefinition.parameters.size() != functionCall.parameters.size())
                return false;
            // Do all the parameters of the function call and definition have the same type?
            for(int i = 0; i < functionDefinition.parameters.size(); i++)
            {
                if(functionDefinition.parameters[i]->type->ident.value != functionCall.parameters[i]->type->ident.value)
                    return false;
            }
            return true;
        }

        scopeBeingSearched = scopeBeingSearched->parentScope;
    }

    return true;
}
std::optional<Function> GenerateData::checkIfFunctionCallsAreValid()
{
    Scope* scopeBeingSearched = currentScope;
    while(scopeBeingSearched != nullptr)
    {
        for(auto functionCall : scopeBeingSearched->calledFunctions)
        {
            if(!isFunctioCallValid(functionCall, scopeBeingSearched))
                return functionCall;
        }

        scopeBeingSearched = scopeBeingSearched->parentScope;
    }

    return std::nullopt;
}

std::optional<std::string> GenerateData::getNameOfStringLiteral(const std::string &stringLiteral)
{
    auto it = std::find(stringLiterals.begin(), stringLiterals.end(), stringLiteral);
    if (it == stringLiterals.end())
        return std::nullopt;
    else
    {
        auto index = std::distance(stringLiterals.begin(), it);
        return "strLit" +  std::to_string(index);
    }
}

std::string GenerateData::defineStringLiteral(const std::string &stringLiteral)
{
    // Avoid redefining an already defined string literal
    if(std::find(stringLiterals.begin(), stringLiterals.end(), stringLiteral) == stringLiterals.end())
        stringLiterals.push_back(stringLiteral);
    
    return getNameOfStringLiteral(stringLiteral).value();
}

void GenerateData::pushOnStack(const std::string &registerName)
{
    output << TAB << "push " << registerName << NEW_LINE;
    stackSize++;
}

void GenerateData::popFromStack(const std::string &registerName)
{
    output << TAB << "pop " << registerName << NEW_LINE;
    stackSize--;
}
std::string GenerateData::generateLabel()
{
    return "label" + std::to_string(labelCount++);
}
