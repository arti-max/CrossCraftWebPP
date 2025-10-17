#pragma once
#include <vector>
#include <algorithm>
#include "level/render/LevelListener.hpp"
#include "level/Level.hpp"
#include "level/Chunk.hpp"
#include "level/tile/Tile.hpp"
#include "phys/AABB.hpp"
#include "render/Frustum.hpp"
#include "render/Textures.hpp"
#include "HitResult.hpp"
#include "Player.hpp"
#include "sort/DistanceSorter.hpp"
#include "sort/DirtyChunkSorter.hpp"
#include <GL/gl.h>

class LevelRenderer : public LevelListener {
private:
    Level* level;
    Textures* textures;
    std::vector<Chunk*> chunks;
    std::vector<Chunk*> sortedChunks;
    int xChunks, yChunks, zChunks;
    GLuint surroundLists;
    int drawDistance = 0;
    float lX = 0.0f;
    float lY = 0.0f;
    float lZ = 0.0f;

public:
    static const int MAX_REBUILDS_PER_FRAME = 4;
    static const int CHUNK_SIZE = 16;

    LevelRenderer(Level* level, Textures* textures);
    ~LevelRenderer();
    
    void allChanged() override;
    std::vector<Chunk*> getAllDirtyChunks();
    void render(Player* player, int layer);
    void renderSurroundingGround();
    void compileSurroundingGround();
    void renderSurroundingWater();
    void compileSurroundingWater();
    void updateDirtyChunks(Player* player);
    void setDirty(int x0, int y0, int z0, int x1, int y1, int z1);
    void tileChanged(int x, int y, int z) override;
    void lightColumnChanged(int x, int z, int y0, int y1) override;
    void toggleDrawDistance();
    void cull(Frustum& frustum);
    void renderHit(HitResult* h, Player* player, int mode, int tileType);
};
