#pragma once
#include "character/Vertex.hpp"
#include "character/Polygon.hpp"
#include <GL/gl.h>
#include <vector>

class Cube {
private:
    std::vector<Vertex*> vertices;
    std::vector<Polygon*> polygons;
    int xTexOffs;
    int yTexOffs;
    bool compiled = false;
    GLuint list = 0;
    
    void compile();

public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float xRot = 0.0f;
    float yRot = 0.0f;
    float zRot = 0.0f;

    Cube(int xTexOffs, int yTexOffs);
    ~Cube();
    
    void setTexOffs(int xTexOffs, int yTexOffs);
    void addBox(float x0, float y0, float z0, int w, int h, int d);
    void setPos(float x, float y, float z);
    void render();
};
