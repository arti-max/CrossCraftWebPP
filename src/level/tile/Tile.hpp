#pragma once
#include <array>
#include "render/Tessellator.hpp"
#include "level/Level.hpp"
#include "util/Random.hpp"
#include "phys/AABB.hpp"
#include "Player.hpp"
#include "particle/ParticleEngine.hpp"

class Tile {

private:

protected:
    float minX, minY, minZ, maxX, maxY, maxZ;
    virtual void setShape(float x0, float y0, float z0, float x1, float y1, float z1);
    virtual bool shouldRenderFace(Level* level, int x, int y, int z, int layer, int face);
    virtual int getTexture(int face);
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

    virtual void render(Tessellator& t, Level* level, int layer, int x, int y, int z);
    virtual void renderFace(Tessellator& t, int x, int y, int z, int face);
    virtual void renderBackFace(Tessellator& t, int x, int y, int z, int face);
    virtual void renderFaceNoTexture(Player* player, Tessellator& t, int x, int y, int z, int face);
    virtual void onDestroy(Level* level, int x, int y, int z, ParticleEngine* engine);
    virtual void tick(Level* level, int x, int y, int z, Random* random);
    virtual bool mayPick();
    virtual bool blocksLight();
    virtual bool isSolid();
    virtual AABB* getTileAABB(int x, int y, int z) const;
    virtual AABB* getAABB(int x, int y, int z) const ;
    virtual void neighborChanged(Level* level, int x, int y, int z, int type);
    virtual int getLiquidType();
    virtual bool isCalmLiquid();
};