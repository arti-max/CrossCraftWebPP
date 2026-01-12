#include "level/tile/GrassTile.hpp"
#include "level/tile/BushTile.hpp"
#include "level/tile/LiquidTile.hpp"
#include "level/tile/CalmLiquidTile.hpp"
#include "level/tile/FallingTile.hpp"
#include "level/tile/LogTile.hpp"
#include "level/tile/TransparentTile.hpp"
#include "level/tile/SpongeTile.hpp"
#include "level/tile/GlassTile.hpp"
#include "item/Item.hpp"
#include "level/tile/Tile.hpp"

std::array<Tile*, 256> Tile::tiles = {nullptr};
const Tile* Tile::empty = nullptr;

static Tile rockTile(1, 1);
static GrassTile grassTile(2);
static Tile dirtTile(3, 3);
static Tile cobbleTile(4, 5);
static Tile woodTile(5, 2);
static Bush bushTile(6, 15);
static Tile unbreakableTile(7, 17);
static LiquidTile waterTile(8, 1);
static CalmLiquidTile calmWaterTile(9, 1);
static LiquidTile lavaTile(10, 2);
static CalmLiquidTile calmLavaTile(11, 2);
static FallingTile gravelTile(12, 19);
static FallingTile sandTile(13, 18);
static LogTile logTile(14);
static TransparentTile leavesTile(15, 22);
static Tile goldOreTile(16, 32);
static Tile ironOreTile(17, 33);
static Tile coalOreTile(18, 34);
static SpongeTile spongeTile(19);
static GlassTile glassTile(20);
static Tile woolTile1(21, 64);
static Tile woolTile2(22, 65);
static Tile woolTile3(23, 66);
static Tile woolTile4(24, 67);
static Tile woolTile5(25, 68);
static Tile woolTile6(26, 69);
static Tile woolTile7(27, 70);
static Tile woolTile8(28, 71);
static Tile woolTile9(29, 72);
static Tile woolTile10(30, 73);
static Tile woolTile11(31, 74);
static Tile woolTile12(32, 75);
static Tile woolTile13(33, 76);
static Tile woolTile14(34, 77);
static Tile woolTile15(35, 78);
static Tile woolTile16(36, 79);
static Bush redFlowerTile(37, 12);
static Bush yellowFlowerTile(38, 13);
static Bush redMushroomTile(39, 28);
static Bush brownMushroomTile(40, 29);
static Tile goldBlockTile(41, 40);


const Tile* Tile::rock = rockTile.setData(SoundType::stone, 1.0f);
const Tile* Tile::grass = grassTile.setData(SoundType::grass, 1.0f);
const Tile* Tile::dirt = dirtTile.setData(SoundType::grass, 1.0f);
const Tile* Tile::cobblestone = cobbleTile.setData(SoundType::stone, 1.0f);
const Tile* Tile::wood = woodTile.setData(SoundType::wood, 1.0f);
const Tile* Tile::bush = bushTile.setData(SoundType::none, 1.0f);
const Tile* Tile::unbreakable = unbreakableTile.setData(SoundType::stone, 1.0f);
const Tile* Tile::water = waterTile.setData(SoundType::none, 1.0f);
const Tile* Tile::calmWater = calmWaterTile.setData(SoundType::none, 1.0f);
const Tile* Tile::lava = lavaTile.setData(SoundType::none, 1.0f);
const Tile* Tile::calmLava = calmLavaTile.setData(SoundType::none, 1.0f);
const Tile* Tile::gravel = gravelTile.setData(SoundType::gravel, 1.0f);
const Tile* Tile::sand = sandTile.setData(SoundType::gravel, 1.0f);
const Tile* Tile::log = logTile.setData(SoundType::wood, 1.0f);
const Tile* Tile::leaves = leavesTile.setData(SoundType::leaves, 0.4f);
const Tile* Tile::goldOre = goldOreTile.setData(SoundType::stone, 1.0f);
const Tile* Tile::ironOre = ironOreTile.setData(SoundType::stone, 1.0f);
const Tile* Tile::coalOre = coalOreTile.setData(SoundType::stone, 1.0f);
const Tile* Tile::sponge = spongeTile.setData(SoundType::cloth, 0.9f);
const Tile* Tile::glass = glassTile.setData(SoundType::metal, 1.0f);
const Tile* Tile::wool1 = woolTile1.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool2 = woolTile2.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool3 = woolTile3.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool4 = woolTile4.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool5 = woolTile5.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool6 = woolTile6.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool7 = woolTile7.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool8 = woolTile8.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool9 = woolTile9.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool10 = woolTile10.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool11 = woolTile11.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool12 = woolTile12.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool13 = woolTile13.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool14 = woolTile14.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool15 = woolTile15.setData(SoundType::cloth, 1.0f);
const Tile* Tile::wool16 = woolTile16.setData(SoundType::cloth, 1.0f);
const Tile* Tile::redFlower = redFlowerTile.setData(SoundType::none, 1.0f);
const Tile* Tile::yellowFlower = yellowFlowerTile.setData(SoundType::none, 1.0f);
const Tile* Tile::redMushroom = redMushroomTile.setData(SoundType::none, 1.0f);
const Tile* Tile::brownMushroom = brownMushroomTile.setData(SoundType::none, 1.0f);
const Tile* Tile::goldBlock = goldBlockTile.setData(SoundType::metal, 1.0f);

