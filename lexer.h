#pragma once

#include <istream>
#include <string>
#include <vector>

#include "token.h"

enum class LexerState {
    Empty,
    Identifier,
    Integer,
    IntegerDot,
    Real,
    Operator,
    Separator,
    Comment,
    CommentEnd,
    EndOfFile,

    LENGTH,
    INVALID
};

enum class CharacterType {
    Alpha,
    Digit,
    Dollar,
    Dot,
    Exclamation,
    Operator,
    Separator,
    Whitespace,
    EndOfFile,

    LENGTH
};

class Lexer {

public:
    Lexer(std::istream& input);
    Lexer(const std::string& filepath);

    ~Lexer();

    Token next();
    std::vector<Token> read_all();

private:
    std::istream& input;
    bool input_dynalloc = false;

    LexerState state;
    char current_char;

    CharacterType char_to_type(char character) const;

};
