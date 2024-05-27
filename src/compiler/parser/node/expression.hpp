#pragma once

#include <variant>
#include <ostream>
#include <vector>

#include "../../token/token.hpp"

struct ExpressionNode;

/**
 * @struct ExpressionLiteralNode
 * @brief A structure representing an expression node containing a literal token.
 *
 * The ExpressionLiteralNode structure is used in the ExpressionNode to represent an expression
 * containing a literal, such as a numeric or string literal.
 */
struct ExpressionLiteralNode
{
    Token literal;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionLiteralNode &node);

/**
 * @struct ExpressionIdentNode
 * @brief A structure representing an expression node containing an identifier token.
 *
 * The ExpressionIdentNode structure is used in the ExpressionNode to represent an expression
 * containing an identifier, such as a variable or function name.
 */
struct ExpressionIdentNode
{
    Token ident;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionIdentNode &node);

struct ExpressionFunctionCallNode
{
    ExpressionIdentNode* functionName;
    std::vector<ExpressionNode*> arguments;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionFunctionCallNode &node);

struct ExpressionBracketsNode
{
    ExpressionNode* expression;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionBracketsNode &node);

enum class Operator
{
    Add,
    Sub,
    Mul,
    Div,
    GreaterThan,
    LessThan,
    EqualTo,
    NotEqualTo
};

struct ExpressionBinaryOperatorNode
{
    ExpressionNode* lhs;
    ExpressionNode* rhs;
    Operator operation;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionBinaryOperatorNode &node);

struct ExpressionAtomNode
{
    std::variant<ExpressionLiteralNode*, ExpressionIdentNode*, ExpressionBracketsNode*, ExpressionFunctionCallNode*> variant;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionAtomNode &node);

/**
 * @struct ExpressionNode
 * @brief A structure representing a generic expression node in an abstract syntax tree (AST).
 *
 * The ExpressionNode structure serves as a wrapper for different types of expression nodes,
 * including literals and identifiers, using std::variant for flexibility.
 */
struct ExpressionNode
{
    std::variant<ExpressionAtomNode*, ExpressionBinaryOperatorNode*> variant;
};

/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ExpressionNode &node);

unsigned char formatOperator(Operator operation);
unsigned char precedenceOfOperator(Operator operation);