#pragma once
#include "Entity.hpp"
#include "player/Player.hpp"

class Level;

class Arrow : public Entity {
private:
    float xd = 0.0f;
    float yd = 0.0f;
    float zd = 0.0f;
    float xRot = 0.0f;
    float yRot = 0.0f;
    float xRotO = 0.0f;
    float yRotO = 0.0f;
    
    int stickTime = 0;
    Entity* owner = nullptr;
    int time = 0;
    int dmg = 0;
public:
    float gravity = 0.0f;
    bool hasHit = false;
    int type = 0;
    Arrow(Level* level, Entity* owner, float x, float y, float z, float yaw, float pitch, float gravity);
    void tick() override;
    void render(float PartialTicks, Textures* textures) override;
    void awardKillScore(Entity* e, int score) override;
    void playerTouch(Player* player) override;
};