#pragma once

#include <ostream>
#include <string>

enum class TokenType {
    Identifier,
    Keyword,
    Integer,
    Real,
    Operator,
    Separator,
    EndOfFile,

    LENGTH,
    NONE
};

struct Token {

    TokenType type;
    std::string lexeme;

private:
    friend std::ostream& operator<<(std::ostream& output_stream, const Token& token);

};

std::ostream& operator<<(std::ostream& output_stream, const Token& token);
