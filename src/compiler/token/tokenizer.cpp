#include "tokenizer.hpp"
#include "token_hint.hpp"

Tokenizer::Tokenizer()
{

}

static void parseToken(ParsingToken& parsingToken, std::vector<Token>& tokens)
{
    std::optional<Token> newToken = parsingToken.intoResultingToken();
    if(newToken.has_value())
        tokens.push_back(newToken.value());
}

std::vector<Token> Tokenizer::tokenize(const std::string& string)
{
    std::vector<Token> tokens;

    ParsingToken parsingToken = ParsingToken();

    for (char c : string)
    {
        parsingToken.addCharacter(c);
        parseToken(parsingToken, tokens);
        parseToken(parsingToken, tokens);
    }
    
    parsingToken.addCharacter(' ');
    parseToken(parsingToken, tokens);

    return tokens;
}