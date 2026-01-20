#pragma once
#include "character/Polygon.hpp"
#include <vector>

class Cube {
private:
    std::vector<Polygon> polygons; 
    
    int xTexOffs;
    int yTexOffs;
    bool compiled = false;
    GLuint list = 0;
    
    void compile();

public:
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float xRot = 0.0f, yRot = 0.0f, zRot = 0.0f;

    Cube(int xTexOffs, int yTexOffs);
    ~Cube();
    
    void addBox(float x0, float y0, float z0, int w, int h, int d);
    void setPos(float x, float y, float z);
    void render(float scale);
};
