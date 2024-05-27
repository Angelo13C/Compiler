#pragma once

#include <vector>
#include <optional>
#include <queue>

#include "../token/token.hpp"
#include "node/core.hpp"
#include "node/expression.hpp"
#include "node/statement.hpp"
#include "error.hpp"
#include "../../utils/arena_allocator.hpp"

/**
 * @class Parser
 * @brief The Parser class is responsible for syntactic analysis, transforming a sequence of tokens into an abstract syntax tree (AST).
 *
 * The Parser takes a vector of tokens as input and generates an AST representing the syntactic structure of the source code.
 */
class Parser
{
public:
    /**
     * @brief Default constructor for the Parser class.
     *
     * Initializes a Parser object.
     */
    Parser();

    /**
     * @brief Parses a sequence of tokens and generates an abstract syntax tree (AST) for the source code.
     *
     * This method takes a vector of tokens as input and performs syntactic analysis to create an AST.
     *
     * @param tokens The vector of tokens representing the source code.
     * @return A ProgramNode representing the abstract syntax tree (AST) of the source code.
     */
    ProgramNode parse(std::vector<Token> tokens);

private:
    /**
     * @brief Parses an expression from a queue of tokens and returns the corresponding ExpressionNode.
     *
     * This method is responsible for parsing expressions, which are building blocks for the abstract syntax tree (AST).
     *
     * @param tokens A queue of tokens to be parsed.
     * @return An optional ExpressionNode representing the parsed expression, or std::nullopt if parsing fails.
     */
    std::optional<ExpressionNode*> parseExpression(std::queue<Token>& tokens);
    std::optional<ExpressionNode*> parseExpressionInternal(std::queue<Token>& tokens, unsigned char minPrecedence);
    std::optional<ExpressionAtomNode*> parseExpressionAtom(std::queue<Token>& tokens);
    
    std::optional<ExpressionFunctionCallNode*> parseExpressionFunctionCall(std::queue<Token>& tokens, Token functionName);

    std::optional<StatementNode*> parseStatement(std::queue<Token> &tokens, ParsingStatementError &error);
    std::optional<StatementScopeNode*> parseScope(std::queue<Token> &tokens, ParsingStatementError &error);
    
    std::optional<StatementDeclareVariableNode*> parseVariableDeclaration(std::queue<Token> &tokens, ParsingStatementError &error);

private:
    ArenaAllocator allocator;
};