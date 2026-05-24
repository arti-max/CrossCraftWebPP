#pragma once
#include "Entity.hpp"
#include "player/Player.hpp"

class CrossCraft;

class Arrow : public Entity {
private:
    float xd = 0.0f;
    float yd = 0.0f;
    float zd = 0.0f;
    float xRot = 0.0f;
    float yRot = 0.0f;
    float xRotO = 0.0f;
    float yRotO = 0.0f;
    bool hasHit = false;
    int stickTime = 0;
    Player* owner = nullptr;
    int time = 0;
public:
    Arrow(CrossCraft* cc, Player* player, float x, float y, float z, float yaw, float pitch);
    void tick() override;
    void render(float PartialTicks, Textures* textures) override;
    void awardKillScore(Entity* e, int score) override;
};