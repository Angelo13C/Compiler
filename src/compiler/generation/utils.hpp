#pragma once

#include <string>
#include <sstream>

#include "generation_data.hpp"

namespace utils
{
    /**
     * @brief Generate assembly code for exiting the process with a specified exit code.
     * @param exitCode The exit code to use in the assembly code.
     */
    void generateExitCode(const std::string &exitCode, std::stringstream &output);

    std::string accessVariable(const GenerateData& generation, const Variable &variable);
    
    void useStdout(GenerateData& generation);
    void useStdin(GenerateData& generation);
    void useHeapAllocation(GenerateData& generation);
    void getStdoutHandle(GenerateData& generation);
    void getStdinHandle(GenerateData& generation);
    void getHeapHandle(GenerateData& generation);
}