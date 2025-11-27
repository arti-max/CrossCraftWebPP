#pragma once
#include "character/Vertex.hpp"
#include "render/Tessellator.hpp"
#include <vector>

class Polygon {
public:
    std::vector<Vertex> vertices;
    
    Polygon(const std::vector<Vertex>& verts) : vertices(verts) {}
    
    Polygon(const std::vector<Vertex>& verts, int u0, int v0, int u1, int v1) {
        this->vertices = verts;
        
        if (this->vertices.size() >= 4) {
            this->vertices[0] = this->vertices[0].remap((float)u1, (float)v0);
            this->vertices[1] = this->vertices[1].remap((float)u0, (float)v0);
            this->vertices[2] = this->vertices[2].remap((float)u0, (float)v1);
            this->vertices[3] = this->vertices[3].remap((float)u1, (float)v1);
        }
    }

    void render(Tessellator& t) {
        for (int i = 3; i >= 0; --i) {
            const Vertex& v = vertices[i];
            t.texture(v.u / 64.0f, v.v / 32.0f);
            t.vertex(v.pos.x, v.pos.y, v.pos.z);
        }
    }
};
