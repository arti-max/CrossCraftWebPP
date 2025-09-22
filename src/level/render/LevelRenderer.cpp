#include "level/render/LevelRenderer.hpp"
#include "render/Tessellator.hpp"
#include <iostream>
#include <cmath>

LevelRenderer::LevelRenderer(Level* level, Textures* textures) 
    : level(level), textures(textures) {
    level->addListener(this);
    surroundLists = glGenLists(2);
    allChanged();
}

LevelRenderer::~LevelRenderer() {
    for (Chunk* chunk : chunks) {
        delete chunk;
    }
    
    glDeleteLists(surroundLists, 2);
}

void LevelRenderer::allChanged() {
    lX = -900000.0f;
    lY = -900000.0f;
    lZ = -900000.0f;
    
    xChunks = (level->width + CHUNK_SIZE - 1) / CHUNK_SIZE;
    yChunks = (level->depth + CHUNK_SIZE - 1) / CHUNK_SIZE;
    zChunks = (level->height + CHUNK_SIZE - 1) / CHUNK_SIZE;
    
    for (Chunk* chunk : chunks) {
        delete chunk;
    }
    
    chunks.clear();
    sortedChunks.clear();
    chunks.reserve(xChunks * yChunks * zChunks);
    sortedChunks.reserve(xChunks * yChunks * zChunks);
    
    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            for (int z = 0; z < zChunks; ++z) {
                int x0 = x * CHUNK_SIZE;
                int y0 = y * CHUNK_SIZE;
                int z0 = z * CHUNK_SIZE;
                int x1 = (x + 1) * CHUNK_SIZE;
                int y1 = (y + 1) * CHUNK_SIZE;
                int z1 = (z + 1) * CHUNK_SIZE;
                
                if (x1 > level->width) x1 = level->width;
                if (y1 > level->depth) y1 = level->depth;
                if (z1 > level->height) z1 = level->height;
                
                Chunk* chunk = new Chunk(*level, x0, y0, z0, x1, y1, z1);
                chunks.push_back(chunk);
                sortedChunks.push_back(chunk);
            }
        }
    }
    
    glNewList(surroundLists + 0, GL_COMPILE);
    compileSurroundingGround();
    glEndList();
    
    glNewList(surroundLists + 1, GL_COMPILE);
    compileSurroundingWater();
    glEndList();
    
    // Помечаем все чанки как грязные
    for (Chunk* chunk : chunks) {
        chunk->reset();
    }
    
    std::cout << "LevelRenderer initialized: " << chunks.size() << " chunks" << std::endl;
}

std::vector<Chunk*> LevelRenderer::getAllDirtyChunks() {
    std::vector<Chunk*> dirty;
    
    for (Chunk* chunk : chunks) {
        if (chunk->isDirty()) {
            dirty.push_back(chunk);
        }
    }
    
    return dirty;
}

void LevelRenderer::render(Player* player, int layer) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("terrain", GL_NEAREST));
    
    float xd = player->x - lX;
    float yd = player->y - lY;
    float zd = player->z - lZ;
    
    if (xd * xd + yd * yd + zd * zd > 64.0f) {
        lX = player->x;
        lY = player->y;
        lZ = player->z;
        std::sort(sortedChunks.begin(), sortedChunks.end(), DistanceSorter(player));
    }
    
    for (Chunk* chunk : sortedChunks) {
        if (chunk->visible) {
            float dd = static_cast<float>(256 / (1 << drawDistance));
            if (drawDistance == 0 || chunk->distanceToSqr(player) < dd * dd) {
                chunk->render(layer);
            }
        }
    }
    
    glDisable(GL_TEXTURE_2D);
}

void LevelRenderer::renderSurroundingGround() {
    glCallList(surroundLists + 0);
}

void LevelRenderer::compileSurroundingGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("rock", GL_NEAREST));
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    Tessellator& t = Tessellator::getInstance();
    float y = level->getGroundLevel() - 2.0f;
    int s = 128;
    
    if (s > level->width) s = level->width;
    if (s > level->height) s = level->height;
    
    int d = 5;
    t.begin();
    
    for (int xx = -s * d; xx < level->width + s * d; xx += s) {
        for (int zz = -s * d; zz < level->height + s * d; zz += s) {
            float yy = y;
            if (xx >= 0 && zz >= 0 && xx < level->width && zz < level->height) {
                yy = 0.0f;
            }
            
            t.vertexUV(static_cast<float>(xx), yy, static_cast<float>(zz + s), 0.0f, static_cast<float>(s));
            t.vertexUV(static_cast<float>(xx + s), yy, static_cast<float>(zz + s), static_cast<float>(s), static_cast<float>(s));
            t.vertexUV(static_cast<float>(xx + s), yy, static_cast<float>(zz), static_cast<float>(s), 0.0f);
            t.vertexUV(static_cast<float>(xx), yy, static_cast<float>(zz), 0.0f, 0.0f);
        }
    }
    
    t.end();
    glFinish();
    
    glColor3f(0.8f, 0.8f, 0.8f);
    t.begin();
    
    for (int xx = 0; xx < level->width; xx += s) {
        t.vertexUV(static_cast<float>(xx), 0.0f, 0.0f, 0.0f, 0.0f);
        t.vertexUV(static_cast<float>(xx + s), 0.0f, 0.0f, static_cast<float>(s), 0.0f);
        t.vertexUV(static_cast<float>(xx + s), y, 0.0f, static_cast<float>(s), y);
        t.vertexUV(static_cast<float>(xx), y, 0.0f, 0.0f, y);
        
        t.vertexUV(static_cast<float>(xx), y, static_cast<float>(level->height), 0.0f, y);
        t.vertexUV(static_cast<float>(xx + s), y, static_cast<float>(level->height), static_cast<float>(s), y);
        t.vertexUV(static_cast<float>(xx + s), 0.0f, static_cast<float>(level->height), static_cast<float>(s), 0.0f);
        t.vertexUV(static_cast<float>(xx), 0.0f, static_cast<float>(level->height), 0.0f, 0.0f);
    }
    
    t.end();
    glFinish();
    
    glDisable(GL_TEXTURE_2D);
}

