#include "token.h"

// Map of all known keywords
const std::unordered_set<std::string> Token::keywords = {
    "int", "float", "bool",
    "if", "else", "then", "endif",
    "while", "whileend",
    "do", "doend",
    "for", "forend",
    "input", "output",
    "and", "or",
    "function"
};

// Contains string representations of token types
const std::string Token::type_strings[] = {
    "!!! INVALID !!!",
    "Identifier",
    "Keyword",
    "Integer",
    "EndOfFile"
};

bool Token::is_keyword() const
{
    return type == TokenType::Identifier && keywords.find(lexeme) != keywords.end();
}

std::ostream& operator<<(std::ostream& output_stream, const Token& token)
{
    return output_stream
        << "[Type: " << Token::type_strings[(int)token.type]
        << ", Lexeme: '" << token.lexeme
        << "']";
}
