#pragma once
#include <string>
#include <vector>
#include "Entity.hpp"

class Font;
class CrossCraft;

class Sign : public Entity {
private:
    float xd = 0.0f;
    float yd = 0.0f;
    float zd = 0.0f;
    float rot = 0.0f;
    Font* font;
    std::vector<std::string> messages;
public:
    Sign(CrossCraft* cc, float x, float y, float z, float rot);
    bool isPickable() override;
    void tick() override;
    void render(float partialTicks, Textures* textures) override;
};