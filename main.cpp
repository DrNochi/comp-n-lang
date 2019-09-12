#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "character.h"
#include "main.h"
#include "token.h"

enum class LexerState {
    INVALID = -1,
    Empty,
    Identifier,
    Integer
};

// Maps FSM states to token types
const TokenType state_token_types[] = {
    TokenType::INVALID,
    TokenType::Identifier,
    TokenType::Integer
};

// State transition table for the lexer FSM
const LexerState state_transition_table[][(int)CharacterType::LENGTH] = {
//  Alpha                   Digit                   Dollar                  Whitespace          EndOfFile
    LexerState::Identifier, LexerState::Integer,    LexerState::INVALID,    LexerState::Empty,  LexerState::Empty,  // Empty
    LexerState::Identifier, LexerState::Identifier, LexerState::Identifier, LexerState::Empty,  LexerState::Empty,  // Identifier
    LexerState::Empty,      LexerState::Integer,    LexerState::Empty,      LexerState::Empty,  LexerState::Empty,  // Integer
};

int main(int argument_count, char* arguments[])
{
    // Display usage information when not enough arguments are supplied
    if (argument_count != 2) fail("Usage: cl FILE");

    // Open file input using user specified path
    std::fstream input(arguments[1]);
    if (!input) fail("Could not open file");

    std::vector<Token> tokens = std::vector<Token>(); // Resulting token stream
    std::stringstream lexeme; // Lexeme buffer of the next token

    LexerState state = LexerState::Empty; // FSM state
    char current_char = input.get(); // Character beeing processed by the FSM / lexer

    // Read input file
    while (!input.eof()) {
        // Get new state from transition table by using charater type
        CharacterType char_type = char_to_type(current_char);
        LexerState new_state = state_transition_table[(int)state][(int)char_type];

        // Check if the new state is valid or an error occured
        if (new_state == LexerState::INVALID) fail("Invalid or unexpected character");

        // Check if token has been accepted by FSM (returns to state 0)
        if (new_state == LexerState::Empty && state != LexerState::Empty) {
            // Add token to token stream
            tokens.push_back({ state_token_types[(int)state], lexeme.str() });
            std::cout << "Accept: " << lexeme.str() << std::endl;

            // Reset lexeme buffer
            lexeme.str("");

            // Continue without fetching new character as the current one has not been processed yet!
            state = new_state;
            continue;
        }

        // Append current char to lexeme (except in state 'Empty' as this state does not correspond to a token, e.g. whitespace)
        if (new_state != LexerState::Empty) lexeme << current_char;

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

    std::cout << std::endl;
    std::cout << "Tokens:" << std::endl;

    for (Token& token : tokens) {
        // Check if any of the identifier tokens is actually a keyword
        if (token.type == TokenType::Identifier && keywords.find(token.lexeme) != keywords.end())
            token.type = TokenType::Keyword;

        // Print tokens
        std::cout << token << ", ";
    }

    std::cout << std::endl;
}
