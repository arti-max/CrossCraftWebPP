#include "gui/ChatScreen.hpp"
#include "CrossCraft.hpp"
#include "net/packet/ChatMessagePacket.hpp"
#include <GLFW/glfw3.h>

ChatScreen::ChatScreen() {
    this->message = "";
}

void ChatScreen::init() {
    Keyboard::enableRepeatEvents(true);
}

void ChatScreen::onClose() {
    Keyboard::enableRepeatEvents(false);
}

void ChatScreen::render(int xMouse, int yMouse) {
    this->fill(2, this->height - 14, this->width - 2, this->height - 2, -2147483648);

    std::string textToDraw = "> " + this->message;

    if ((this->tickCount / 6) % 2 == 0) {
        textToDraw += "_";
    }

    this->drawString(textToDraw.c_str(), 4, this->height - 12, 0xE0E0E0);
}

void ChatScreen::keyPressed(char eventCharacter, int eventKey) {
    if (eventKey == GLFW_KEY_ESCAPE) {
        this->cc->setScreen(nullptr);
        return;
    }

    if (eventKey == GLFW_KEY_ENTER) {
        if (!this->message.empty()) {
            ChatMessagePacket* packet = new ChatMessagePacket(this->message);
            this->cc->client->sendPacket(packet);
        }
        this->cc->setScreen(nullptr);
        return;
    }

    if (eventKey == GLFW_KEY_BACKSPACE && !this->message.empty()) {
        this->message.pop_back();
    }
    
    if (isprint(eventCharacter) && this->message.length() < 64) {
        this->message += eventCharacter;
    }
}

void ChatScreen::tick() {
    this->tickCount++;
}
