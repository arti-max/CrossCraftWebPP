#include "gui/ingame/DeathScreen.hpp"
#include "CrossCraft.hpp"
#include "gui/pause/settings/SettingsScreen.hpp"
#include "gui/pause/LevelGenerateScreen.hpp"
#include "gui/pause/LevelLoadScreen.hpp"
#include "gui/Button.hpp"
#include <string>

void DeathScreen::init() {
    this->buttons.clear();
    this->buttons.push_back(new Button(1, this->width / 2 - 100, this->height / 4 + 72, "Generate new level..."));   
    this->buttons.push_back(new Button(2, this->width / 2 - 100, this->height / 4 + 96, "Load level"));   
    this->cc->releaseMouse();
}

void DeathScreen::buttonClicked(Button* btn) {
    if (btn->id == 0) {
        this->cc->setScreen(new SettingsScreen(this, this->cc->settings));
    }

    if (btn->id == 1) {
        this->cc->setScreen(new LevelGenerateScreen(this));
    }

    if (btn->id == 2) {
        this->cc->setScreen(new LevelLoadScreen(this));
    }
}

void DeathScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 1615855616, -1602211792);
    glPushMatrix();
    glScalef(2.0f, 2.0f, 2.0f);
    this->drawCenteredString("Game over!", this->width / 2 / 2, 30, 16777215);
    glScalef(1.0f, 1.0f, 1.0f);
    glPopMatrix();
    std::string scoreMsg = "Score: &e" + std::to_string(this->cc->player->getScore());
    this->drawCenteredString(scoreMsg.c_str(), this->width / 2, 100, 16777215);
    Screen::render(xMouse, yMouse);
}