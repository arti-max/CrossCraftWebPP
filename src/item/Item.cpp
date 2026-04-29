#include "item/Item.hpp"
#include "util/Random.hpp"
#include "level/tile/Tile.hpp"
#include "util/Logger.hpp"
#include "player/Player.hpp"
#include "item/TakeEntityAnim.hpp"


std::vector<ItemModel*> Item::models;

Item::Item(Level* level, float x, float y, float z, int resourceId) : Entity(level) {
    this->resourceId = resourceId;
    this->setSize(0.25f, 0.25f);
    this->heightOffset = this->bbHeight / 2.0f;
    this->setPos(x, y, z);
    this->yd = 0.2f;
    this->xd = (float)(Random::random() * 0.20000000298023224 - 0.10000000149011612);
    this->zd = (float)(Random::random() * 0.20000000298023224 - 0.10000000149011612);
    this->rot = (float)(Random::random() * 360.0);
    Logger::logf(PREFIX_DEBUG, "Item spawned at: %f %f %f ID: %d\n", x, y, z, resourceId);
    this->makeStepSound = false;
}

void Item::tick() {
    Entity::tick();

    this->yd -= 0.04f;
    this->move(xd, yd, zd);
    this->xd *= 0.98f;
    this->yd *= 0.98f;
    this->zd *= 0.98f;
    if (this->onGround) {
        this->xd *= 0.7f;
        this->zd *= 0.7f;
        this->yd *= -0.5f;
    }
    this->age++;
    this->hoverAnim++;
    if (this->age >= 6000) {
        this->remove();
    }
}

void Item::render(float partialTicks, Textures* textures) {
    Tessellator& t = Tessellator::getInstance();
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("terrain", GL_NEAREST));
    float brightness = this->level->getBrightness((int)this->x, (int)this->y, (int)this->z);
    float rotation = this->rot + ((float)this->hoverAnim + partialTicks) * 3.0f;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor4f(brightness, brightness, brightness, 1.0f);
    float levitation = sin(this->hoverAnim / 10.0) * 0.1 + 0.1f;
    float interpX = this->xo + (this->x - this->xo) * partialTicks;
    float interpY = this->yo + (this->y - this->yo) * partialTicks;
    float interpZ = this->zo + (this->z - this->zo) * partialTicks;
    glTranslatef(interpX, interpY+levitation, interpZ);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);
    // glScalef(0.25f, 0.25f, 0.25f);
    // glTranslatef(-0.5f, -0.5f, -0.5f);
    models[this->resourceId]->generateList();
    glDisable(GL_TEXTURE_2D);
    float glow;
    glow = (glow = (glow = std::sin(rotation / 10.0F) * 0.5F + 0.5F) * glow) * glow;

        glDisable(GL_TEXTURE_2D); // Чтобы рисовало сплошным цветом
        glEnable(GL_BLEND);
        glDisable(GL_LIGHTING);   // Выключаем свет (чтобы свечение было "чистым")
        glDisable(GL_ALPHA_TEST); // ВАЖНО! Отключаем отсечение прозрачности
        
        // Аддитивный блендинг (лучше всего для свечения)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
        
        // КЛЮЧЕВОЙ МОМЕНТ: Запрещаем писать в буфер глубины!
        // Это значит, что свечение нарисуется ПОВЕРХ, но не будет перекрывать сам предмет.
        glDepthMask(GL_FALSE); 
        
        // Устанавливаем цвет (ModelPart теперь его подхватит)
        // Попробуйте Красный цвет для теста, чтобы убедиться что блендинг работает
        // glColor4f(1.0f, 0.0f, 0.0f, glow * 0.4f); 
        glColor4f(1.0f, 1.0f, 1.0f, glow * 0.4f); 

        // Чуть-чуть увеличиваем масштаб, чтобы избежать Z-fighting на гранях
        glPushMatrix();
        
        models[this->resourceId]->generateList();
        
        glPopMatrix();
        
        // Восстанавливаем стейт
        glDepthMask(GL_TRUE);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void Item::playerTouch(Player* player) {
    if (player->inventory->addItem(this->resourceId)) {
        this->level->addEntity(new TakeEntityAnim(this->level, this, player));
        this->remove();
    }
}

