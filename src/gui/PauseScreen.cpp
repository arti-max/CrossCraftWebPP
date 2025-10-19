#include "gui/PauseScreen.hpp"
#include "CrossCraft.hpp"
#include "gui/LevelLoadScreen.hpp"
#include "gui/LevelSaveScreen.hpp"
#include "gui/LevelGenerateScreen.hpp"

void PauseScreen::init() {
    this->buttons.push_back(new Button(0, this->width / 2 - 100, this->height / 3 + 0, 200, 20, "Generate new level"));
    this->buttons.push_back(new Button(1, this->width / 2 - 100, this->height / 3 + 32, 200, 20, "Save level..."));
    this->buttons.push_back(new Button(2, this->width / 2 - 100, this->height / 3 + 64, 200, 20, "Load level..."));
    this->buttons.push_back(new Button(3, this->width / 2 - 100, this->height / 3 + 96, 200, 20, "Back to game"));

    if (this->cc->user == nullptr) {
        this->buttons[1]->enabled = false;
        this->buttons[2]->enabled = false;
    }
}

void PauseScreen::buttonClicked(Button* btn) {
    if (btn->id == 0) {
        this->cc->setScreen(new LevelGenerateScreen(this));
    }
    if (this->cc->user != nullptr) {
        if (btn->id == 1) {
            this->cc->setScreen(new LevelSaveScreen(this));
        }
        if (btn->id == 2) {
            this->cc->setScreen(new LevelLoadScreen(this));
        }
    }
    if (btn->id == 3) {
        this->cc->grabMouse();
        this->cc->setScreen(nullptr);
    }
}

void PauseScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 0x20050500, 0xA0303060);
    this->drawCenteredString("Game Menu", this->width / 2, 40, 0xFFFFFFFF);
    Screen::render(xMouse, yMouse);
}