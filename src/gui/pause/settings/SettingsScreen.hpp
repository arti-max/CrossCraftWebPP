#pragma once
#include "gui/Screen.hpp"
#include "gui/Button.hpp"
#include "Settings.hpp"
#include <string>

class SettingsScreen : public Screen {
private:
    Screen* parent;
    std::string title = "Options";
    Settings* settings;

protected:
    void buttonClicked(Button* btn) override;
public:
    SettingsScreen(Screen* parent, Settings* settings);
    void render(int xMouse, int yMouse) override;
    void init() override;
};