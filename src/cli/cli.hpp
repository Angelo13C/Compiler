#pragma once
#include <iostream>

/**
 * @struct CLIArguments
 * @brief A structure representing command-line interface (CLI) arguments for the compiler program.
 *
 * The CLIArguments structure parses and stores command-line arguments, providing access to relevant values.
 */
struct CLIArguments
{
public:
    /**
     * @brief Constructor for CLIArguments.
     *
     * Initializes a CLIArguments object by parsing the command-line arguments.
     *
     * @param argc The number of command-line arguments.
     * @param argv An array of strings representing the command-line arguments.
     */
    CLIArguments(int argc, char* argv[]);

    /**
     * @brief Retrieves the path to the file to be compiled.
     *
     * @return A pointer to the string representing the path to the file to be compiled.
     */
    char* getPathToFileToCompile();

private:
    char* pathToFileToCompile;
};