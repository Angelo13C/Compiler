#pragma once
#include <unordered_map>
#include <string>

enum ParsingStatementErrorType
{
    None,

    MissingSemicolon,
    InvalidExpression,
    InvalidVariableDeclaration,
    ScopeNotClosed,
    IfStatementDoesntHaveAValidCondition,
    IfStatementDoesntHaveAValidScope,
    WhileStatementDoesntHaveAValidCondition,
    WhileStatementDoesntHaveAValidScope,
    InvalidFunctionDefinitionReturn,
    InvalidFunctionDefinitionName,
    InvalidFunctionDefinitionOpenBracket,
    InvalidFunctionCall,
    InvalidMacroCall,
};

/**
 * @brief A map associating parsing statement errors with corresponding string error.
 */
const std::unordered_map<ParsingStatementErrorType, std::string> parsingErrorToString = {
    { None, "" },
    { MissingSemicolon, "Missing semicolon!" },
    { InvalidExpression, "Invalid expression!" },
    { InvalidVariableDeclaration, "Invalid variable declaration!" },
    { ScopeNotClosed, "Scope not closed!" },
    { IfStatementDoesntHaveAValidCondition, "If statement doesn't have a valid condition!" },
    { IfStatementDoesntHaveAValidScope, "If statement doesn't have a valid scope!" },
    { WhileStatementDoesntHaveAValidCondition, "While statement doesn't have a valid condition!" },
    { WhileStatementDoesntHaveAValidScope, "While statement doesn't have a valid scope!" },
    { InvalidFunctionDefinitionReturn, "The function definition doesn't have a valid return type!" },
    { InvalidFunctionDefinitionName, "The function definition doesn't have a valid name!" },
    { InvalidFunctionDefinitionOpenBracket, "The function definition doesn't open the bracket!" },
    { InvalidFunctionCall, "Invalid function call!" },
    { InvalidMacroCall, "Invalid macro call!" }
};

struct ParsingStatementError
{
    ParsingStatementErrorType type;
    Meta metadata;
    std::string hint;
};