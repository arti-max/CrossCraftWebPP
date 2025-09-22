
#include "CrossCraft.hpp"
#include "gl4esinit.h"

extern "C" void initialize_gl4es();


int main() {
    initialize_gl4es();

    CrossCraft game = CrossCraft("#canvas", 800, 600, false);

    game.run();
}