#include "cli.hpp"

CLIArguments::CLIArguments(int argc, char* argv[]) : pathToFileToCompile(pathToFileToCompile)
{
    if(argc != 2)
    {
        std::cerr << "You need to pass exactly 1 parameter to this program" << std::endl;
        std::cerr << "Parameter: [PATH_TO_FILE_TO_COMPILE]" << std::endl;
        std::cerr << "Example:" << std::endl;
        std::cerr << "Compiler.exe my_program.bc" << std::endl;

        pathToFileToCompile = nullptr;

        return;
    }

    pathToFileToCompile = argv[1];
}

char* CLIArguments::getPathToFileToCompile()
{
    return pathToFileToCompile;
}