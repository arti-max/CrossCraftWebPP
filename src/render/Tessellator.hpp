#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

class Tessellator {
private:
    Tessellator();
    Tessellator(const Tessellator&) = delete;
    Tessellator& operator=(const Tessellator&) = delete;

    std::vector<float> buffer;
    int vertices;
    int p;
    int len;
    float u, v;
    float r, g, b;
    bool hasTexture;
    bool hasColor;
    bool noColor;
public:
    static const int MAX_FLOATS = 524288;

    static Tessellator& getInstance();

    void begin();
    void end();
    void clear();

    void vertex(float x, float y, float z);
    void texture(float u, float v);
    void color (float r, float g, float b);
    void vertexUV(float x, float y, float z, float u, float v);
    void _noColor();
};