#include "utils.hpp"

#include "special/consts.hpp"

void utils::generateExitCode(const std::string &exitCode, std::stringstream &output)
{
    output << TAB << "mov rcx, " << exitCode << NEW_LINE;
    output << TAB << "call " << EXIT_PROCESS_WINDOWS << NEW_LINE;
}

std::string utils::accessVariable(const GenerateData& generation, const Variable &variable)
{
    std::stringstream output;
    output << "QWORD [rsp + " << (generation.stackSize - variable.stackPtr - 1) * 8 << "]";
    return output.str();
}

void utils::useStdout(GenerateData& generation)
{
    generation.output << TAB << "extern GetStdHandle" << NEW_LINE << TAB << "extern WriteFile" << NEW_LINE;
}
void utils::useStdin(GenerateData& generation)
{
    generation.output << TAB << "extern ReadFile" << NEW_LINE;
}
void utils::useHeapAllocation(GenerateData& generation)
{
    generation.dataSection << TAB << "heapHandle dq 0" << NEW_LINE;
    generation.output << TAB << "extern GetProcessHeap" << NEW_LINE << TAB << "extern HeapAlloc" << NEW_LINE << TAB << "extern HeapFree" << NEW_LINE;
}
void utils::getStdoutHandle(GenerateData& generation)
{
    generation.output << TAB << "; Get stdout" << NEW_LINE << TAB << "mov rcx, -11" << NEW_LINE << TAB << "call GetStdHandle" << NEW_LINE << TAB << "mov [rel stdout], rax" << NEW_LINE;
}
void utils::getStdinHandle(GenerateData& generation)
{
    generation.output << TAB << "; Get stdin" << NEW_LINE << TAB << "mov rcx, -10" << NEW_LINE << TAB << "call GetStdHandle" << NEW_LINE << TAB << "mov [rel stdin], rax" << NEW_LINE;
}
void utils::getHeapHandle(GenerateData& generation)
{
    generation.output << TAB << "; Get heap handle" << NEW_LINE << TAB << "call GetProcessHeap" << NEW_LINE << TAB << "mov [rel heapHandle], rax" << NEW_LINE;
}