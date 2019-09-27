sources := main.cpp lexer.cpp token.cpp

bin/cl: $(sources)
	g++ -Wall -Wextra -Wpedantic -Werror -g -o $@ $^
