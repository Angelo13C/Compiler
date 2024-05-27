#pragma once

#include <vector>
#include <ostream>

#include "statement.hpp"

/**
 * @struct ProgramNode
 * @brief A structure representing the root node of an abstract syntax tree (AST) for a program.
 *
 * The ProgramNode structure serves as the top-level node in the AST, containing a vector of StatementNodes
 * representing the statements in the program.
 */
struct ProgramNode
{
    std::vector<StatementNode*> nodes;
};
    
/**
 * @brief Formats the node as a string for debugging purposes.
 */
std::ostream& operator<<(std::ostream& os, const ProgramNode &programNode);