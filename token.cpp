#include "token.h"

// Contains string representations of token types
const std::string type_strings[(int)TokenType::LENGTH] = {
    "Identifier",
    "Keyword",
    "Integer",
    "Real",
    "Operator",
    "Separator",
    "EndOfFile"
};

std::ostream& operator<<(std::ostream& output_stream, const Token& token)
{
    return output_stream
        << "[Type: " << type_strings[(int)token.type]
        << ", Lexeme: '" << token.lexeme
        << "']";
}
