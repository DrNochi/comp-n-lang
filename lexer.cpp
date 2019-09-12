#include "lexer.h"

#include <fstream>
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

// Maps lexer states to token types (INVALID if not an accepting state)
const TokenType Lexer::state_token_types[] = {
    TokenType::INVALID,
    TokenType::Identifier,
    TokenType::Integer,
    TokenType::Operator,
    TokenType::Separator,
    TokenType::EndOfFile
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
    // Check if filepath is valid/could be opened
    if (!input) fail("Could not open file");

    // Remember that input stream has been dynamically allocated
    input_dynalloc = true;
}

Lexer::~Lexer()
{
    // Deallocate input stream if dynamically allocated
    if (input_dynalloc) delete &input;
}

// Read exactly one token form the input stream
Token Lexer::next()
{
    // Read next (partial) token
    Token token = _next();

    // "Token transformations" (recognize keywords, reals/floats): (partial) token -> complete token
    if (token.type == TokenType::Identifier && token.is_keyword()) { // Check if token is actually a keyword
        token.type = TokenType::Keyword;
    } else if (token.type == TokenType::Integer) { // Check if token is a real number
        // Get next two (partial) tokens as lookahead
        Token dot = _next();
        Token fraction = _next();

        // Check if interger is followed by a dot and another integer as decimal part
        if (dot.type == TokenType::Separator && dot.lexeme == "." && fraction.type == TokenType::Integer) {
            // Construct new token of type Real
            std::ostringstream lexeme;
            lexeme << token.lexeme << "." << fraction.lexeme;
            token = { TokenType::Real, lexeme.str() };
        } else {
            // Store tokens in lookahead buffer (not consumed)
            lookahead_buffer.push_back(dot);
            lookahead_buffer.push_back(fraction);
        }
    }

    std::cout << "Accept: " << token << std::endl;
    return token;
}

// Gets a (partial) token from the input stream
Token Lexer::_next()
{
    // Return next token from lookahead buffer if available
    if (!lookahead_buffer.empty()) {
        Token token = lookahead_buffer.front();
        lookahead_buffer.pop_front();
        return token;
    }

    // Lexeme buffer of the next token
    std::ostringstream lexeme;

    // Read input stream
    while (true) {
        // Get new state from transition table by using charater type
        CharacterType char_type = char_to_type(current_char);
        LexerState new_state = state_transition_table[(int)state][(int)char_type];

        // Check if the new state is valid or an error occured
        if (new_state == LexerState::INVALID) fail("Unexpected character");

        // Check if token has been accepted by FSM (returns to state Empty)
        if (new_state == LexerState::Empty && state != LexerState::Empty) {
            Token token = { state_token_types[(int)state], lexeme.str() };
            state = LexerState::Empty; // Reset state
            return token; // Return accepted token
        }

        // Append current char to lexeme (except in state Empty & EndOfFile as these do not need lexeme data, e.g. skip whitespace)
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
                    break; // Comment ended, line break found
            }
        }
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

    // Invalid or unrecognized input
    fail("Invalid character");
}
