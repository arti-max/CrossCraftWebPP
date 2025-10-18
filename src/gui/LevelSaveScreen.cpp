#include "gui/LevelSaveScreen.hpp"
#include "CrossCraft.hpp"

LevelSaveScreen::LevelSaveScreen(Screen* parent) : LevelLoadScreen(parent) {
    this->title = "Save level";
}

void LevelSaveScreen::updateButtonLabels(std::vector<std::string> names) {
    for (int i = 0; i < 5; ++i) {
        Button* btn = this->buttons[i];
        btn->msg = names[i];
        btn->visible = true;
    }
}

void LevelSaveScreen::loadLevel(int levelId) {
    this->cc->setScreen(new LevelSaveInputScreen(this, this->buttons[levelId]->msg, levelId));
}