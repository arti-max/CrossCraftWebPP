#pragma once
#include <array>
#include "render/Tessellator.hpp"
#include "level/Level.hpp"
#include "util/Random.hpp"
#include "phys/AABB.hpp"
#include "Player.hpp"

class Tile {

private:

protected:
    float minX, minY, minZ, maxX, maxY, maxZ;
    void setShape(float x0, float y0, float z0, float x1, float y1, float z1);
    bool shouldRenderFace(Level* level, int x, int y, int z, int layer, int face);
    int getTexture(int face);
public:
    Tile(int id);
    Tile (int id, int textureId);
    ~Tile();
    static std::array<Tile*, 256> tiles;
    static const Tile* empty;
    static const Tile* rock;
    static const Tile* grass;
    static const Tile* dirt;
    static const Tile* cobblestone;
    static const Tile* wood;
    static const Tile* bush;
    static const Tile* water;
    static const Tile* unbreakable;
    static const Tile* calmWater;
    static const Tile* lava;
    static const Tile* calmLava;

    int textureId;
    int id;

    void render(Tessellator& t, Level* level, int layer, int x, int y, int z);
    void renderFace(Tessellator& t, int x, int y, int z, int face);
    void renderBackFace(Tessellator& t, int x, int y, int z, int face);
    void renderFaceNoTexture(Player* player);
    void onDestroy(Level* level, int x, int y, int z);   // TODO: PatricleEngine
    void tick(Level* level, int x, int y, int z, Random random);
    bool mayPick();
    bool blocksLight();
    bool isSolid();
    virtual AABB* getTileAABB(int x, int y, int z) const;
    virtual AABB* getAABB(int x, int y, int z) const ;
    void neighborChanged(Level* level, int x, int y, int z, int type);
    int getLiquidType();
    bool isCalmLiquid();
};