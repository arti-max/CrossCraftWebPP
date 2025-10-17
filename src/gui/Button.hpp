#pragma once
#include <string>

class Button {
public:
    int id;
    int x;
    int y; 
    int w;
    int h;
    std::string msg;
    bool visible = true;
    bool enabled = true;

    Button(int id, int x, int y, int w, int h, std::string msg) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->msg = std::move(msg);
    }
};