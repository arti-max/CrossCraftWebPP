#pragma once
#include "level/tile/Tile.hpp"

class LiquidTile : public Tile {
protected:
    int liquidType;
    int calmTileId;
    int tileId;
    int spreadSpeed = 1;

    bool shouldRenderFace(Level* level, int x, int y, int z, int layer, int face) override;
private:
    virtual bool checkWater(Level* level, int x, int y, int z, int depth);
public:
    LiquidTile(int id, int liquidType);
    ~LiquidTile();
    void tick(Level* level, int x, int y, int z, Random* random) override;
    virtual bool updateWater(Level* level, int x, int y, int z, int depth);
    void renderFace(Tessellator& t, int x, int y, int z, int face) override;
    void neighborChanged(Level* level, int x, int y, int z, int type) override;
    bool mayPick() override;
    AABB* getAABB(int x, int y, int z) const override;
    bool blocksLight() override;
    bool isSolid() override;
    int getLiquidType() override;
};