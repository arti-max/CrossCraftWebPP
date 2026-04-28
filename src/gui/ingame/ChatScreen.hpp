#pragma once
#include "gui/Screen.hpp"
#include <string>

class ChatScreen : public Screen {
private:
    std::string message;
    int tickCount = 0;
    int inputDelay = 4;

public:
    ChatScreen();

    void init() override;
    void render(int xMouse, int yMouse) override;
    void keyPressed(char eventCharacter, int eventKey) override;
    void tick() override;
    void onClose() override;
    void addText(std::string txt);
};
