#include <iostream>

#include "cli/cli.hpp"
#include "compiler/compiler.hpp"

int main(int argc, char* argv[])
{
    CLIArguments cliArguments = CLIArguments(argc, argv);

    char* pathToFileToCompile = cliArguments.getPathToFileToCompile();
    if(pathToFileToCompile != nullptr)
    {
        Compiler compiler = Compiler(Tokenizer(), Parser(), Generator(), CompilerSettings {
            .showTokenizerOutput = true,
            .showParserOutput = true,
            .showGeneratorOutput = true
        });
        int compileStatus = compiler.compileAndWriteToFile(pathToFileToCompile, "out.asm");
        if(compileStatus != 0)
            return compileStatus;
    }

    std::cin.get();

    return 0;
}