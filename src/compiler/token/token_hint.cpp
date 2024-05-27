#include "token_hint.hpp"
#include <iostream>
ParsingToken::ParsingToken() : currentTokenValue(""), hint(TokenHint::None)
{

}

void ParsingToken::addCharacter(char character)
{
    currentTokenValue += character;

    if(hint == TokenHint::None)
    {
        if(std::isdigit(character))
            hint = TokenHint::Number;
        else if(std::isspace(character))
            hint = TokenHint::Space;
        else if(character == '\"')
            hint = TokenHint::StringLiteral;
        else if(character == '=' || character == ';' || character == '>' || character == '<' || 
                character == '+' || character == '-' || character == '*' || character == '/' || 
                character == '(' || character == ')' || character == '{' || character == '}' ||
                character == ',' || character == '!')
            hint = TokenHint::Sign;
        else if(std::isalpha(character))
            hint = TokenHint::Alphabetic;
    }
}

std::optional<Token> ParsingToken::intoResultingToken(Meta& metadata)
{
    if(currentTokenValue.empty())
        return std::nullopt;
        
    std::optional<Token> token = std::nullopt;

    char lastCharacter = currentTokenValue.back();

    switch (hint)
    {
        case TokenHint::Space:
        {
            // Spaces are cleared out
            if(!std::isspace(lastCharacter))
            {
                *this = ParsingToken();

                addCharacter(lastCharacter);
                return intoResultingToken(metadata);
            }
            break;
        }
        case TokenHint::Comment:
        {
            if(lastCharacter == '\n')
            {
                token = Token { .type = TokenType::Comment, .value = currentTokenValue.substr(0, currentTokenValue.length() - 1), .metadata = metadata };

                *this = ParsingToken();
            }
            break;
        }
        case TokenHint::Alphabetic:
        {
            if(!std::isalnum(lastCharacter) && lastCharacter != '!' && lastCharacter != '_')
            {
                auto stringWithoutLastCharacter = currentTokenValue.substr(0, currentTokenValue.length() - 1);
                auto keywordToken = tokenValueToKeywordToken.find(stringWithoutLastCharacter);
                // If it's a keyword
                if(keywordToken != tokenValueToKeywordToken.end())
                {
                    token = keywordToken->second;
                    token.value().metadata = metadata;
                }
                else
                    token = Token { .type = TokenType::Ident, .value = stringWithoutLastCharacter, .metadata = metadata };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
                
            break;
        }
        case TokenHint::Sign:
        {
            if(lastCharacter == ';')
            {
                token = Token { .type = TokenType::Semicolon, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == ',')
            {
                token = Token { .type = TokenType::Comma, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '>')
            {
                token = Token { .type = TokenType::GreaterThanSign, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '<')
            {
                token = Token { .type = TokenType::LessThanSign, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '+')
            {
                token = Token { .type = TokenType::PlusSign, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '-')
            {
                token = Token { .type = TokenType::MinusSign, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '*')
            {
                token = Token { .type = TokenType::MultiplicationSign, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(currentTokenValue.length() == 2 && currentTokenValue[0] == '/')
            {
                if(lastCharacter != '/')
                {
                    token = Token { .type = TokenType::DivisionSign, .metadata = metadata };

                    *this = ParsingToken();
                    addCharacter(lastCharacter);
                }
                else
                {
                    hint = TokenHint::Comment;
                }
            }
            else if(lastCharacter == '(')
            {
                token = Token { .type = TokenType::OpenRoundBracket, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == ')')
            {
                token = Token { .type = TokenType::CloseRoundBracket, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '{')
            {
                token = Token { .type = TokenType::OpenCurlyBracket, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(lastCharacter == '}')
            {
                token = Token { .type = TokenType::CloseCurlyBracket, .metadata = metadata };

                *this = ParsingToken();
            }
            else if(currentTokenValue.length() == 3 && currentTokenValue[0] == '!' && currentTokenValue[1] == '=' && lastCharacter != '=')
            {
                token = Token { .type = TokenType::NotEqualSign, .metadata = metadata };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
            else if(currentTokenValue.length() == 2 && currentTokenValue[0] == '=' && lastCharacter != '=')
            {
                token = Token { .type = TokenType::EqualSign, .metadata = metadata };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
            else if(currentTokenValue.length() == 3 && currentTokenValue[0] == '=' && currentTokenValue[1] == '=' && lastCharacter != '=')
            {
                token = Token { .type = TokenType::DoubleEqualSign, .metadata = metadata };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
                
            break;
        }
        case TokenHint::StringLiteral:
        {
            if(lastCharacter == '\"' && currentTokenValue.length() > 1)
            {
                token = Token { .type = TokenType::LiteralString, .value = currentTokenValue.substr(1, currentTokenValue.length() - 2), .metadata = metadata };

                *this = ParsingToken();
            }

            break;
        }
        case TokenHint::Number:
        {
            if(std::isspace(lastCharacter) || lastCharacter == ';' || lastCharacter == ')' || lastCharacter == ',')
            {
                token = Token { .type = TokenType::LiteralNumber, .value = currentTokenValue.substr(0, currentTokenValue.length() - 1), .metadata = metadata };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
            
            break;
        }
        
        default:
            break;
    }

    return token;
}