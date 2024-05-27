#include "statement.hpp"

std::ostream& operator<<(std::ostream& os, const StatementReturnNode &node)
{
    os << "StatementReturnNode " << *node.expression;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementDeclareVariableNode &node)
{
    os << "StatementDeclareVariableNode " << *node.type << " " << *node.name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementAssignVariableNode &node)
{
    os << "StatementAssignVariableNode " << *node.name << " = " << *node.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementScopeNode &node)
{
    os << "StatementScopeNode START" << std::endl;
    for(auto statement : node.statements)
    {
        os << *statement << std::endl;
    }
    os << "StatementScopeNode END" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementIfNode &node)
{
    os << "StatementIfNode if (" << *node.condition << ") then " << *node.scope;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementWhileNode &node)
{
    os << "StatementWhileNode while (" << *node.condition << ") then " << *node.scope;
    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementFunctionDefinitionNode &node)
{
    os << "StatementFunctionDefinitionNode " << *node.returnType << " " << *node.functionName << " (";
    for(auto parameter : node.parameters)
    {
        os << *parameter << ", ";
    }
    os << ")" << *node.implementation;

    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementMacroNode &node)
{
    os << "StatementMacroNode " << *node.macroName << "! (" << std::endl;
    for(auto parameter : node.arguments)
    {
        os << *parameter << ", ";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const StatementNode &node)
{
    std::visit([&](auto& arg)
    {
        os << *arg;
    }, node.variant);
    return os;
}