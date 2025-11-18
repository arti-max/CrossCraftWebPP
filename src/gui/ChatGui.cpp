#include "gui/ChatGui.hpp"
#include "gui/Screen.hpp"

const int MESSAGE_LIFETIME = 200;

ChatGui::ChatGui() {}

void ChatGui::addMessage(const std::string& text) {
    messages.insert(messages.begin(), {text, MESSAGE_LIFETIME});

    if (messages.size() > 10) {
        messages.pop_back();
    }
}

void ChatGui::tick() {
    for (int i = 0; i < messages.size(); ++i) {
        messages[i].lifetime--;
    }

    messages.erase(
        std::remove_if(messages.begin(), messages.end(), 
            [](const ChatMessage& msg) {
                return msg.lifetime <= 0;
            }
        ),
        messages.end()
    );
}

void ChatGui::render(Font* font, int width, int height) {
    for (int i = 0; i < messages.size(); ++i) {
        const auto& msg = messages[i];
        
        float alpha = 1.0f;

        int yPos = height - 30 - (i * 10);
        // this->fill(2, yPos - 1, width - 2, yPos + 9, (int)(alpha * 128) << 24);

        font->drawShadow(msg.text, 2, yPos, 0xFFFFFF | (static_cast<int>(alpha * 255) << 24));
    }
}

void ChatGui::fill(int x0, int y0, int x1, int y1, int col) {
    float a = (col >> 24 & 255) / 255.0f;
    float r = (col >> 16 & 255) / 255.0f;
    float g = (col >> 8 & 255) / 255.0f;
    float b = (col & 255) / 255.0f;
    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    t.begin();
    t.vertex((float)x0, (float)y1, 0.0f);
    t.vertex((float)x1, (float)y1, 0.0f);
    t.vertex((float)x1, (float)y0, 0.0f);
    t.vertex((float)x0, (float)y0, 0.0f);
    t.end();
    glDisable(GL_BLEND);
}