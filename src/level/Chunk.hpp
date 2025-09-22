#pragma once
#include <vector>
#include <array>
#include <string>
#include "render/Tessellator.hpp"
#include "level/Level.hpp"
#include "phys/AABB.hpp"
#include "level/Tile.hpp"
#include "Player.hpp"
#include <GL/gl.h>

class Chunk {
private:
    static Tessellator& t;
    Level& level;
    int x0, y0, z0;
    int x1, y1, z1;
    float x, y, z;
    int lists;
    bool dirty = true;
public:
    static int rebuiltThisFrame;
    static int updates;
    AABB boundingBox;
    bool visible = false;

    Chunk(Level& level, int x0, int y0, int z0, int x1, int y1, int z1);
    void rebuild(int layer);
    void rebuild();
    void render(int layer);
    void setDirty();
    bool isDirty();
    float distanceToSqr(Player* player);
    void reset();
};