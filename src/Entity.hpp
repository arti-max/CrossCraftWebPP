#pragma once
#include "phys/AABB.hpp"
#include <vector>
#include <cmath>
#include <random>
#include <string>

class Textures;
class EntityMesh;
class Level;

class Entity {
protected:
    Level* level = nullptr;
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
    EntityMesh* emesh = nullptr;
    
    bool onGround = false;
    bool horizontalCollision = false;
    bool removed = false;
    bool makeStepSound = true;

    Entity(Level* level);
    virtual ~Entity() = default;

protected:
    void setSize(float w, float h);
    virtual bool isPlayer();

public:
    virtual void resetPos();
    virtual void setPos(float x, float y, float z);
    virtual void setRot(float yRot, float xRot);
    virtual void remove();
    virtual void turn(float xo, float yo);
    void interpolateTurn(float xo, float yo);
    virtual void tick();
    bool isFree(float xa, float ya, float za);
    virtual void move(float xa, float ya, float za);
    virtual bool isInWater();
    virtual bool isInLava();
    void moveRelative(float xa, float za, float speed);
    bool isLit();
    float getBrightness();
    virtual void render(float a, Textures* textures);
    virtual void playSound(const std::string& name, float volume, float pitch);
    virtual bool intersects(float x0, float y0, float z0, float x1, float y1, float z1);
    void setLevel(Level* level);
    virtual void playerTouch(Entity* player);


private:
    float randomFloat();
};
