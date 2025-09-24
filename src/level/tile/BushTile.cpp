#include "level/tile/BushTile.hpp"

Bush::Bush(int id) : Tile(id) {
    this->textureId = 15;
}

void Bush::tick(Level* level, int x, int y, int z, Random* random) {
    int tileIdBelow = level->getTile(x, y-1, z);

    if (!level->isLit(x, y, z) || (tileIdBelow != Tile::grass->id && tileIdBelow != Tile::dirt->id)) {
        level->setTile(x, y, z, 0);
    }
}

void Bush::render(Tessellator& t, Level* level, int layer, int x, int y, int z) {
    if (level->isLit(x, y, z) ^ (layer != 1)) {
        return;
    }

    int textureId = this->textureId;

    const float atlasSize = 16.0f;
    const float tilePixels = 16.0f;
    const float atlasPixels = 256.0f;

    const float col = static_cast<float>(textureId % static_cast<int>(atlasSize));
    const float row = static_cast<float>(textureId / static_cast<int>(atlasSize));

    const float u0 = col * tilePixels / atlasPixels;
    const float v0 = row * tilePixels / atlasPixels;
    const float u1 = u0 + tilePixels / atlasPixels;
    const float v1 = v0 + tilePixels / atlasPixels;

    t.color(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < 2; ++i) {
        float angle = i * M_PI / 2.0f + M_PI / 4.0f;
        float sin = std::sin(angle) * 0.5f;
        float cos = std::cos(angle) * 0.5f;

        float x0 = x + 0.5f - sin;
        float x1 = x + 0.5f + sin;
        float y0 = y + 0.0f;
        float y1 = y + 1.0f;
        float z0 = z + 0.5f - cos;
        float z1 = z + 0.5f + cos;

        t.vertexUV(x0, y1, z0, u0, v0);
        t.vertexUV(x1, y1, z1, u1, v0);
        t.vertexUV(x1, y0, z1, u1, v1);
        t.vertexUV(x0, y0, z0, u0, v1);

        t.vertexUV(x1, y1, z1, u1, v0);
        t.vertexUV(x0, y1, z0, u0, v0);
        t.vertexUV(x0, y0, z0, u0, v1);
        t.vertexUV(x1, y0, z1, u1, v1);
    }
}

AABB* Bush::getAABB(int x, int y, int z) const {
    return nullptr;
}

bool Bush::blocksLight() {
    return false;
}

bool Bush::isSolid() {
    return false;
}