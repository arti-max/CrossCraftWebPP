#include "level/Chunk.hpp"

Tessellator& Chunk::t = Tessellator::getInstance();

Chunk::Chunk(Level& level, int x0, int y0, int z0, int x1, int y1, int z1) : 
    level(level),
    boundingBox((float)x0, (float)y0, (float)z0, (float)x1, (float)y1, (float)z1) {
    this->x0 = x0;
    this->y0 = y0;
    this->z0 = z0;
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
    
    this->x = (x0 + x1) / 2.0f;
    this->y = (y0 + y1) / 2.0f;
    this->z = (z0 + z1) / 2.0f;

    this->lists = glGenLists(3);
}

void Chunk::rebuild(int layer) {
    glNewList(this->lists + layer, GL_COMPILE);
    glEnable(GL_TEXTURE_2D);
    this->t.begin();

    for (int x = this->x0; x < this->x1; ++x) {
        for (int y = this->y0; y < this->y1; ++y) {
            for (int z = this->z0; z < this->z1; ++z) {
                int id = this->level.getTile(x, y, z);

                if (id > 0) {
                    Tile::tiles[id]->render(this->t, this->level, layer, x, y, z);
                }
            }
        }
    }

    this->t.end();
    glEndList();
}

void Chunk::rebuild() {
    this->updates++;
    this->rebuild(0);
    this->rebuild(1);
    this->rebuild(2);
    this->dirty = false;
}

void Chunk::render(int layer) {
    glCallList(this->lists + layer);
}

bool Chunk::isDirty() {
    return this->dirty;
}

void Chunk::setDirty() {
    this->dirty = true;
}

void Chunk::reset() {
    this->dirty = true;

    for (int i = 0; i < 3; ++i) {
        glNewList(this->lists + i, GL_COMPILE);
        glEndList();
    }
}

float Chunk::distanceToSqr(Player* player) {
    float xd = player->x - this->x;
    float yd = player->y - this->y;
    float zd = player->z - this->z;
    return xd*xd + yd*yd + zd*zd;
}