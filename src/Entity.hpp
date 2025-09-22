#pragma once
#include "level/Level.hpp"
#include "phys/AABB.hpp"
#include <vector>
#include <cmath>
#include <random>

class Entity {
protected:
    Level* level;
    float heightOffset = 0.0f;
    float bbWidth = 0.6f;
    float bbHeight = 1.8f;
    
    static std::mt19937 randomGenerator;

public:
    float xo, yo, zo;
    float x, y, z;
    float xd, yd, zd;
    float yRot, xRot;

    AABB bb;
    
    bool onGround = false;
    bool horizontalCollision = false;
    bool removed = false;

    Entity(Level* level);
    virtual ~Entity() = default;

protected:
    void resetPos();
    void setSize(float w, float h);
    void setPos(float x, float y, float z);

public:
    void remove();
    void turn(float xo, float yo);
    virtual void tick();
    bool isFree(float xa, float ya, float za);
    void move(float xa, float ya, float za);
    bool isInWater();
    bool isInLava();
    void moveRelative(float xa, float za, float speed);
    bool isLit();
    virtual void render(float a);

private:
    float randomFloat();
};
