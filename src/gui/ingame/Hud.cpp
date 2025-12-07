#include "gui/ingame/Hud.hpp"
#include <GL/gl.h>

Hud::Hud(Textures* textures, int width, int height) : 
    textures(textures), width(width), height(height) {}

void Hud::drawTexturedModalRect(int x, int y, int u, int v, int w, int h) {
    float uScale = 1.0f / 256.0f;
    float vScale = 1.0f / 64.0f;
    Tessellator& t = Tessellator::getInstance();
    
    t.begin();
    t.vertexUV(x,     y + h, 0.0f, u * uScale,       (v + h) * vScale);
    t.vertexUV(x + w, y + h, 0.0f, (u + w) * uScale, (v + h) * vScale);
    t.vertexUV(x + w, y,     0.0f, (u + w) * uScale, v * vScale);
    t.vertexUV(x,     y,     0.0f, u * uScale,       v * vScale);
    t.end();
}

void Hud::render(Player* player, Level* level, float partialTicks) {
    int screenWidth = this->width;
    int screenHeight = this->height;
    int xStart = (screenWidth - 182) / 2;
    int yStart = screenHeight - 22; 

    glDisable(GL_DEPTH_TEST); 

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("gui", GL_NEAREST));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawTexturedModalRect(xStart, yStart, 0, 0, 182, 22);

    int slot = player->inventory->selectedSlot; 
    drawTexturedModalRect(xStart - 1 + slot * 20, yStart - 1, 0, 22, 24, 24);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT); 

    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_COLOR_MATERIAL);

    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("terrain", GL_NEAREST));

    Tessellator& t = Tessellator::getInstance();

    for (int i = 0; i < 9; ++i) {
        int tileId = player->inventory->slots[i];
        if (tileId > 0 && Tile::tiles[tileId] != nullptr) {
            glPushMatrix();
            
            float itemX = xStart + 11 + i * 20;
            float itemY = yStart + 11;
            
            glTranslatef(itemX, itemY, -50.0f);
            glScalef(10.0f, 10.0f, 10.0f);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(45.0, 0.0f, 1.0f, 0.0f);
            glTranslatef(-1.5f, 0.5f, 0.5f);
            glScalef(-1.0f, -1.0f, -1.0f);
            
            t.begin();
            Tile::tiles[tileId]->render(t, level, 0, -2, 0, 0);
            t.end();
            
            glPopMatrix();
        }
    }
    
    glDisable(GL_RESCALE_NORMAL);
    glDisable(GL_COLOR_MATERIAL);
}
