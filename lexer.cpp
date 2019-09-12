#include "lexer.h"

#include <sstream>

#include "helpers.h"

// State transition table for the lexer FSM
const LexerState Lexer::state_transition_table[][(int)CharacterType::LENGTH] = {
//  Alpha                   Digit                   Dollar                  Operator                Separator               Whitespace              EndOfFile
    LexerState::Identifier, LexerState::Integer,    LexerState::INVALID,    LexerState::Operator,   LexerState::Separator,  LexerState::Empty,      LexerState::EndOfFile,  // Empty
    LexerState::Identifier, LexerState::Identifier, LexerState::Identifier, LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      // Identifier
    LexerState::Empty,      LexerState::Integer,    LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      // Integer
    LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      // Operator
    LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      // Separator
    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::Empty,      // EndOfFile
};

const TokenType Lexer::state_token_types[] = {
    TokenType::INVALID,
    TokenType::Identifier,
    TokenType::Integer,
    TokenType::Operator,
    TokenType::Separator,
    TokenType::EndOfFile
};

Lexer::Lexer(const std::string& filepath) :
    input(filepath),
    state(LexerState::Empty)
{
    if (!input) fail("Could not open file");
    current_char = input.get();
}

Token Lexer::next()
{
    Token token = _next();

    // Check if any of the identifier tokens is actually a keyword
    if (token.type == TokenType::Identifier && token.is_keyword())
        token.type = TokenType::Keyword;

    if (token.type == TokenType::Integer) {
        Token dot = _next();
        Token fraction = _next();
        if (dot.type == TokenType::Separator && dot.lexeme == "." && fraction.type == TokenType::Integer) {
            std::ostringstream lexeme;
            lexeme << token.lexeme << "." << fraction.lexeme;
            token = { TokenType::Real, lexeme.str() };
        } else {
            lookahead_buffer.push_back(dot);
            lookahead_buffer.push_back(fraction);
        }
    }

    std::cout << "Accept: " << token << std::endl;
    return token;
}

Token Lexer::_next()
{
    if (!lookahead_buffer.empty()) {
        Token token = lookahead_buffer.front();
        lookahead_buffer.pop_front();
        return token;
    }

    std::ostringstream lexeme; // Lexeme buffer of the next token

    // Read input file
    while (true) {
        // Get new state from transition table by using charater type
        CharacterType char_type = char_to_type(current_char);
        LexerState new_state = state_transition_table[(int)state][(int)char_type];

        // Check if the new state is valid or an error occured
        if (new_state == LexerState::INVALID) fail("Unexpected character");

        // Check if token has been accepted by FSM (returns to state 0)
        if (new_state == LexerState::Empty && state != LexerState::Empty) {
            Token token = { state_token_types[(int)state], lexeme.str() };
            state = LexerState::Empty;
            return token;
        }

        // Append current char to lexeme (except in state 'Empty' as this state does not correspond to a token, e.g. whitespace)
        if (new_state != LexerState::Empty && new_state != LexerState::EndOfFile)
            lexeme << current_char;

        // Set new state of the FSM and fetch new character for next iteration
        state = new_state;
        current_char = input.get();

        // Skip line comment introduced by ! (exclamation mark)
        if (current_char == '!') {
            while (!input.eof()) {
                current_char = input.get();
                if (current_char == '\r' || current_char == '\n')
                    break;
            }
        }
    }
}

std::vector<Token> Lexer::read_all()
{
    std::vector<Token> tokens; // Resulting token stream

    Token next_token;
    do tokens.push_back(next_token = next());
    while (next_token.type != TokenType::EndOfFile);

    return tokens;
}

CharacterType Lexer::char_to_type(char character) const
{
    if (character >= 'A' && character <= 'z') return CharacterType::Alpha;
    if (character >= '0' && character <= '9') return CharacterType::Digit;

    switch (character) {
        case '$':
            return CharacterType::Dollar;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
        case '<':
        case '>':
            return CharacterType::Operator;
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case '\'':
        case ',':
        case '.':
        case ':':
        case ';':
            return CharacterType::Separator;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return CharacterType::Whitespace;
        case std::ifstream::traits_type::eof():
            return CharacterType::EndOfFile;
    }

    fail("Invalid character");
}
