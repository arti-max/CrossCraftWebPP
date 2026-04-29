#pragma once
#include "Entity.hpp"
#include "render/Textures.hpp"
#include "render/Tessellator.hpp"
#include "item/ItemModel.hpp"
#include <vector>

class Item : public Entity {
private:
    int resourceId;
    int age = 0;
    float hoverAnim;
    int rot;
public:
    static std::vector<ItemModel*> models; 
    Item(Level* level, float x, float y, float z, int resourceId);

    void tick() override;
    void render(float partialTicks, Textures* textures) override;
    void renderFace(Tessellator& t, int x, int y, int z, int face);
    void playerTouch(Player* p) override;
    static void initModels(); 
};