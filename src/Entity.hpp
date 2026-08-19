#pragma once
#include "phys/AABB.hpp"
#include "EntityType.hpp"
#include "model/Vec3D.hpp"
#include <vector>
#include <cmath>
#include <random>
#include <string>

class Textures;
class EntityMesh;
class Level;
class Player;

class Entity {
protected:
    Level* level = nullptr;
    float heightOffset = 0.0f;
    float bbWidth = 0.6f;
    float bbHeight = 1.8f;
    
    static std::mt19937 randomGenerator;

private:
    std::vector<AABB> aabbs;

public:
    float xo = 0.0f;
    float yo = 0.0f;
    float zo = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float xd = 0.0f;
    float yd = 0.0f;
    float zd = 0.0f;
    float yRot = 0.0f, xRot = 0.0f;
    float yRotO = 0.0f, xRotO = 0.0f;
    float xRotI = 0.0f, yRotI = 0.0f;
    float walkDist = 0.0f;
    float walkDistO = 0.0f;
    float pushthrough = 0.0f;
    float footSize = 0.0f;
    float ySlideOffset = 0.0f;
    float fallDistance = 0.0f;
    int nextStep = 1;
    float xOld = 0.0f;
    float yOld = 0.0f;
    float zOld = 0.0f;

    AABB bb;
    EntityMesh* emesh = nullptr;
    
    bool onGround = false;
    bool horizontalCollision = false;
    bool noPhysics = false;    
    bool slide = true;
    bool hovered = false;
    bool collision = false;
    bool removed = false;
    bool removeExternally = false;
    bool makeStepSound = true;
    bool debug = false;

    Entity(Level* level);
    virtual ~Entity() = default;

protected:
    void setSize(float w, float h);
    virtual bool isPlayer();
    virtual void causeFallDamage(float fall);

public:
    virtual void resetPos();
    virtual void setPos(float x, float y, float z);
    virtual void setRot(float yRot, float xRot);
    virtual void remove();
    virtual void turn(float xo, float yo);
    virtual void interpolateTurn(float xo, float yo);
    virtual void tick();
    virtual bool isFree(float xa, float ya, float za);
    virtual void move(float xa, float ya, float za);
    virtual bool isInWater();
    virtual bool isInLava();
    virtual void moveRelative(float xa, float za, float speed);
    virtual bool isLit();
    virtual float getBrightness(float partialTicks);
    virtual void render(float partialTicks, Textures* textures);
    virtual void playSound(const std::string& name, float volume, float pitch);
    virtual bool intersects(float x0, float y0, float z0, float x1, float y1, float z1);
    virtual void setLevel(Level* level);
    virtual void playerTouch(Player* player);
    virtual bool isPushable();
    virtual void push(Entity* e);
    virtual void push(float x, float y, float z);
    virtual void moveTo(float x, float y, float z, float yRot, float xRot);
    virtual float distanceTo(Entity* e);
    virtual float distanceTo(float x, float y, float z);
    virtual float distanceToSqr(Entity* e);
    virtual bool isPickable();
    virtual bool isShootable();
    virtual void awardKillScore(Entity* e, int score);
    virtual bool isUnderWater();
    virtual void hurt(Entity* e, int dmg);
    virtual EntityType getEntityType() const { return EntityType::Entity; }
    virtual bool shouldRender(Vec3D vec);
    virtual bool shouldRenderAtSqrDistance(float dist);

private:
    float randomFloat();
};
