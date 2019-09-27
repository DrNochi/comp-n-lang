#include "lexer.h"

#include <fstream>
#include <sstream>
#include <unordered_set>

#include "helpers.h"

// State transition table for the lexer FSM
const LexerState state_transition_table[(int)LexerState::LENGTH][(int)CharacterType::LENGTH] = {
//  Alpha                   Digit                   Dollar                  Dot                     Exclamation             Operator                Separator               Whitespace              EndOfFile
    LexerState::Identifier, LexerState::Integer,    LexerState::INVALID,    LexerState::Separator,  LexerState::Comment,    LexerState::Operator,   LexerState::Separator,  LexerState::Empty,      LexerState::EndOfFile,
    LexerState::Identifier, LexerState::Identifier, LexerState::Identifier, LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,
    LexerState::Empty,      LexerState::Integer,    LexerState::Empty,      LexerState::IntegerDot, LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,
    LexerState::INVALID,    LexerState::Real,       LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,
    LexerState::Empty,      LexerState::Real,       LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,
    LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,
    LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,
    LexerState::Comment,    LexerState::Comment,    LexerState::Comment,    LexerState::Comment,    LexerState::CommentEnd, LexerState::Comment,    LexerState::Comment,    LexerState::Comment,    LexerState::INVALID,
    LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,      LexerState::Empty,
    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::INVALID,    LexerState::Empty
};

// Maps lexer states to token types (NONE if not an accepting state)
const TokenType state_token_types[(int)LexerState::LENGTH] = {
    TokenType::NONE,
    TokenType::Identifier,
    TokenType::Integer,
    TokenType::NONE,
    TokenType::Real,
    TokenType::Operator,
    TokenType::Separator,
    TokenType::NONE,
    TokenType::NONE,
    TokenType::EndOfFile
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

Lexer::Lexer(std::istream& input) :
    input(input),
    state(LexerState::Empty),
    current_char(input.get())
{
    // Nothing
}

Lexer::Lexer(const std::string& filepath) :
    Lexer(*new std::ifstream(filepath))
{
    // Remember that input stream has been dynamically allocated
    input_dynalloc = true;

    // Check if filepath is valid/could be opened
    if (!input) fail("Could not open file");
}

Lexer::~Lexer()
{
    // Deallocate input stream if dynamically allocated
    if (input_dynalloc) delete &input;
}

// Reads exactly one token form the input stream
Token Lexer::next()
{
    // Lexeme buffer of the next token
    std::ostringstream lexeme;

    // Read input stream
    while (true) {
        // Get new state from transition table by using charater type
        CharacterType char_type = char_to_type(current_char);
        LexerState new_state = state_transition_table[(int)state][(int)char_type];

        // Check if the new state is valid or an error occured
        if (new_state == LexerState::INVALID) fail("Unexpected character");

        // Check if the end of a code unit has been reached
        if (new_state == LexerState::Empty && state != LexerState::Empty) {
            TokenType type = state_token_types[(int)state];

            // Reset state for next iteration
            state = LexerState::Empty;

            // Check if a token should be produced
            if (type != TokenType::NONE) {
                std::string lexeme_string = lexeme.str();

                // Check if identifier is actually a keyword
                if (type == TokenType::Identifier && keywords.find(lexeme_string) != keywords.end())
                    type = TokenType::Keyword;

                // Return accepted token
                return { type, lexeme_string };
            } else {
                // Reset lexeme and continue with next iteration
                lexeme.str("");
                continue;
            }
        }

        // Append current char to lexeme (except in state Empty & EndOfFile as these do not need lexeme data, e.g. skip whitespace)
        if (new_state != LexerState::Empty && new_state != LexerState::EndOfFile)
            lexeme << current_char;

        // Set new state of the FSM and fetch new character for next iteration
        state = new_state;
        current_char = input.get();
    }
}

// Gets all available tokens from the input stream
std::vector<Token> Lexer::read_all()
{
    std::vector<Token> tokens;

    // Read tokens until EndOfFile is found
    Token next_token;
    do tokens.push_back(next_token = next());
    while (next_token.type != TokenType::EndOfFile);

    return tokens;
}

// Converts input char to a CharacterType
CharacterType Lexer::char_to_type(char character) const
{
    if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'))
        return CharacterType::Alpha;
    if (character >= '0' && character <= '9')
        return CharacterType::Digit;

    switch (character) {
        case '$':
            return CharacterType::Dollar;
        case '.':
            return CharacterType::Dot;
        case '!':
            return CharacterType::Exclamation;

        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
        case '<':
        case '>':
            return CharacterType::Operator;

        case '\'':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
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

    // Invalid or unrecognized input
    fail("Invalid character");
}
