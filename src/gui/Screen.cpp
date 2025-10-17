#include "gui/Screen.hpp"
#include "CrossCraft.hpp"

const int GRAY_1 = -8355680;
const int GRAY_2 = -9408400;
const int BLACK = -16777216;
const int BLACK_GRAY = -6250336;
const int WHITE = 14737632;
const int BLUE_GRAY = -8355680;
const int LIGHT_YELLOW = 16777120;


void Screen::render(int xMouse, int yMouse) {
    for (int i = 0; i < this->buttons.size(); ++i) {
        Button* btn = this->buttons[i];
        if (!btn->visible) continue;
        this->fill(btn->x - 1, btn->y - 1, btn->x + btn->w + 1, btn->y + btn->h + 1, BLACK);
        if (!btn->enabled) {
            this->fill(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h, GRAY_1);
            this->drawCenteredString(btn->msg.c_str(), btn->x + btn->w / 2, btn->y + (btn->h - 8) / 2, BLACK_GRAY);
        } else if (xMouse >= btn->x && yMouse >= btn->y && xMouse < btn->x + btn->w && yMouse < btn->y + btn->h) {
            this->fill(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h, BLUE_GRAY);
            this->drawCenteredString(btn->msg.c_str(), btn->x + btn->w / 2, btn->y + (btn->h - 8) / 2, LIGHT_YELLOW);
        } else {
            this->fill(btn->x, btn->y, btn->x + btn->w, btn->y + btn->h, GRAY_2);
            this->drawCenteredString(btn->msg.c_str(), btn->x + btn->w / 2, btn->y + (btn->h - 8) / 2, WHITE);
        }
    }
}

void Screen::onClose() {
    // no implementation
}

void Screen::init(CrossCraft* cc, int width, int height) {
    this->cc = cc;
    this->width = width;
    this->height = height;
    this->buttons.clear();
    this->init();
}

void Screen::init() {
    // no implementation
}

void Screen::fill(int x0, int y0, int x1, int y1, int col) {
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

void Screen::fillGradient(int x0, int y0, int x1, int y1, int col1, int col2) {
    float a1 = (col1 >> 24 & 255) / 255.0f;
    float r1 = (col1 >> 16 & 255) / 255.0f;
    float g1 = (col1 >> 8 & 255) / 255.0f;
    float b1 = (col1 & 255) / 255.0f;
    float a2 = (col2 >> 24 & 255) / 255.0f;
    float r2 = (col2 >> 16 & 255) / 255.0f;
    float g2 = (col2 >> 8 & 255) / 255.0f;
    float b2 = (col2 & 255) / 255.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(r1, g1, b1, a1);
    glVertex2f((float)x1, (float)y0);
    glVertex2f((float)x0, (float)y0);
    glColor4f(r2, g2, b2, a2);
    glVertex2f((float)x0, (float)y1);
    glVertex2f((float)x1, (float)y1);
    glEnd();
    glFinish();
    glDisable(GL_BLEND);
}

void Screen::drawCenteredString(const char str[], int x, int y, int color) {
    Font* font = this->cc->font;
    font->drawShadow(str, x - font->width(str) / 2, y, color);
}

void Screen::drawString(const char str[], int x, int y, int color) {
    Font* font = this->cc->font;
    font->drawShadow(str, x, y, color);
}

void Screen::mouseClicked(int x, int y, int mbtn) {
    if (mbtn == 0) {
        for (int i = 0; i < this->buttons.size(); ++i) {
            Button* btn = this->buttons[i];
            if (btn->enabled && btn->visible && x >= btn->x && y >= btn->y && x < btn->x + btn->w && y < btn->y + btn->h) {
                this->buttonClicked(btn);
            }
        }
    }
}

void Screen::buttonClicked(Button* btn) {
    // no implementation
}

void Screen::updateEvents() {
    while (Mouse::next()) {
        if (Mouse::getEventButtonState()) {
            float xm = Mouse::getEventX() * this->width / this->cc->width;
            float ym = Mouse::getEventY() * this->height / this->cc->height;
            this->mouseClicked(xm, ym, Mouse::getEventButton());
        }
    }

    while (Keyboard::next()) {
        if (Keyboard::getEventKeyState()) {
            this->keyPressed(Keyboard::getEventCharacter(), Keyboard::getEventKey());
        }
    }
}

void Screen::keyPressed(char eventCharacter, int eventKey) {
    // no implementation
}

void Screen::tick() {
    // no implementation
}