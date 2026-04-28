#pragma once

#include "mob/ai/AI.hpp"
#include "util/Random.hpp"
#include <vector>

class Entity;
class Level;
class Mob;


class BasicAI : public AI {
public:
    Random* random = new Random();
    float xxa; // Ax
    float yya; // Ay
    float yRotA;
    Level* level;
    Mob* mob;
    bool jumping = false;
    int attackDelay = 0;
    float runSpeed = 0.7f;
    int noActionTime = 0;
    Entity* attackTarget = nullptr;

    void tick(Level* level, Mob* mob) override;
    void jumpFromGround();
    void update();
    void beforeRemove() override;
    void hurt(Entity* e, int dmg) override;
};