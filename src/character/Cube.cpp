#include "character/Cube.hpp"
#include "render/Tessellator.hpp"
#include <GL/gl.h>
#include <cmath>
#include <iostream>

Cube::Cube(int xTexOffs, int yTexOffs) {
    this->xTexOffs = xTexOffs;
    this->yTexOffs = yTexOffs;
}

Cube::~Cube() {
    for (Vertex* vertex : vertices) {
        if (vertex) {
            delete vertex->pos;
            delete vertex;
        }
    }
    
    for (Polygon* polygon : polygons) {
        if (polygon) {
            delete polygon;
        }
    }
    
    if (compiled && list != 0) {
        glDeleteLists(list, 1);
    }
}

void Cube::setTexOffs(int xTexOffs, int yTexOffs) {
    this->xTexOffs = xTexOffs;
    this->yTexOffs = yTexOffs;
}

void Cube::addBox(float x0, float y0, float z0, int w, int h, int d) {
    
    for (Vertex* vertex : vertices) {
        if (vertex) {
            delete vertex->pos;
            delete vertex;
        }
    }
    for (Polygon* polygon : polygons) {
        if (polygon) {
            delete polygon;
        }
    }
    
    vertices.clear();
    polygons.clear();
    
    vertices.reserve(8);
    polygons.reserve(6);
    
    float x1 = x0 + static_cast<float>(w);
    float y1 = y0 + static_cast<float>(h);
    float z1 = z0 + static_cast<float>(d);

    Vertex* u0 = new Vertex(x0, y0, z0, 0.0f, 0.0f);
    Vertex* u1 = new Vertex(x1, y0, z0, 0.0f, 8.0f);
    Vertex* u2 = new Vertex(x1, y1, z0, 8.0f, 8.0f);
    Vertex* u3 = new Vertex(x0, y1, z0, 8.0f, 0.0f);
    Vertex* l0 = new Vertex(x0, y0, z1, 0.0f, 0.0f);
    Vertex* l1 = new Vertex(x1, y0, z1, 0.0f, 8.0f);
    Vertex* l2 = new Vertex(x1, y1, z1, 8.0f, 8.0f);
    Vertex* l3 = new Vertex(x0, y1, z1, 8.0f, 0.0f);

    vertices.push_back(u0);
    vertices.push_back(u1);
    vertices.push_back(u2);
    vertices.push_back(u3);
    vertices.push_back(l0);
    vertices.push_back(l1);
    vertices.push_back(l2);
    vertices.push_back(l3);

    int face0_u0 = xTexOffs + d + w, face0_v0 = yTexOffs + d;
    int face0_u1 = xTexOffs + d + w + d, face0_v1 = yTexOffs + d + h;
    polygons.push_back(new Polygon({l1, u1, u2, l2}, face0_u0, face0_v0, face0_u1, face0_v1));
    
    int face1_u0 = xTexOffs + 0, face1_v0 = yTexOffs + d;
    int face1_u1 = xTexOffs + d, face1_v1 = yTexOffs + d + h;
    polygons.push_back(new Polygon({u0, l0, l3, u3}, face1_u0, face1_v0, face1_u1, face1_v1));
    
    int face2_u0 = xTexOffs + d, face2_v0 = yTexOffs + 0;
    int face2_u1 = xTexOffs + d + w, face2_v1 = yTexOffs + d;
    polygons.push_back(new Polygon({l1, l0, u0, u1}, face2_u0, face2_v0, face2_u1, face2_v1));
    
    int face3_u0 = xTexOffs + d + w, face3_v0 = yTexOffs + 0;
    int face3_u1 = xTexOffs + d + w + w, face3_v1 = yTexOffs + d;
    polygons.push_back(new Polygon({u2, u3, l3, l2}, face3_u0, face3_v0, face3_u1, face3_v1));
    
    int face4_u0 = xTexOffs + d, face4_v0 = yTexOffs + d;
    int face4_u1 = xTexOffs + d + w, face4_v1 = yTexOffs + d + h;
    polygons.push_back(new Polygon({u1, u0, u3, u2}, face4_u0, face4_v0, face4_u1, face4_v1));
    
    int face5_u0 = xTexOffs + d + w + d, face5_v0 = yTexOffs + d;
    int face5_u1 = xTexOffs + d + w + d + w, face5_v1 = yTexOffs + d + h;
    polygons.push_back(new Polygon({l0, l1, l2, l3}, face5_u0, face5_v0, face5_u1, face5_v1));

    if (compiled) {
        glDeleteLists(list, 1);
        compiled = false;
        list = 0;
    }
}

void Cube::setPos(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

void Cube::render() {
    if (!compiled) {
        compile();
    }
    
    const float radToDeg = 180.0f / M_PI;
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(zRot * radToDeg, 0.0f, 0.0f, 1.0f);
    glRotatef(yRot * radToDeg, 0.0f, 1.0f, 0.0f);
    glRotatef(xRot * radToDeg, 1.0f, 0.0f, 0.0f);
    glCallList(list);
    glPopMatrix();
}

void Cube::compile() {
    if (polygons.empty()) {
        printf("ERROR: No polygons to compile!\n");
        return;
    }
    
    list = glGenLists(1);
    if (list == 0) {
        printf("ERROR: Failed to generate display list!\n");
        return;
    }
    
    glNewList(list, GL_COMPILE);
    Tessellator& t = Tessellator::getInstance();
    t.begin();
    
    for (size_t i = 0; i < polygons.size(); ++i) {
        if (polygons[i]) {
            polygons[i]->render(t);
        }
    }
    
    t.end();
    glEndList();
    compiled = true;
}
