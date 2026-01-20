#pragma once
#include <string>
#include <vector>
#include "gui/Font.hpp"

struct ChatMessage {
    std::string text;
    int lifetime;
};

class ChatGui {
private:
public:
    std::vector<ChatMessage> messages;

    ChatGui();

    void addMessage(const std::string& text);
    void tick();
    void render(Font* font, int width, int height);
    void fill(int x0, int y0, int x1, int y1, int col);
};
