#include "gui/pause/settings/SettingsScreen.hpp"
#include "gui/pause/settings/ControlsScreen.hpp"
#include "CrossCraft.hpp"

SettingsScreen::SettingsScreen(Screen* parent, Settings* settings) {
    this->parent = parent;
    this->settings = settings;
}

void SettingsScreen::init() {
    for (int i = 0; i < this->settings->settingsCount; ++i) {
        this->buttons.push_back(new Button(i, this->width / 2 - 155 + i % 2 * 160, this->height / 6 + 24 * (i >> 1), 150, 20, this->settings->getSettingStateStr(i)));
    }

    this->buttons.push_back(new Button(100, this->width / 2 - 100, this->height / 6 + 120 + 12, 200, 20, "Controls..."));
    this->buttons.push_back(new Button(200, this->width / 2 - 100, this->height / 6 + 168, 200, 20, "Done"));
}

void SettingsScreen::buttonClicked(Button* btn) {
    if (btn->enabled) {
        if (btn->id < this->settings->settingsCount) {
            this->settings->toggleSetting(btn->id, 1);
            btn->msg = this->settings->getSettingStateStr(btn->id);
        }

        if (btn->id == 200) {
            this->cc->setScreen(this->parent);
        }

        if (btn->id == 100) {
            this->cc->setScreen(new ControlsScreen(this, this->settings));
        }
    }
}

void SettingsScreen::render(int xMouse, int yMouse) {
    this->fillGradient(0, 0, this->width, this->height, 1610941696, -1607454624);
    this->drawCenteredString(this->title.c_str(), this->width / 2, 20, 16777215);
    Screen::render(xMouse, yMouse);
}