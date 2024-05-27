#pragma once

#include <variant>
#include <sstream>
#include <vector>

#include "expression.hpp"

struct StatementNode;

/**
 * @struct StatementReturnNode
 * @brief A structure representing a return statement node in an abstract syntax tree (AST).
 *
 * The StatementReturnNode structure is used in the StatementNode to represent a return statement
 * with an associated expression.
 */
struct StatementReturnNode
{
    ExpressionNode* expression;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementReturnNode &node);

/**
 * @struct StatementDeclareVariableNode
 * @brief A structure representing a variable declaration statement node in an abstract syntax tree (AST).
 *
 * The StatementDeclareVariableNode structure is used in the StatementNode to represent a statement
 * declaring a variable with an associated identifier.
 */
struct StatementDeclareVariableNode
{
    ExpressionIdentNode* type;
    ExpressionIdentNode* name;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementDeclareVariableNode &node);

struct StatementAssignVariableNode
{
    ExpressionIdentNode* name;
    ExpressionNode* value;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementAssignVariableNode &node);

struct StatementScopeNode
{
    std::vector<StatementNode*> statements;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementScopeNode &node);

struct StatementIfNode
{
    ExpressionNode* condition;
    StatementScopeNode* scope;
    std::optional<StatementScopeNode*> elseScope;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementIfNode &node);

struct StatementWhileNode
{
    ExpressionNode* condition;
    StatementScopeNode* scope;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementWhileNode &node);

struct StatementFunctionDefinitionNode
{
    ExpressionIdentNode* returnType;
    ExpressionIdentNode* functionName;
    std::vector<StatementDeclareVariableNode*> parameters;    
    StatementScopeNode* implementation;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementFunctionDefinitionNode &node);

struct StatementMacroNode
{
    ExpressionIdentNode* macroName;
    std::vector<ExpressionNode*> arguments;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementMacroNode &node);

/**
 * @struct StatementNode
 * @brief A structure representing a generic statement node in an abstract syntax tree (AST).
 *
 * The StatementNode structure serves as a wrapper for different types of statement nodes,
 * including return statements and variable declarations, using std::variant for flexibility.
 */
struct StatementNode
{
    std::variant<StatementReturnNode*, StatementDeclareVariableNode*, StatementAssignVariableNode*, 
                StatementScopeNode*, StatementIfNode*, StatementWhileNode*, StatementFunctionDefinitionNode*, ExpressionFunctionCallNode*,
                StatementMacroNode*> variant;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const StatementNode &node);