#pragma once

#include "token.hpp"
#include <vector>
#include <string>

/**
 * @class Tokenizer
 * @brief The Tokenizer class is responsible for converting a string of source code into a sequence of tokens.
 *
 * The Tokenizer takes a source code string as input and produces a vector of Token objects, where each
 * token represents a meaningful unit in the source code, such as keywords, identifiers, literals, etc.
 */
class Tokenizer
{
public:
    /**
     * @brief Default constructor for the Tokenizer class.
     *
     * Initializes a Tokenizer object.
     */
    Tokenizer();

    /**
     * @brief Tokenizes the given source code string.
     *
     * This method processes the input string and produces a vector of Token objects representing
     * the lexical units found in the source code.
     *
     * @param string The source code string to be tokenized.
     * @return A vector of Token objects representing the tokens found in the source code.
     */
    std::vector<Token> tokenize(const std::string& string);
};