#pragma once

#include <stdlib.h>
#include <iostream>

// Print an error message and exit
[[noreturn]]
inline void fail(const std::string message) {
    std::cerr << message << std::endl;
    exit(-1);
}
