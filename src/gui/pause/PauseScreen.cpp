#include "gui/pause/PauseScreen.hpp"
#include "CrossCraft.hpp"
#include "gui/pause/LevelLoadScreen.hpp"
#include "gui/pause/LevelSaveScreen.hpp"
#include "gui/pause/LevelGenerateScreen.hpp"
#include "gui/pause/settings/SettingsScreen.hpp"

void PauseScreen::init() {
    this->buttons.push_back(new Button(0, this->width / 2 - 100, this->height / 4, 200, 20, "Options..."));
    this->buttons.push_back(new Button(1, this->width / 2 - 100, this->height / 4 + 24, 200, 20, "Generate new level"));
    this->buttons.push_back(new Button(2, this->width / 2 - 100, this->height / 4 + 48, 200, 20, "Save level..."));
    this->buttons.push_back(new Button(3, this->width / 2 - 100, this->height / 4 + 72, 200, 20, "Load level..."));
    this->buttons.push_back(new Button(4, this->width / 2 - 100, this->height / 4 + 120, 200, 20, "Back to game"));

    if (this->cc->userData == nullptr) {
        this->buttons[2]->enabled = false;
        this->buttons[3]->enabled = false;
    }
    if (this->cc->mpMode) {
        this->buttons[1]->enabled = false;
        this->buttons[2]->enabled = false;
        this->buttons[3]->enabled = false;
    }
}

void PauseScreen::buttonClicked(Button* btn) {
    if (btn->id == 0) {
        this->cc->setScreen(new SettingsScreen(this, this->cc->settings));
    }
    if (btn->id == 1) {
        this->cc->setScreen(new LevelGenerateScreen(this));
    }
    if (this->cc->userData != nullptr) {
        if (btn->id == 2) {
            this->cc->setScreen(new LevelSaveScreen(this));
        }
        if (btn->id == 3) {
            this->cc->setScreen(new LevelLoadScreen(this));
        }
    }
    if (btn->id == 4) {
        // this->cc->grabMouse();
        this->cc->setScreen(nullptr);
        this->cc->waitingForFocus = true;
    }
}

void PauseScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 0x20050500, 0xA0303060);
    this->drawCenteredString("Game Menu", this->width / 2, 40, 0xFFFFFFFF);
    Screen::render(xMouse, yMouse);
}

void PauseScreen::tick() {
    // if (Mouse::isGrabbed()) {
    //     this->cc->setScreen(nullptr);
    // }
}