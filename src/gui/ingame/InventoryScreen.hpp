#pragma once
#include "gui/Screen.hpp"
#include "model/ModelManager.hpp"

class InventoryScreen : public Screen {
protected:
    void mouseClicked(int x, int y, int button) override;
public:
    InventoryScreen();

    void init() override;
    void render(int xMouse, int yMouse) override;
    void renderPlayerModel(float partialTicks);

    void tick() override;
private:
    int hp = 5;
    int atk = 20;
    int def = 33;
    int spd = 50;

    float animStep = 0.0f;
    float animStepO = 0.0f;
    float run = 0.0f;
    float runO = 0.0f;

    ModelManager* models = new ModelManager();

    float timeOffs = 0;

    std::vector<int> blocks;
};