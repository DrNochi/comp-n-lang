#pragma once

#include <fstream>
#include <string>
#include <deque>
#include <vector>

#include "token.h"

enum class LexerState {
    INVALID = -1,
    Empty,
    Identifier,
    Integer,
    Operator,
    Separator,
    EndOfFile
};

enum class CharacterType {
    Alpha,
    Digit,
    Dollar,
    Operator,
    Separator,
    Whitespace,
    EndOfFile,
    LENGTH
};

class Lexer {

public:
    Lexer(const std::string& filepath);

    Token next();
    std::vector<Token> read_all();

private:
    static const LexerState state_transition_table[][(int)CharacterType::LENGTH];
    static const TokenType state_token_types[];

    std::ifstream input;
    LexerState state;
    char current_char;

    std::deque<Token> lookahead_buffer;
    Token _next();

    CharacterType char_to_type(char character) const;

};