void Item::initModels() {
    if (!models.empty()) return;

    models.resize(256, nullptr);

    for (int t = 0; t < 256; ++t) {
        Tile* tile;
        if ((tile = Tile::tiles[t]) != nullptr) {
            Item::models[t] = new ItemModel(tile->textureId);
        }
    }

    // Tessellator& t = Tessellator::getInstance();
    // for (int i = 0; i < 256; i++) {
    //     Tile* tile = Tile::tiles[i];
    //     if (tile != nullptr) {
    //         int listId = glGenLists(1);
    //         glNewList(listId, GL_COMPILE);
    //         t.begin();
    //         int tex = tile->getTexture(0);

    //         const float atlasSize = 16.0f;
    //         const float tilePixels = 16.0f;
    //         const float atlasPixels = 256.0f;

    //         float tileUVSize = tilePixels / atlasPixels;
    //         float size = tileUVSize * 0.3f;
    //         float offset = tileUVSize * 0.25f;

    //         float epsilon = 0.0f; 

    //         float col = (float)(tex % (int)(atlasSize));
    //         float row = (float)(tex / (int)(atlasSize));

    //         float u0 = (col * tilePixels) / atlasPixels + epsilon;
    //         float u1 = u0 + (tilePixels / atlasPixels) - (epsilon * 2.0f);
    //         float v0 = (row * tilePixels) / atlasPixels + epsilon;
    //         float v1 = v0 + (tilePixels / atlasPixels) - (epsilon * 2.0f);
            
    //         u0 = u0 + offset; 
    //         u1 = u0 + offset + size;
    //         v0 = v0 + offset;
    //         v1 = v0 + offset + size;

    //         float x0 = (float)0 + 0;
    //         float x1 = (float)0 + 1;
    //         float y0 = (float)0 + 0;
    //         float y1 = (float)0 + 1;
    //         float z0 = (float)0 + 0;
    //         float z1 = (float)0 + 1;
    //         t.vertexUV(x0, y0, z1, u0, v1);
    //         t.vertexUV(x0, y0, z0, u0, v0);
    //         t.vertexUV(x1, y0, z0, u1, v0);
    //         t.vertexUV(x1, y0, z1, u1, v1);
    //         t.vertexUV(x1, y1, z1, u1, v1);
    //         t.vertexUV(x1, y1, z0, u1, v0);
    //         t.vertexUV(x0, y1, z0, u0, v0);
    //         t.vertexUV(x0, y1, z1, u0, v1);
    //         t.vertexUV(x0, y1, z0, u1, v0);
    //         t.vertexUV(x1, y1, z0, u0, v0);
    //         t.vertexUV(x1, y0, z0, u0, v1);
    //         t.vertexUV(x0, y0, z0, u1, v1);
    //         t.vertexUV(x0, y1, z1, u0, v0);
    //         t.vertexUV(x0, y0, z1, u0, v1);
    //         t.vertexUV(x1, y0, z1, u1, v1);
    //         t.vertexUV(x1, y1, z1, u1, v0);
    //         t.vertexUV(x0, y1, z1, u1, v0);
    //         t.vertexUV(x0, y1, z0, u0, v0);
    //         t.vertexUV(x0, y0, z0, u0, v1);
    //         t.vertexUV(x0, y0, z1, u1, v1);
    //         t.vertexUV(x1, y0, z1, u0, v1);
    //         t.vertexUV(x1, y0, z0, u1, v1);
    //         t.vertexUV(x1, y1, z0, u1, v0);
    //         t.vertexUV(x1, y1, z1, u0, v0);
    //         t.end();
    //         glEndList();

    //         models[i] = listId;
    //     }
    // }
}