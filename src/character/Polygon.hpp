#pragma once
#include "character/Vertex.hpp"
#include "character/Vec3.hpp"
#include "render/Tessellator.hpp"
#include <GL/gl.h>
#include <vector>
#include <iostream>

class Polygon {
public:
    std::vector<Vertex*> vertices;
    int vertexCount;
    
    Polygon(std::vector<Vertex*> vertices) {
        this->vertices = vertices;
        this->vertexCount = vertices.size();
    }
    
    Polygon(std::vector<Vertex*> vertices, int u0, int v0, int u1, int v1) : Polygon(vertices) {
        
        if (vertices.size() >= 4) {
            this->vertices[0] = vertices[0]->remap(static_cast<float>(u1), static_cast<float>(v0));
            this->vertices[1] = vertices[1]->remap(static_cast<float>(u0), static_cast<float>(v0));
            this->vertices[2] = vertices[2]->remap(static_cast<float>(u0), static_cast<float>(v1));
            this->vertices[3] = vertices[3]->remap(static_cast<float>(u1), static_cast<float>(v1));
        }
    }

    void render(Tessellator& t) {
        t.color(1.0f, 1.0f, 1.0f);

        for (int i = 3; i >= 0; --i) {
            if (i < vertices.size() && vertices[i] && vertices[i]->pos) {
                Vertex* v = vertices[i];
                float u = v->u / 64.0f;
                float v_coord = v->v / 32.0f;
                t.texture(u, v_coord);
                t.vertex(v->pos->x, v->pos->y, v->pos->z);
            }
        }
    }
};
