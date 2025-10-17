#pragma once
#include "character/Cube.hpp"
#include <cmath>

class ZombieModel {
public:
    Cube* head;
    Cube* body;
    Cube* arm0;
    Cube* arm1;
    Cube* leg0;
    Cube* leg1;

    ZombieModel();
    ~ZombieModel();
    
    void render(float time);
};
