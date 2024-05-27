#pragma once
#include <iostream>
#include <string>

#include "settings.hpp"
#include "token/tokenizer.hpp"
#include "parser/parser.hpp"
#include "generation/generator.hpp"

/**
 * @class Compiler
 * @brief The Compiler class orchestrates the compilation process.
 *
 * The Compiler class takes care of tokenization, parsing, and code generation
 * to produce executable code from a given input source code.
 */
class Compiler
{
public:
    /**
     * @brief Constructor for the Compiler class.
     * @param tokenizer The tokenizer is responsible for lexical analysis.
     * @param parser The parser is responsible for syntactic analysis.
     * @param generator The code generator is responsible for producing executable code.
     */
    Compiler(Tokenizer tokenizer, Parser parser, Generator generator, CompilerSettings settings);

    /**
     * @brief Compiles the given source code and returns the result.
     * @param input The source code to be compiled.
     * @return The compiled code as a string.
     */
    std::string compile(const std::string& input);
    
    /**
     * @brief Compiles the source code from a file and writes the result to another file.
     * @param inputFilePath The path to the source code file.
     * @param outputFilePath The path to the output file for the compiled code.
     * @return The exit code of the compilation process.
     */
    int compileAndWriteToFile(const std::string& inputFilePath, const std::string& outputFilePath);

private:
    /**
     * @brief Reads the contents of a file.
     * @param filePath The path to the file to be read.
     * @return The contents of the file as a string.
     */
    std::string readFile(const std::string& filePath);
    /**
     * @brief Writes the output string to a file.
     * @param output The string to be written to the file.
     * @param fileName The name of the output file.
     */
    void writeOutputToFile(const std::string& output, const std::string& fileName);

    Tokenizer tokenizer;
    Parser parser;
    Generator generator;

    CompilerSettings settings;
};