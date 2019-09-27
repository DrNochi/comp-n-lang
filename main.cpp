#include <iostream>
#include <vector>

#include "helpers.h"
#include "lexer.h"
#include "token.h"

int main(int argument_count, char* arguments[])
{
    // Display usage information when too many arguments are supplied
    if (argument_count > 2) fail("Usage: cl [FILE]");

    // Construct lexer (either using the user supplied filename or standard input)
    Lexer lexer = argument_count == 2 ? Lexer(arguments[1]) : Lexer(std::cin);

    // Lex input
    std::vector<Token> tokens = lexer.read_all();

    std::cout << std::endl;

    // Print out the list of tokens
    std::cout << "Tokens:" << std::endl;
    std::cout << "-------" << std::endl << std::endl;
    for (Token& token : tokens) std::cout << token << ",\n";
    std::cout << std::endl;
}
