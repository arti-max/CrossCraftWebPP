#pragma once
#include "render/Tessellator.hpp"
#include "render/Textures.hpp"
#include "player/Player.hpp"
#include "level/tile/Tile.hpp"
#include "level/Level.hpp"
#include <vector>

class Hud {
private:
    Textures* textures;
    int width;
    int height;

public:
    Hud(Textures* textures, int width, int height);
    void render(Player* player, Level* level, float partialTicks);
    
private:
    void drawTexturedModalRect(int x, int y, int u, int v, int width, int height);
};
