sources := main.cpp lexer.cpp token.cpp

bin/cl: $(sources)
	g++ -Wpedantic -Werror -g -o $@ $^
