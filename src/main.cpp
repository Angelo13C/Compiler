#include <iostream>

#include "cli/cli.hpp"

int main(int argc, char* argv[])
{
    CLIArguments cliArguments = CLIArguments(argc, argv);

    char* pathToFileToCompile = cliArguments.getPathToFileToCompile();
    if(pathToFileToCompile != nullptr)
    {
    }

    std::cin.get();

    return 0;
}