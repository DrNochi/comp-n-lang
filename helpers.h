#pragma once

#include <stdlib.h>
#include <iostream>

[[noreturn]]
inline void fail(const std::string message) {
    // Print an error message and exit
    std::cerr << message << std::endl;
    exit(-1);
}
