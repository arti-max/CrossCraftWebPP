#include "gui/LevelGenerateScreen.hpp"
#include "CrossCraft.hpp"

LevelGenerateScreen::LevelGenerateScreen(Screen* parent) : Screen::Screen() {
    this->parent = parent;
}

void LevelGenerateScreen::init() {
    this->buttons.clear();
    this->buttonNames.push_back("Small");
    this->buttonNames.push_back("Normal");
    this->buttonNames.push_back("Huge");

    for (int i = 0; i < 3; ++i) {
        this->buttons.push_back(new Button(i, this->width / 2 - 100, this->height / 4 + i * 24, 200, 20, this->buttonNames[i]));
    }

    this->buttons.push_back(new Button(5, this->width / 2 - 100, this->height / 4 + 144, 200, 20, "Cancel"));
}

void LevelGenerateScreen::buttonClicked(Button* btn) {
    if (btn->enabled) {
        if (btn->id == 0) {
            this->cc->generateNewLevel(128, 128, 64);
            this->cc->setScreen(nullptr);
            this->cc->grabMouse();
        }
        if (btn->id == 1) {
            this->cc->generateNewLevel(256, 256, 64);
            this->cc->setScreen(nullptr);
            this->cc->grabMouse();
        }
        if (btn->id == 2) {
            this->cc->generateNewLevel(512, 512, 64);
            this->cc->setScreen(nullptr);
            this->cc->grabMouse();
        }
        if (btn->id == 5) {
            this->cc->setScreen(parent);
        }
    }
}

void LevelGenerateScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 537199872, -1607454624);
    this->drawCenteredString(this->title.c_str(), this->width / 2, 40, 0xFFFFFFFF);
    Screen::render(xMouse, yMouse);
}