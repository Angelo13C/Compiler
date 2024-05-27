#pragma once

#include "token.hpp"
#include <optional>
#include <string>
#include <unordered_map>

/**
 * @enum TokenHint
 * @brief An enumeration representing hints for expected token types during parsing.
 */
enum class TokenHint
{
    None,

    Space,
    Comment,
    Alphabetic,
    Sign,
    Number,
    StringLiteral
};

/**
 * @struct ParsingToken
 * @brief A structure representing a token being parsed during lexical analysis.
 *
 * The ParsingToken structure is used during the lexical analysis process to accumulate characters
 * and provide hints about the expected token type. It is capable of transforming into a final Token.
 */
struct ParsingToken
{
    /**
     * @brief Default constructor for ParsingToken.
     *
     * Initializes a ParsingToken with default values.
     */
    ParsingToken();

    /**
     * @brief Adds a character to the current token being parsed.
     * @param character The character to be added to the current token.
     */
    void addCharacter(char character);

    /**
     * @brief Converts the accumulated characters into a resulting Token based on the hint.
     * @return An optional Token, or std::nullopt if no valid token can be formed.
     */
    std::optional<Token> intoResultingToken(Meta& metadata);
public:
    TokenHint hint;
    std::string currentTokenValue;
};

/**
 * @brief A map associating string values with corresponding keyword Tokens.
 */
const std::unordered_map<std::string, Token> tokenValueToKeywordToken = {
    { "return", Token { .type = TokenType::KeywordReturn }},
    { "int", Token { .type = TokenType::KeywordInt }},
    { "string", Token { .type = TokenType::KeywordString }},
    { "if", Token { .type = TokenType::KeywordIf }},
    { "else", Token { .type = TokenType::KeywordElse }},
    { "while", Token { .type = TokenType::KeywordWhile }},
    { "fn", Token { .type = TokenType::KeywordFn }},
    { "asm!", Token { .type = TokenType::KeywordAsm, .value = "asm!" }},
    { "include!", Token { .type = TokenType::KeywordAsm, .value = "include!" }},
};