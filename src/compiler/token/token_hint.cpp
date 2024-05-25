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
                character == ',')
            hint = TokenHint::Sign;
        else if(std::isalpha(character))
            hint = TokenHint::Alphabetic;
    }
}

std::optional<Token> ParsingToken::intoResultingToken()
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
                return intoResultingToken();
            }
            break;
        }
        case TokenHint::Comment:
        {
            if(lastCharacter == '\n')
            {
                token = Token { .type = TokenType::Comment, .value = currentTokenValue.substr(0, currentTokenValue.length() - 1) };

                *this = ParsingToken();
            }
            break;
        }
        case TokenHint::Alphabetic:
        {
            auto keywordToken = tokenValueToKeywordToken.find(currentTokenValue);
            // If it's a keyword
            if(keywordToken != tokenValueToKeywordToken.end())
            {
                token = keywordToken->second;

                *this = ParsingToken();
            }
            else
            {
                if(!std::isalnum(lastCharacter))
                {
                    token = Token { .type = TokenType::Ident, .value = currentTokenValue.substr(0, currentTokenValue.length() - 1) };

                    *this = ParsingToken();
                    addCharacter(lastCharacter);
                }
            }
                
            break;
        }
        case TokenHint::Sign:
        {
            if(lastCharacter == ';')
            {
                token = Token { .type = TokenType::Semicolon };

                *this = ParsingToken();
            }
            else if(lastCharacter == ',')
            {
                token = Token { .type = TokenType::Comma };

                *this = ParsingToken();
            }
            else if(lastCharacter == '>')
            {
                token = Token { .type = TokenType::GreaterThanSign };

                *this = ParsingToken();
            }
            else if(lastCharacter == '<')
            {
                token = Token { .type = TokenType::LessThanSign };

                *this = ParsingToken();
            }
            else if(lastCharacter == '+')
            {
                token = Token { .type = TokenType::PlusSign };

                *this = ParsingToken();
            }
            else if(lastCharacter == '-')
            {
                token = Token { .type = TokenType::MinusSign };

                *this = ParsingToken();
            }
            else if(lastCharacter == '*')
            {
                token = Token { .type = TokenType::MultiplicationSign };

                *this = ParsingToken();
            }
            else if(currentTokenValue.length() == 2 && currentTokenValue[0] == '/')
            {
                if(lastCharacter != '/')
                {
                    token = Token { .type = TokenType::DivisionSign };

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
                token = Token { .type = TokenType::OpenRoundBracket };

                *this = ParsingToken();
            }
            else if(lastCharacter == ')')
            {
                token = Token { .type = TokenType::CloseRoundBracket };

                *this = ParsingToken();
            }
            else if(lastCharacter == '{')
            {
                token = Token { .type = TokenType::OpenCurlyBracket };

                *this = ParsingToken();
            }
            else if(lastCharacter == '}')
            {
                token = Token { .type = TokenType::CloseCurlyBracket };

                *this = ParsingToken();
            }
            else if(currentTokenValue.length() == 2 && currentTokenValue[0] == '=' && lastCharacter != '=')
            {
                token = Token { .type = TokenType::EqualSign };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
            else if(currentTokenValue.length() == 3 && currentTokenValue[0] == '=' && currentTokenValue[1] == '=' && lastCharacter != '=')
            {
                token = Token { .type = TokenType::DoubleEqualSign };

                *this = ParsingToken();
                addCharacter(lastCharacter);
            }
                
            break;
        }
        case TokenHint::StringLiteral:
        {
            if(lastCharacter == '\"' && currentTokenValue.length() > 1)
            {
                token = Token { .type = TokenType::LiteralString, .value = currentTokenValue.substr(1, currentTokenValue.length() - 2) };

                *this = ParsingToken();
            }

            break;
        }
        case TokenHint::Number:
        {
            if(std::isspace(lastCharacter) || lastCharacter == ';' || lastCharacter == ')' || lastCharacter == ',')
            {
                token = Token { .type = TokenType::LiteralNumber, .value = currentTokenValue.substr(0, currentTokenValue.length() - 1) };

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