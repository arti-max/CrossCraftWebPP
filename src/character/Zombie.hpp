#pragma once
#include "Entity.hpp"
#include "character/ZombieModel.hpp"
#include "render/Textures.hpp"
#include "level/Level.hpp"
#include <cmath>
#include <cstdlib>
#include <chrono>

class HZombie : public Entity {
private:
    static HZombieModel* zombieModel;
    Textures* textures;

public:
    float rot;
    float timeOffs;
    float speed;
    float rotA;

    HZombie(Level* level, Textures* textures, float x, float y, float z);
    ~HZombie() = default;
    
    void tick() override;
    void render(float partialTicks, Textures* textures) override;
    
    static void initModel();
    static void destroyModel();
};
