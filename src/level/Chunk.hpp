#pragma once
#include <vector>
#include <array>
#include <string>
#include "render/Tessellator.hpp"
#include "level/Level.hpp"
#include "phys/AABB.hpp"
#include "level/tile/Tile.hpp"
#include "player/Player.hpp"
#include <GL/gl.h>

class Chunk {
private:
    static Tessellator& t;
    Level* level;
    int x0 = 0, y0 = 0, z0 = 0;
    int x1 = 0, y1 = 0, z1 = 0;
    int lists = 0;
    bool dirty = true;
public:
    static int rebuiltThisFrame;
    static int updates;
    AABB boundingBox;
    float boundingSphereRadius = 0.0f;
    float x = 0.0f, y = 0.0f, z = 0.0f;
    bool visible = false;

    Chunk(Level* level, int x0, int y0, int z0, int x1, int y1, int z1);
    void rebuild(int layer);
    void rebuild();
    void render(int layer);
    void setDirty();
    bool isDirty();
    float distanceToSqr(Player* player);
    void reset();
};