#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "helpers.h"
#include "lexer.h"
#include "token.h"

int main(int argument_count, char* arguments[])
{
    // Display usage information when not enough arguments are supplied
    if (argument_count != 2) fail("Usage: cl FILE");

    Lexer lexer(arguments[1]);
    std::vector<Token> tokens = lexer.read_all();

    std::cout << std::endl;

    std::cout << "Tokens:" << std::endl;
    for (Token& token : tokens) std::cout << token << ", ";
    std::cout << std::endl;
}