Tile::Tile(int id) {
    tiles[id] = this;
    this->id = id;
    this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    this->textureId = 0;
}

Tile::Tile(int id, int texture) : Tile(id) {
    this->textureId = texture;
}

Tile* Tile::setData(const SoundType& st, float particleGravity) {
    this->st = &st;
    this->particleGravity = particleGravity;
    return this;
}

bool Tile::shouldRenderFace(Level* level, int x, int y, int z, int layer, int face) {
    if (layer == 1) {
        return false;
    } else {
        return !level->isSolidTile(x, y, z);
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

bool Tile::render(Tessellator& t, Level* level, int layer, int x, int y, int z) {
    bool rendered = false;
    float c1 = 0.5f;
    float c2 = 0.8f;
    float c3 = 0.6f;

    if (this->shouldRenderFace(level, x, y - 1, z, layer, 0)) {
        float brightness = this->getBrightness(level, x, y - 1, z);
        t.color(brightness * c1, brightness * c1, brightness * c1);
        this->renderFace(t, x, y, z, 0);
        rendered = true;
    }
    if (this->shouldRenderFace(level, x, y + 1, z, layer, 1)) {
        float brightness = this->getBrightness(level, x, y + 1, z);
        t.color(brightness * 1.0f, brightness * 1.0f, brightness * 1.0f);
        this->renderFace(t, x, y, z, 1);
        rendered = true;
    }
    if (this->shouldRenderFace(level, x, y, z - 1, layer, 2)) {
        float brightness = this->getBrightness(level, x, y, z - 1);
        t.color(brightness * c2, brightness * c2, brightness * c2);
        this->renderFace(t, x, y, z, 2);
        rendered = true;
    }
    if (this->shouldRenderFace(level, x, y, z + 1, layer, 3)) {
        float brightness = this->getBrightness(level, x, y, z + 1);
        t.color(brightness * c2, brightness * c2, brightness * c2);
        this->renderFace(t, x, y, z, 3);
        rendered = true;
    }
    if (this->shouldRenderFace(level, x - 1, y, z, layer, 4)) {
        float brightness = this->getBrightness(level, x - 1, y, z);
        t.color(brightness * c3, brightness * c3, brightness * c3);
        this->renderFace(t, x , y, z, 4);
        rendered = true;
    }
    if (this->shouldRenderFace(level, x + 1, y, z, layer, 5)) {
        float brightness = this->getBrightness(level, x + 1, y, z);
        t.color(brightness * c3, brightness * c3, brightness * c3);
        this->renderFace(t, x, y, z, 5);
        rendered = true;
    }

    return rendered;
}

float Tile::getBrightness(Level* level, int x, int y, int z) {
    return level->getBrightness(x, y, z);
}

void Tile::renderFace(Tessellator& t, int x, int y, int z, int face) {
    const int tex = this->getTexture(face);

    const float atlasSize = 16.0f;
    const float tilePixels = 16.0f;
    const float atlasPixels = 256.0f;
    
    const float epsilon = 0.01f / atlasPixels; 

    const float col = static_cast<float>(tex % static_cast<int>(atlasSize));
    const float row = static_cast<float>(tex / static_cast<int>(atlasSize));

    const float u0 = (col * tilePixels) / atlasPixels + epsilon;
    const float u1 = u0 + (tilePixels / atlasPixels) - (epsilon * 2.0f);
    const float v0 = (row * tilePixels) / atlasPixels + epsilon;
    const float v1 = v0 + (tilePixels / atlasPixels) - (epsilon * 2.0f);

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

void Tile::renderBackFace(Tessellator& t, int x, int y, int z, int face) {
    const int tex = this->getTexture(face);

    const float atlasSize = 16.0f;
    const float tilePixels = 16.0f;
    const float atlasPixels = 256.0f;
    
    const float epsilon = 0.01f / atlasPixels;

    const float col = static_cast<float>(tex % static_cast<int>(atlasSize));
    const float row = static_cast<float>(tex / static_cast<int>(atlasSize));

    const float u0 = (col * tilePixels) / atlasPixels + epsilon;
    const float u1 = u0 + (tilePixels / atlasPixels) - (epsilon * 2.0f);
    const float v0 = (row * tilePixels) / atlasPixels + epsilon;
    const float v1 = v0 + (tilePixels / atlasPixels) - (epsilon * 2.0f);

    float x0 = static_cast<float>(x) + this->minX;
    float x1 = static_cast<float>(x) + this->maxX;
    float y0 = static_cast<float>(y) + this->minY;
    float y1 = static_cast<float>(y) + this->maxY;
    float z0 = static_cast<float>(z) + this->minZ;
    float z1 = static_cast<float>(z) + this->maxZ;
    
    if (face == 0) {
        t.vertexUV(x1, y0, z1, u1, v1);
        t.vertexUV(x1, y0, z0, u1, v0);
        t.vertexUV(x0, y0, z0, u0, v0);
        t.vertexUV(x0, y0, z1, u0, v1);
    } else if (face == 1) {
        t.vertexUV(x0, y1, z1, u0, v1);
        t.vertexUV(x0, y1, z0, u0, v0);
        t.vertexUV(x1, y1, z0, u1, v0);
        t.vertexUV(x1, y1, z1, u1, v1);
    } else if (face == 2) {
        t.vertexUV(x0, y0, z0, u1, v1);
        t.vertexUV(x1, y0, z0, u0, v1);
        t.vertexUV(x1, y1, z0, u0, v0);
        t.vertexUV(x0, y1, z0, u1, v0);
    } else if (face == 3) {
        t.vertexUV(x1, y1, z1, u1, v0);
        t.vertexUV(x1, y0, z1, u1, v1);
        t.vertexUV(x0, y0, z1, u0, v1);
        t.vertexUV(x0, y1, z1, u0, v0);
    } else if (face == 4) {
        t.vertexUV(x0, y0, z1, u1, v1);
        t.vertexUV(x0, y0, z0, u0, v1);
        t.vertexUV(x0, y1, z0, u0, v0);
        t.vertexUV(x0, y1, z1, u1, v0);
    } else if (face == 5) {
        t.vertexUV(x1, y1, z1, u0, v0);
        t.vertexUV(x1, y1, z0, u1, v0);
        t.vertexUV(x1, y0, z0, u1, v1);
        t.vertexUV(x1, y0, z1, u0, v1);
    }
}

void Tile::renderFaceNoTexture(Player* player, Tessellator& t, int x, int y, int z, int face) {
    float x0 = (float)x + this->minX;
    float x1 = (float)x + this->maxX;
    float y0 = (float)y + this->minY;
    float y1 = (float)y + this->maxY;
    float z0 = (float)z + this->minZ;
    float z1 = (float)z + this->maxZ;
    if (face == 0) {
        t.vertex(x0, y0, z1);
        t.vertex(x0, y0, z0);
        t.vertex(x1, y0, z0);
        t.vertex(x1, y0, z1);
    }
    if (face == 1) {
        t.vertex(x1, y1, z1);
        t.vertex(x1, y1, z0);
        t.vertex(x0, y1, z0);
        t.vertex(x0, y1, z1);
    }
    if (face == 2) {
        t.vertex(x0, y1, z0);
        t.vertex(x1, y1, z0);
        t.vertex(x1, y0, z0);
        t.vertex(x0, y0, z0);
    }
    if (face == 3) {
        t.vertex(x0, y1, z1);
        t.vertex(x0, y0, z1);
        t.vertex(x1, y0, z1);
        t.vertex(x1, y1, z1);
    }
    if (face == 4) {
        t.vertex(x0, y1, z1);
        t.vertex(x0, y1, z0);
        t.vertex(x0, y0, z0);
        t.vertex(x0, y0, z1);
    }
    if (face == 5) {
        t.vertex(x1, y0, z1);
        t.vertex(x1, y0, z0);
        t.vertex(x1, y1, z0);
        t.vertex(x1, y1, z1);
    }
}

void Tile::onDestroy(Level* level, int x, int y, int z, ParticleEngine* engine, bool drop) {
    Random rnd = Random();
    if (drop) {
        float spr = 0.7f;
        float dropX = rnd.nextFloat() * spr + (1.0f - spr) * 0.5f;
        float dropY = rnd.nextFloat() * spr + (1.0f - spr) * 0.5f;
        float dropZ = rnd.nextFloat() * spr + (1.0f - spr) * 0.5f;
        level->addEntity(new Item(level, x+dropX, y+dropY, z+dropZ, this->id));
    }
    int spread = 4;

    for (int offsetX = 0; offsetX < spread; offsetX++) {
        for (int offsetY = 0; offsetY < spread; offsetY++) {
            for (int offsetZ = 0; offsetZ < spread; offsetZ++) {
                float targetX = x + (offsetX + 0.5f) / spread;
                float targetY = y + (offsetY + 0.5f) / spread;
                float targetZ = z + (offsetZ + 0.5f) / spread;

                float motionX = targetX - x - 0.5f;
                float motionY = targetY - y - 0.5f;
                float motionZ = targetZ - z - 0.5f;

                Particle* p = (new Particle(level, targetX, targetY, targetZ, motionX, motionY, motionZ, this));
                engine->add(p);
            }
        }
    }
}

void Tile::neighborChanged(Level* level, int x, int y, int z, int type) {
    // no implementation
}

void Tile::tick(Level* level, int x, int y, int z, Random* random) {
    // no implementation
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

void Tile::onBlockAdded(Level* level, int x, int y, int z) {
    // No implementation
}