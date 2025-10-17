#pragma once
#include "Entity.hpp"
#include "character/ZombieModel.hpp"
#include "render/Textures.hpp"
#include "level/Level.hpp"
#include <cmath>
#include <cstdlib>
#include <chrono>

class Zombie : public Entity {
private:
    static ZombieModel* zombieModel;
    Textures* textures;

public:
    float rot;
    float timeOffs;
    float speed;
    float rotA;

    Zombie(Level* level, Textures* textures, float x, float y, float z);
    ~Zombie() = default;
    
    void tick() override;
    void render(float partialTicks) override;
    
    static void initModel();
    static void destroyModel();
};
