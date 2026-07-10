#pragma once
#include "render/Tessellator.hpp"
#include "render/Textures.hpp"
#include "player/Player.hpp"
#include "level/tile/Tile.hpp"
#include "level/Level.hpp"
#include "util/Random.hpp"
#include "util/TinyFormat.hpp"
#include <vector>

class CrossCraft;

class Hud {
private:
    Textures* textures;
    CrossCraft* cc;
    int width = 0;
    int height = 0;
    Random* random = new Random();

public:
    int ticks = 0;

    Hud(CrossCraft* cc, Textures* textures, int width, int height);
    void render(Player* player, Level* level, float partialTicks);
    
private:
    void drawTexturedModalRect(int x, int y, int u, int v, int width, int height);
};
