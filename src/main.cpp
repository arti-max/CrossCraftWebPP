#include "CrossCraftApplet.hpp"
#include "gl4esinit.h"
#include <iostream>

extern "C" void initialize_gl4es();

int main() {
    std::cout << "CrossCraft C++ main() called" << std::endl;
    
    try {
        std::cout << "Initializing gl4es..." << std::endl;
        initialize_gl4es();
        std::cout << "gl4es initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "ERROR initializing gl4es: " << e.what() << std::endl;
        return -1;
    }
    
    std::cout << "Waiting for JavaScript to call startApplet..." << std::endl;
    std::cout << "main() completed successfully" << std::endl;
    
    return 0;
}
