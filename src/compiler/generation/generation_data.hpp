#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <optional>

#include "special/consts.hpp"
#include "../parser/node/statement.hpp"
#include "special/function.hpp"
#include "error.hpp"

std::string codeGenerationErrorToString(CodeGenerationError error);

struct Variable;

/**
 * @brief Structure representing information about a scope during code generation.
 */
struct Scope
{
    size_t startStackPtr;
    std::unordered_map<std::string, Variable> definedVariables;
    std::unordered_map<std::string, Function> definedFunctions;
    std::vector<Function> calledFunctions;
    std::vector<Scope*> innerScopes;
    Scope* parentScope;
};

/**
 * @brief Structure representing information about a variable during code generation.
 */
struct Variable
{
    size_t stackPtr;
};

struct FunctionDefinition
{
    Scope* scope;
    size_t parametersCount;
};

/**
 * @brief Structure containing data for code generation and related functions.
 */
struct GenerateData
{
    GenerateData() : output(std::stringstream()), stackSize(0), globalScope(Scope { }), currentScope(&globalScope), labelCount(0) {}

    std::stringstream output;
    std::stringstream dataSection;
    std::stringstream roDataSection;
    std::vector<std::string> stringLiterals;
    std::vector<CodeGenerationError> errors;
    size_t stackSize;

    Scope globalScope;
    Scope* currentScope;
    std::optional<FunctionDefinition> currentFunctionDefinition;
    
    unsigned int labelCount;

    std::string convertToProgram();

    void enterScope();
    void exitScope();

    void defineVariable(const std::string &variableName);
    void defineVariableWithOffsetStack(const std::string &variableName, size_t offsetStack);
    
    std::optional<Variable> getVariableByName(const std::string &variableName);

    /**
     * @brief Check if a variable with the given name already exists.
     * @param variableName The name of the variable.
     * @return True if the variable exists, false otherwise.
     */
    bool doesVariableExist(const std::string &variableName);

    void defineFunction(Function function);
    void callFunction(Function function);
    std::optional<Function> checkIfFunctionCallsAreValid();

    std::optional<std::string> getNameOfStringLiteral(const std::string &stringLiteral);
    std::string defineStringLiteral(const std::string &stringLiteral);

    /**
     * @brief Push a register onto the stack and update stack size.
     * @param registerName The name of the register to push.
     */
    void pushOnStack(const std::string &registerName);

    /**
     * @brief Pop a register from the stack and update stack size.
     * @param registerName The name of the register to pop.
     */
    void popFromStack(const std::string &registerName);

    std::string generateLabel();
};