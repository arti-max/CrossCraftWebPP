#pragma once
#include "level/Level.hpp"
#include "phys/AABB.hpp"
#include <vector>
#include <cmath>
#include <random>
#include <string>

class Textures;

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
    float yRotO, xRotO;
    float xRotI, yRotI;
    float walkDist = 0.0f;

    AABB bb;
    
    bool onGround = false;
    bool horizontalCollision = false;
    bool removed = false;
    bool makeStepSound = true;

    Entity(Level* level);
    virtual ~Entity() = default;

protected:
    void setSize(float w, float h);

public:
    void resetPos();
    void setPos(float x, float y, float z);
    void setRot(float yRot, float xRot);
    void remove();
    void turn(float xo, float yo);
    void interpolateTurn(float xo, float yo);
    virtual void tick();
    bool isFree(float xa, float ya, float za);
    void move(float xa, float ya, float za);
    bool isInWater();
    bool isInLava();
    void moveRelative(float xa, float za, float speed);
    bool isLit();
    float getBrightness();
    virtual void render(float a, Textures* textures);
    virtual void playSound(const std::string& name, float volume, float pitch);

private:
    float randomFloat();
};
