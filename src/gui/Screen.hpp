#pragma once
#include <GL/gl.h>
#include <vector>
#include "gui/Button.hpp"

class CrossCraft;

class Screen {
protected:
    CrossCraft* cc;
    int width;
    int height;
    float imgZOrder = 0.0f;
    std::vector<Button*> buttons;
    virtual void fill(int x0, int y0, int x1, int y1, int col);
    virtual void fillGradient(int x0, int y0, int x1, int y1, int col1, int col2);
    virtual void mouseClicked(int x, int y, int mbtn);
    virtual void buttonClicked(Button* btn);
    virtual void keyPressed(char eventCharacter, int eventKey);
public:
    bool grabMouse = false;
    virtual void render(int xMouse, int yMouse);
    virtual void onClose();
    virtual void init(CrossCraft* cc, int width, int height);
    virtual void init();
    virtual void drawCenteredString(const char str[], int x, int y, int color);
    virtual void drawString(const char str[], int x, int y, int color);
    void updateEvents();
    void updateMouseEvents();
    void updateKeyboardEvents();
    virtual void tick();
    virtual void drawImage(int x, int y, int u, int v, int w, int h);
};