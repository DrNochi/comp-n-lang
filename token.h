#pragma once

#include <iostream>
#include <string>
#include <unordered_set>

enum class TokenType {
    INVALID,
    Identifier,
    Keyword,
    Integer
};

// Contains string representations of token types
const std::string token_type_strings[] = {
    "!!! INVALID !!!",
    "Identifier",
    "Keyword",
    "Integer"
};

// Map of all known keywords
const std::unordered_set<std::string> keywords = {
    "int", "float", "bool",
    "if", "else", "then", "endif",
    "while", "whileend",
    "do", "doend",
    "for", "forend",
    "input", "output",
    "and", "or",
    "function"
};


struct Token {
    TokenType type;
    std::string lexeme;
};

inline std::ostream& operator<<(std::ostream& output_stream, const Token& token)
{
    return output_stream
        << "[Type: " << token_type_strings[(int)token.type]
        << ", Lexeme: '" << token.lexeme
        << "']";
}
