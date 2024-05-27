#pragma once

#include <string>

#include "generation_data.hpp"
#include "../parser/node/core.hpp"

/**
 * @brief Class responsible for generating assembly code based on parsed program nodes.
 */
class Generator
{
public:
    /**
     * @brief Default constructor for the Generator class.
     *
     * Initializes a Generator object.
     */
    Generator();

    /**
     * @brief Generate assembly code for the entire program.
     * @param program The root node of the parsed program.
     * @return The generated assembly code as a string.
     */
    std::string generate(const ProgramNode& program);

private:
    /**
     * @brief Generate assembly code for a statement.
     * @param statement The statement node to generate code for.
     * @param generation Reference to the GenerateData object containing code generation information.
     */
    void generateStatement(const StatementNode* statement, GenerateData& generation);

    /**
     * @brief Generate assembly code for a scope statement.
     * @param statement The statement scope node to generate code for.
     * @param generation Reference to the GenerateData object containing code generation information.
     */
    void generateStatementScope(const StatementScopeNode* statement, GenerateData& generation);
    /**
     * @brief Generate assembly code for a scope statement without entering in the scope.
     * @param statement The statement scope node to generate code for.
     * @param generation Reference to the GenerateData object containing code generation information.
     */
    void generateStatementScopeWithoutEntering(const StatementScopeNode* statement, GenerateData& generation);

    /**
     * @brief Generate assembly code for an expression.
     * @param expression The expression node to generate code for.
     * @param generation Reference to the GenerateData object containing code generation information.
     */
    void generateExpression(const std::string& registerName, const ExpressionNode* expression, GenerateData& generation);

    /**
     * @brief Generate assembly code for an atom of an expression.
     * @param expression The expression node to generate code for.
     * @param generation Reference to the GenerateData object containing code generation information.
     */
    void generateExpressionAtom(const std::string& registerName, const ExpressionAtomNode* expression, GenerateData& generation);
};