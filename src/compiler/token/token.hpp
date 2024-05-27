#pragma once

#include <optional>
#include <string>

#include "meta.hpp"

/**
 * @enum TokenType
 * @brief An enumeration representing different types of tokens.
 */
enum class TokenType
{
    Unknown, ///< Unknown token type.

    KeywordReturn, ///< Keyword "return".
    KeywordInt,
    KeywordString,
    KeywordIf,
    KeywordElse,
    KeywordWhile,
    KeywordFn,
    KeywordAsm,
    KeywordInclude,

    Comment,

    Ident,

    LiteralNumber, ///< Numeric literal.
    LiteralString, ///< Alphabetic literal.

    Semicolon, ///< Semicolon token.
    
    Comma,

    EqualSign,
    DoubleEqualSign,
    NotEqualSign,
    GreaterThanSign,
    LessThanSign,
    
    PlusSign,
    MinusSign,
    MultiplicationSign,
    DivisionSign,

    OpenRoundBracket,
    CloseRoundBracket,
    OpenCurlyBracket,
    CloseCurlyBracket,
};

/**
 * @struct Token
 * @brief A structure representing a token in the lexical analysis process.
 *
 * The Token structure holds information about the type and optional value of a token.
 * It provides a formatting function to represent the token as a string for debugging purposes.
 */
struct Token
{
    TokenType type;
    std::optional<std::string> value;

    Meta metadata;

    /**
     * @brief Formats the token as a string for debugging purposes.
     * @return A formatted string representation of the token.
     */
    const std::string format() const
    {
        switch (type)
        {
        case TokenType::KeywordReturn:
            return "KeywordReturn";
        case TokenType::KeywordInt:
            return "KeywordInt";
        case TokenType::KeywordString:
            return "KeywordString";
        case TokenType::KeywordIf:
            return "KeywordIf";
        case TokenType::KeywordElse:
            return "KeywordElse";
        case TokenType::KeywordFn:
            return "KeywordFn";
        case TokenType::KeywordAsm:
            return "asm!";
        case TokenType::KeywordInclude:
            return "include!";

        case TokenType::Ident:
            return value.value();
            
        case TokenType::Comment:
            return value.value();

        case TokenType::LiteralNumber:
            return value.value();
        case TokenType::LiteralString:
            return value.value();

        case TokenType::Semicolon:
            return ";";
        
        case TokenType::Comma:
            return ",";

        case TokenType::EqualSign:
            return "=";
        case TokenType::DoubleEqualSign:
            return "==";
        case TokenType::NotEqualSign:
            return "!=";
            
        case TokenType::GreaterThanSign:
            return ">";
        case TokenType::LessThanSign:
            return "<";
            
        case TokenType::PlusSign:
            return "+";
        case TokenType::MinusSign:
            return "-";
        case TokenType::MultiplicationSign:
            return "*";
        case TokenType::DivisionSign:
            return "/";

        case TokenType::OpenRoundBracket:
            return "(";
        case TokenType::CloseRoundBracket:
            return ")";
        case TokenType::OpenCurlyBracket:
            return "{";
        case TokenType::CloseCurlyBracket:
            return "}";

        default:
            return "[Unknown]";
        }
    }
};