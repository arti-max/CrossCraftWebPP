#include "gui/pause/settings/ControlsScreen.hpp"
#include "CrossCraft.hpp"

ControlsScreen::ControlsScreen(Screen* parent, Settings* settings) {
    this->parent = parent;
    this->settings = settings;
}

void ControlsScreen::init() {
    for (int i = 0; i < this->settings->keys.size(); ++i) {
        this->buttons.push_back(new Button(i, this->width / 2 - 155 + i % 2 * 160, this->height / 6 + 24 * (i >> 1), 150, 20, this->settings->getKeyName(i)));
    }

    this->buttons.push_back(new Button(200, this->width / 2 - 100, this->height / 6 + 168, 200, 20, "Done"));
}

void ControlsScreen::buttonClicked(Button* btn) {
    for (int i = 0; i < this->settings->keys.size(); ++i) {
        this->buttons[i]->msg = this->settings->getKeyName(i);
    }

    if (btn->id == 200) {
        this->cc->setScreen(this->parent);
    } else {
        this->key = btn->id;
        btn->msg = "> " + this->settings->getKeyName(btn->id) + " <";
    }
}

void ControlsScreen::keyPressed(char eventCharacter, int eventKey) {
    if (this->key >= 0) {
        this->settings->setKey(this->key, eventKey);
        this->buttons[this->key]->msg = this->settings->getKeyName(this->key);
        this->key = -1;
    } else {
        Screen::keyPressed(eventCharacter, eventKey);
    }
}

void ControlsScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 1610941696, -1607454624);
    this->drawCenteredString(this->title.c_str(), this->width / 2, 20, 16777215);
    Screen::render(xMouse, yMouse);
}