void LevelRenderer::renderSurroundingWater() {
    glCallList(surroundLists + 1);
}

void LevelRenderer::compileSurroundingWater() {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("water", GL_NEAREST));
    
    float y = level->getGroundLevel();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Tessellator& t = Tessellator::getInstance();
    int s = 128;
    if (s > level->width) s = level->width;
    if (s > level->height) s = level->height;
    
    int d = 5;
    t.begin();
    
    for (int xx = -s * d; xx < level->width + s * d; xx += s) {
        for (int zz = -s * d; zz < level->height + s * d; zz += s) {
            float yy = y - 0.1f;
            if (xx < 0 || zz < 0 || xx >= level->width || zz >= level->height) {
                // Двухсторонние квады для воды
                t.vertexUV(static_cast<float>(xx), yy, static_cast<float>(zz + s), 0.0f, static_cast<float>(s));
                t.vertexUV(static_cast<float>(xx + s), yy, static_cast<float>(zz + s), static_cast<float>(s), static_cast<float>(s));
                t.vertexUV(static_cast<float>(xx + s), yy, static_cast<float>(zz), static_cast<float>(s), 0.0f);
                t.vertexUV(static_cast<float>(xx), yy, static_cast<float>(zz), 0.0f, 0.0f);
                
                t.vertexUV(static_cast<float>(xx), yy, static_cast<float>(zz), 0.0f, 0.0f);
                t.vertexUV(static_cast<float>(xx + s), yy, static_cast<float>(zz), static_cast<float>(s), 0.0f);
                t.vertexUV(static_cast<float>(xx + s), yy, static_cast<float>(zz + s), static_cast<float>(s), static_cast<float>(s));
                t.vertexUV(static_cast<float>(xx), yy, static_cast<float>(zz + s), 0.0f, static_cast<float>(s));
            }
        }
    }
    
    t.end();
    glFinish();
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void LevelRenderer::updateDirtyChunks(Player* player) {
    std::vector<Chunk*> dirty = getAllDirtyChunks();
    if (!dirty.empty()) {
        std::sort(dirty.begin(), dirty.end(), DirtyChunkSorter(player));
        
        int rebuiltCount = 0;
        for (Chunk* chunk : dirty) {
            if (rebuiltCount >= MAX_REBUILDS_PER_FRAME) break;
            chunk->rebuild();
            rebuiltCount++;
        }
    }
}

void LevelRenderer::setDirty(int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= CHUNK_SIZE;
    x1 /= CHUNK_SIZE;
    y0 /= CHUNK_SIZE;
    y1 /= CHUNK_SIZE;
    z0 /= CHUNK_SIZE;
    z1 /= CHUNK_SIZE;
    
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (z0 < 0) z0 = 0;
    if (x1 >= xChunks) x1 = xChunks - 1;
    if (y1 >= yChunks) y1 = yChunks - 1;
    if (z1 >= zChunks) z1 = zChunks - 1;
    
    for (int x = x0; x <= x1; ++x) {
        for (int y = y0; y <= y1; ++y) {
            for (int z = z0; z <= z1; ++z) {
                int index = (x + y * xChunks) * zChunks + z;
                if (index < chunks.size()) {
                    chunks[index]->setDirty();
                }
            }
        }
    }
}

void LevelRenderer::tileChanged(int x, int y, int z) {
    setDirty(x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void LevelRenderer::lightColumnChanged(int x, int z, int y0, int y1) {
    setDirty(x - 1, y0 - 1, z - 1, x + 1, y1 + 1, z + 1);
}

void LevelRenderer::toggleDrawDistance() {
    drawDistance = (drawDistance + 1) % 4;
    std::cout << "Draw distance: " << drawDistance << std::endl;
}

void LevelRenderer::cull(Frustum& frustum) {
    for (Chunk* chunk : chunks) {
        chunk->visible = frustum.isVisible(chunk->boundingBox);
    }
}
