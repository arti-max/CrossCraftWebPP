#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <array>

class CrossCraft;

class Renderer {
private:
    CrossCraft* cc;
public:
    Renderer(CrossCraft* cc);

    void setLighting(bool enable);
    

    inline const float* getBuffer(float a, float b, float c, float d) {
        static float buf[4];
        buf[0] = a; buf[1] = b; buf[2] = c; buf[3] = d;
        return buf;
    }
};