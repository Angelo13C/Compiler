#include "core.hpp"

std::ostream& operator<<(std::ostream& os, const ProgramNode &programNode)
{
    os << "Start program" << std::endl;
    for(const auto node : programNode.nodes)
        os << *node << std::endl;

    os << "End program" << std::endl;

    return os;
}