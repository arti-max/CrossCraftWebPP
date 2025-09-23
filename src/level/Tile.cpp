    #include "level/Tile.hpp"

    std::array<Tile*, 256> Tile::tiles = {};
    const Tile* Tile::empty = nullptr;

    static Tile rockTile(1, 1);
    static Tile grassTile(1, 0);
    static Tile unbreakableTile(7, 17);

    const Tile* Tile::rock = &rockTile;
    const Tile* Tile::grass = &grassTile;
    const Tile* Tile::unbreakable = &unbreakableTile;

    Tile::Tile(int id) {
        tiles[id] = this;
        this->id = id;
        this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
        this->textureId = 0;
    }

    Tile::Tile(int id, int texture) : Tile(id) {
        this->textureId = texture;
    }

    bool Tile::shouldRenderFace(Level* level, int x, int y, int z, int layer, int face) {
        bool layerOk = true;
        if (layer == 2) {
            return false;
        } else {
            if (layer >= 0) {
                layerOk = (level->isLit(x, y, z)) ^ (layer == 1);
            }

            return !level->isSolidTile(x, y, z) && layerOk;
        }
    }

    int Tile::getTexture(int face) {
        return this->textureId;
    }

    void Tile::setShape(float x0, float y0, float z0, float x1, float y1, float z1) {
        this->minX = x0;
        this->minY = y0;
        this->minZ = z0;
        this->maxX = x1;
        this->maxY = y1;
        this->maxZ = z1;
    }

    void Tile::render(Tessellator& t, Level* level, int layer, int x, int y, int z) {
        float c1 = 1.0;
        float c2 = 0.6;
        float c3 = 0.8;

        if (this->shouldRenderFace(level, x, y - 1, z, layer, 0)) {
            t.color(c1, c1, c1);
            this->renderFace(t, x, y, z, 0);
        }
        if (this->shouldRenderFace(level, x, y + 1, z, layer, 1)) {
            t.color(c1, c1, c1);
            this->renderFace(t, x, y, z, 1);
        }
        if (this->shouldRenderFace(level, x, y, z - 1, layer, 2)) {
            t.color(c2, c2, c2);
            this->renderFace(t, x, y, z, 2);
        }
        if (this->shouldRenderFace(level, x, y, z + 1, layer, 3)) {
            t.color(c2, c2, c2);
            this->renderFace(t, x, y, z, 3);
        }
        if (this->shouldRenderFace(level, x - 1, y, z, layer, 4)) {
            t.color(c3, c3, c3);
            this->renderFace(t, x , y, z, 4);
        }
        if (this->shouldRenderFace(level, x + 1, y, z, layer, 5)) {
            t.color(c3, c3, c3);
            this->renderFace(t, x, y, z, 5);
        }
    }

    void Tile::renderFace(Tessellator& t, int x, int y, int z, int face) {
        const int tex = this->getTexture(face);

        const float atlasSize = 16.0f;
        const float tilePixels = 16.0f;
        const float atlasPixels = 256.0f;
        
        const float col = static_cast<float>(tex % static_cast<int>(atlasSize));
        const float row = static_cast<float>(tex / static_cast<int>(atlasSize));

        const float u0 = col * tilePixels / atlasPixels;
        const float u1 = u0 + tilePixels / atlasPixels;
        const float v0 = row * tilePixels / atlasPixels;
        const float v1 = v0 + tilePixels / atlasPixels;

        float x0 = (float)x + this->minX;
        float x1 = (float)x + this->maxX;
        float y0 = (float)y + this->minY;
        float y1 = (float)y + this->maxY;
        float z0 = (float)z + this->minZ;
        float z1 = (float)z + this->maxZ;
        if (face == 0) {
            t.vertexUV(x0, y0, z1, u0, v1);
            t.vertexUV(x0, y0, z0, u0, v0);
            t.vertexUV(x1, y0, z0, u1, v0);
            t.vertexUV(x1, y0, z1, u1, v1);
        } else if (face == 1) {
            t.vertexUV(x1, y1, z1, u1, v1);
            t.vertexUV(x1, y1, z0, u1, v0);
            t.vertexUV(x0, y1, z0, u0, v0);
            t.vertexUV(x0, y1, z1, u0, v1);
        } else if (face == 2) {
            t.vertexUV(x0, y1, z0, u1, v0);
            t.vertexUV(x1, y1, z0, u0, v0);
            t.vertexUV(x1, y0, z0, u0, v1);
            t.vertexUV(x0, y0, z0, u1, v1);
        } else if (face == 3) {
            t.vertexUV(x0, y1, z1, u0, v0);
            t.vertexUV(x0, y0, z1, u0, v1);
            t.vertexUV(x1, y0, z1, u1, v1);
            t.vertexUV(x1, y1, z1, u1, v0);
        } else if (face == 4) {
            t.vertexUV(x0, y1, z1, u1, v0);
            t.vertexUV(x0, y1, z0, u0, v0);
            t.vertexUV(x0, y0, z0, u0, v1);
            t.vertexUV(x0, y0, z1, u1, v1);
        } else if (face == 5) {
            t.vertexUV(x1, y0, z1, u0, v1);
            t.vertexUV(x1, y0, z0, u1, v1);
            t.vertexUV(x1, y1, z0, u1, v0);
            t.vertexUV(x1, y1, z1, u0, v0);
        }
    }

    AABB* Tile::getTileAABB(int x, int y, int z) const {
        return new AABB(x, y, z, x+1, y+1, z+1);
    }

    AABB* Tile::getAABB(int x, int y, int z) const {
        return new AABB(x, y, z, x+1, y+1, z+1);
    }

    bool Tile::mayPick() {
        return true;
    }

    bool Tile::isSolid() {
        return true;
    }

    bool Tile::blocksLight() {
        return true;
    }

    int Tile::getLiquidType() {
        return 0;
    }

    bool Tile::isCalmLiquid() {
        return false;
    }
