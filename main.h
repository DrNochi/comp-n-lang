#pragma once

#include <iostream>

// Print an error message and exit
inline void fail(const std::string message) {
    std::cerr << message << std::endl;
    exit(-1);
}
