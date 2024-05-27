#include "tokenizer.hpp"
#include "token_hint.hpp"

Tokenizer::Tokenizer()
{

}

static void parseToken(ParsingToken& parsingToken, std::vector<Token>& tokens, Meta metadata)
{
    std::optional<Token> newToken = parsingToken.intoResultingToken(metadata);
    if(newToken.has_value())
        tokens.push_back(newToken.value());
}

std::vector<Token> Tokenizer::tokenize(const std::string& string)
{
    std::vector<Token> tokens;

    ParsingToken parsingToken = ParsingToken();

    size_t lineNumber = 0;
    size_t columnNumber = 0;
    for (char c : string)
    {
        columnNumber++;

        if(c == '\n')
        {
            lineNumber++;
            columnNumber = 0;
        }

        parsingToken.addCharacter(c);
        parseToken(parsingToken, tokens, Meta { .lineNumber = lineNumber, .columnNumber = columnNumber});
        parseToken(parsingToken, tokens, Meta { .lineNumber = lineNumber, .columnNumber = columnNumber});
    }
    
    parsingToken.addCharacter(' ');
    parseToken(parsingToken, tokens, Meta { .lineNumber = lineNumber, .columnNumber = columnNumber});

    return tokens;
}