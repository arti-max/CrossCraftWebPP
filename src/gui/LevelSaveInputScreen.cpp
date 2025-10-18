#include "gui/LevelSaveInputScreen.hpp"
#include "CrossCraft.hpp"

LevelSaveInputScreen::LevelSaveInputScreen(Screen* parent, std::string currentName, int levelId) : Screen::Screen() {
    this->parent = parent;
    this->levelId = levelId;
    this->levelName = std::move(currentName);

    if (this->levelName == "-") {
        this->levelName = "";
    }
}

void LevelSaveInputScreen::init(CrossCraft* cc, int width, int height) {
    Screen::init(cc, width, height);
    this->buttons.clear();
    Keyboard::enableRepeatEvents(true);
    this->buttons.push_back(new Button(0, this->width / 2 - 100, this->height / 4 + 120, 200, 20, "Save"));
    this->buttons.push_back(new Button(1, this->width / 2 - 100, this->height / 4 + 144, 200, 20, "Cancel"));

    this->buttons[0]->enabled = this->levelName.length() > 1;
}

void LevelSaveInputScreen::onClose() {
    Keyboard::enableRepeatEvents(false);
}

void LevelSaveInputScreen::tick() {
    this->tickCounter++;
}

void LevelSaveInputScreen::buttonClicked(Button* btn) {
    if (btn->enabled) {
        if (btn->id == 0 && this->levelName.length() > 1) {
            this->cc->saveLevel(this->levelId, this->levelName.c_str());
            this->cc->grabMouse();
            this->cc->setScreen(nullptr);
        }

        if (btn->id == 1) {
            this->cc->setScreen(this->parent);
        }
    }
}

void LevelSaveInputScreen::keyPressed(char c, int key) {
    if (key == GLFW_KEY_BACKSPACE && this->levelName.length() > 0) {
        this->levelName = this->levelName.substr(0, this->levelName.length() - 1);
    }

    if (c >= 32 && c <= 126 && this->levelName.length() < 64) {
        this->levelName += c;
    }

    this->buttons[0]->enabled = this->levelName.length() > 1;
}

void LevelSaveInputScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 0x60050500, 0xA0303060);
    this->drawCenteredString(this->title.c_str(), this->width / 2, 40, 0xFFFFFFFF);

    int fieldX = this->width / 2 - 100;
    int fieldY = this->height / 2 - 10;

    this->fill(fieldX - 1, fieldY - 1, fieldX + 200 + 1, fieldY + 20 + 1, 0xFFA0A0A0);
    this->fill(fieldX, fieldY, fieldX + 200, fieldY + 20, 0xFF000000);

    this->drawString((this->levelName + (this->tickCounter / 6 % 2 == 0 ? "_" : "")).c_str(), fieldX + 4, fieldY + 6, 0x00E0E0E0);
    Screen::render(xMouse, yMouse);
}