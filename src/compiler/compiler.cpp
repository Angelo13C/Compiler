#include "compiler.hpp"
#include "token/token.hpp"

#include <fstream>
#include <sstream>

Compiler::Compiler(Tokenizer tokenizer, Parser parser, Generator generator, CompilerSettings settings): 
    tokenizer(tokenizer), parser(std::move(parser)), generator(generator), settings(settings)
{
    
}

static void logSection(const std::string& sectionName)
{
    const std::string PREFIX = "----------";
    const std::string SUFFIX = PREFIX;
    
    std::cout << std::endl << PREFIX << sectionName << SUFFIX << std::endl << std::endl;
}

std::string Compiler::compile(const std::string& input)
{
    logSection("Tokenizing");
    std::vector<Token> tokens = tokenizer.tokenize(input);
    if(settings.showTokenizerOutput)
    {
        for (Token token : tokens)
        {
            std::cout << token.format() << std::endl;
        }
    }

    logSection("Parsing");
    ProgramNode program = parser.parse(tokens);
    if(program.nodes.empty())
        return "";
    if(settings.showParserOutput)
        std::cout << program;

    logSection("Generating output");
    std::string output = generator.generate(program);

    if(settings.showGeneratorOutput)
        std::cout << "Output:\n" << output;

    return output;
}

int Compiler::compileAndWriteToFile(const std::string& inputFilePath, const std::string& outputFilePath)
{
    std::cout << "Compiling: " << inputFilePath << std::endl << std::endl;

    std::string input = readFile(inputFilePath);

    std::string output = compile(input);

    writeOutputToFile(output, outputFilePath);

    return 0;
}

std::string Compiler::readFile(const std::string& filePath)
{
    std::stringstream contentStream;
    std::fstream fileStream(filePath, std::ios::in);
    contentStream << fileStream.rdbuf();
    std::string content = contentStream.str();
    fileStream.close();

    return content;
}

void Compiler::writeOutputToFile(const std::string& output, const std::string& fileName)
{
    std::fstream file(fileName, std::ios::out);
    file << output;
    file.close();
}