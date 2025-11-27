#include "character/Cube.hpp"
#include <cmath>
#include <iostream>

Cube::Cube(int xTexOffs, int yTexOffs) : xTexOffs(xTexOffs), yTexOffs(yTexOffs) {}

Cube::~Cube() {
    if (compiled && list != 0) {
        glDeleteLists(list, 1);
    }
}

void Cube::setPos(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Cube::addBox(float x0, float y0, float z0, int w, int h, int d) {
    polygons.clear();
    
    float x1 = x0 + w;
    float y1 = y0 + h;
    float z1 = z0 + d;

    Vertex u0(x0, y0, z0, 0.0f, 0.0f);
    Vertex u1(x1, y0, z0, 0.0f, 8.0f);
    Vertex u2(x1, y1, z0, 8.0f, 8.0f);
    Vertex u3(x0, y1, z0, 8.0f, 0.0f);
    Vertex l0(x0, y0, z1, 0.0f, 0.0f);
    Vertex l1(x1, y0, z1, 0.0f, 8.0f);
    Vertex l2(x1, y1, z1, 8.0f, 8.0f);
    Vertex l3(x0, y1, z1, 8.0f, 0.0f);

    polygons.push_back(Polygon({l1, u1, u2, l2}, 
        xTexOffs + d + w, yTexOffs + d, 
        xTexOffs + d + w + d, yTexOffs + d + h));
        
    polygons.push_back(Polygon({u0, l0, l3, u3}, 
        xTexOffs, yTexOffs + d, 
        xTexOffs + d, yTexOffs + d + h));
        
    polygons.push_back(Polygon({l1, l0, u0, u1}, 
        xTexOffs + d, yTexOffs, 
        xTexOffs + d + w, yTexOffs + d));
        
    polygons.push_back(Polygon({u2, u3, l3, l2}, 
        xTexOffs + d + w, yTexOffs, 
        xTexOffs + d + w + w, yTexOffs + d));
        
    polygons.push_back(Polygon({u1, u0, u3, u2}, 
        xTexOffs + d, yTexOffs + d, 
        xTexOffs + d + w, yTexOffs + d + h));
        
    polygons.push_back(Polygon({l0, l1, l2, l3}, 
        xTexOffs + d + w + d, yTexOffs + d, 
        xTexOffs + d + w + d + w, yTexOffs + d + h));

    if (compiled) {
        compiled = false; 
    }
}

void Cube::render() {
    if (!compiled) {
        compile();
    }
    
    float radToDeg = 57.29578f;
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(zRot * radToDeg, 0.0f, 0.0f, 1.0f);
    glRotatef(yRot * radToDeg, 0.0f, 1.0f, 0.0f);
    glRotatef(xRot * radToDeg, 1.0f, 0.0f, 0.0f);
    glCallList(list);
    glPopMatrix();
}

void Cube::compile() {
    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    Tessellator& t = Tessellator::getInstance();
    t.begin();
    
    for (auto& poly : polygons) {
        poly.render(t);
    }
    
    t.end();
    glEndList();
    compiled = true;
}
