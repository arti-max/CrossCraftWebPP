#pragma once
#include <array>
#include "render/Tessellator.hpp"
#include "level/Level.hpp"
#include "util/Random.hpp"
#include "phys/AABB.hpp"
#include "player/Player.hpp"
#include "particle/ParticleEngine.hpp"
#include "sound/SoundType.hpp"

class Tile {

private:

protected:
    float minX, minY, minZ, maxX, maxY, maxZ;
    virtual void setShape(float x0, float y0, float z0, float x1, float y1, float z1);
    virtual bool shouldRenderFace(Level* level, int x, int y, int z, int layer, int face);
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
    static const Tile* gravel;
    static const Tile* sand;
    static const Tile* log;
    static const Tile* leaves;
    static const Tile* goldOre;
    static const Tile* ironOre;
    static const Tile* coalOre;
    static const Tile* sponge;
    static const Tile* glass;
    static const Tile* wool1;
    static const Tile* wool2;
    static const Tile* wool3;
    static const Tile* wool4;
    static const Tile* wool5;
    static const Tile* wool6;
    static const Tile* wool7;
    static const Tile* wool8;
    static const Tile* wool9;
    static const Tile* wool10;
    static const Tile* wool11;
    static const Tile* wool12;
    static const Tile* wool13;
    static const Tile* wool14;
    static const Tile* wool15;
    static const Tile* wool16;
    static const Tile* redFlower;
    static const Tile* yellowFlower;
    static const Tile* redMushroom;
    static const Tile* brownMushroom;
    static const Tile* goldBlock;

    int textureId;
    int id;
    float particleGravity;
    const SoundType* st;

    virtual Tile* setData(const SoundType& st, float particleGravity);
    virtual int getTexture(int face);
    virtual bool render(Tessellator& t, Level* level, int layer, int x, int y, int z);
    virtual void renderFace(Tessellator& t, int x, int y, int z, int face);
    virtual void renderBackFace(Tessellator& t, int x, int y, int z, int face);
    virtual void renderFaceNoTexture(Player* player, Tessellator& t, int x, int y, int z, int face);
    virtual void onDestroy(Level* level, int x, int y, int z, ParticleEngine* engine, bool drop);
    virtual void tick(Level* level, int x, int y, int z, Random* random);
    virtual bool mayPick();
    virtual bool blocksLight();
    virtual bool isSolid();
    virtual AABB* getTileAABB(int x, int y, int z) const;
    virtual AABB* getAABB(int x, int y, int z) const ;
    virtual void neighborChanged(Level* level, int x, int y, int z, int type);
    virtual int getLiquidType();
    virtual bool isCalmLiquid();
    virtual void onBlockAdded(Level* level, int x, int y, int z);
    virtual float getBrightness(Level* level, int x, int y, int z);
};
