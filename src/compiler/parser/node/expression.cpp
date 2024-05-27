#include "expression.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const ExpressionLiteralNode &node)
{
    os << "ExpressionLiteralNode " << node.literal.format();
    return os;
}

std::ostream& operator<<(std::ostream& os, const ExpressionIdentNode &node)
{
    os << "ExpressionIdentNode " << node.ident.format();
    return os;
}

std::ostream& operator<<(std::ostream& os, const ExpressionFunctionCallNode &node)
{
    os << "ExpressionFunctionCallNode " << *node.functionName << " Arguments: ";
    for(auto argument : node.arguments)
        os << *argument << " ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ExpressionBracketsNode &node)
{
    os << "ExpressionBracketsNode " << *node.expression;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ExpressionAtomNode &node)
{
    std::visit([&](auto arg)
    {
        os << *arg;
    }, node.variant);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ExpressionBinaryOperatorNode &node)
{
    os << "ExpressionBinaryOperatorNode " << *node.lhs << " " << formatOperator(node.operation) << " " << *node.rhs;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ExpressionNode &node)
{
    std::visit([&](auto arg)
    {
        os << *arg;
    }, node.variant);
    return os;
}

unsigned char formatOperator(Operator operation)
{
    switch(operation)
    {
        case Operator::Add:
            return '+';
        case Operator::Sub: 
            return '-';
        case Operator::Mul: 
            return '*';
        case Operator::Div: 
            return '/'; 
        case Operator::GreaterThan: 
            return '>';
        case Operator::LessThan: 
            return '<'; 
        case Operator::EqualTo: 
            return '=='; 
        case Operator::NotEqualTo: 
            return '!='; 
    }

    return 0;
}

unsigned char precedenceOfOperator(Operator operation)
{
    switch(operation)
    {
        case Operator::Add:
            return 2;
        case Operator::Sub:
            return 2;
        case Operator::Mul:
            return 3;
        case Operator::Div:
            return 3;
        case Operator::GreaterThan:
            return 1;
        case Operator::LessThan:
            return 1;
        case Operator::EqualTo:
            return 1;
        case Operator::NotEqualTo:
            return 1;
    }

    std::cerr << "Invalid operator!" << std::endl;
    exit(255);
}