#pragma once

enum class CharacterType {
    Alpha,
    Digit,
    Dollar,
    Whitespace,
    EndOfFile,
    LENGTH
};

// Convert a character to its respective type
inline CharacterType char_to_type(char character)
{
    if (character >= 'A' && character <= 'z') return CharacterType::Alpha;
    if (character >= '0' && character <= '9') return CharacterType::Digit;

    switch (character) {
        case '$':
            return CharacterType::Dollar;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return CharacterType::Whitespace;
    }

    return CharacterType::EndOfFile;
}
