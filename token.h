#pragma once

#include <ostream>
#include <string>
#include <unordered_set>

enum class TokenType {
    INVALID,
    Identifier,
    Keyword,
    Integer,
    Real,
    Operator,
    Separator,
    EndOfFile
};

struct Token {

    TokenType type;
    std::string lexeme;

    bool is_keyword() const;

private:
    static const std::unordered_set<std::string> keywords;
    static const std::string type_strings[];

    friend std::ostream& operator<<(std::ostream& output_stream, const Token& token);

};

std::ostream& operator<<(std::ostream& output_stream, const Token& token);
