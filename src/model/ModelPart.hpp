#pragma once
#include "model/Vec3D.hpp"
#include "model/Vertex.hpp"
#include "model/Quad.hpp"
#include "render/Tessellator.hpp"
#include <GL/gl.h>

class ModelPart {
private:
    int textureOffsetX = 0;
    int textureOffsetY = 0;
    
public:
    std::vector<Quad> quads;
    
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
    
    bool mirror = false;
    bool showModel = true;
    bool isHidden = false;
    
    bool compiled = false;
    int displayList = 0;

    ModelPart(int u, int v) {
        this->textureOffsetX = u;
        this->textureOffsetY = v;
    }

    void addBox(float x, float y, float z, int w, int h, int d, float expand = 0.0f) {
        this->quads.clear();

        float x1 = x + (float)w;
        float y1 = y + (float)h;
        float z1 = z + (float)d;

        x -= expand;
        y -= expand;
        z -= expand;
        x1 += expand;
        y1 += expand;
        z1 += expand;

        if (this->mirror) {
            float temp = x1;
            x1 = x;
            x = temp;
        }

        NewVertex v0(x, y, z, 0.0f, 0.0f);
        NewVertex v1(x1, y, z, 0.0f, 8.0f);
        NewVertex v2(x1, y1, z, 8.0f, 8.0f);
        NewVertex v3(x, y1, z, 8.0f, 0.0f);
        NewVertex v4(x, y, z1, 0.0f, 0.0f);
        NewVertex v5(x1, y, z1, 0.0f, 8.0f);
        NewVertex v6(x1, y1, z1, 8.0f, 8.0f);
        NewVertex v7(x, y1, z1, 8.0f, 0.0f);

        // Right
        quads.push_back(Quad({v5, v1, v2, v6}, 
            textureOffsetX + d + w, textureOffsetY + d, 
            textureOffsetX + d + w + d, textureOffsetY + d + h));

        // Left
        quads.push_back(Quad({v0, v4, v7, v3}, 
            textureOffsetX, textureOffsetY + d, 
            textureOffsetX + d, textureOffsetY + d + h));

        // Top
        quads.push_back(Quad({v5, v4, v0, v1}, 
            textureOffsetX + d, textureOffsetY, 
            textureOffsetX + d + w, textureOffsetY + d));

        // Bottom
        quads.push_back(Quad({v1, v0, v3, v2}, 
            textureOffsetX + d + w, textureOffsetY, 
            textureOffsetX + d + w + w, textureOffsetY + d));

        // Front
        quads.push_back(Quad({v1, v0, v3, v2}, 
            textureOffsetX + d, textureOffsetY + d, 
            textureOffsetX + d + w, textureOffsetY + d + h));   
        
        this->quads.clear();
        
        // Quad 0: 15, 11, 12, 21 -> v5, v1, v2, v6
        quads.push_back(Quad({v5, v1, v2, v6}, textureOffsetX + d + w, textureOffsetY + d, textureOffsetX + d + w + d, textureOffsetY + d + h));
        
        // Quad 1: 20, 13, 14, 18 -> v0, v4, v7, v3
        quads.push_back(Quad({v0, v4, v7, v3}, textureOffsetX, textureOffsetY + d, textureOffsetX + d, textureOffsetY + d + h));
        
        // Quad 2: 15, 13, 20, 11 -> v5, v4, v0, v1
        quads.push_back(Quad({v5, v4, v0, v1}, textureOffsetX + d, textureOffsetY, textureOffsetX + d + w, textureOffsetY + d));
        
        // Quad 3: 12, 18, 14, 21 -> v2, v3, v7, v6
        quads.push_back(Quad({v2, v3, v7, v6}, textureOffsetX + d + w, textureOffsetY, textureOffsetX + d + w + w, textureOffsetY + d));
        
        // Quad 4: 11, 20, 18, 12 -> v1, v0, v3, v2
        quads.push_back(Quad({v1, v0, v3, v2}, textureOffsetX + d, textureOffsetY + d, textureOffsetX + d + w, textureOffsetY + d + h));
        
        // Quad 5: 13, 15, 21, 14 -> v4, v5, v6, v7
        quads.push_back(Quad({v4, v5, v6, v7}, textureOffsetX + d + w + d, textureOffsetY + d, textureOffsetX + d + w + d + w, textureOffsetY + d + h));

        if (this->mirror) {
            for (auto& quad : this->quads) {
                std::vector<NewVertex> reversed;
                for (int i = quad.vertices.size() - 1; i >= 0; i--) {
                    reversed.push_back(quad.vertices[i]);
                }
                quad.vertices = reversed;
            }
        }
    }

    void setPosition(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void render(float scale) {
        if (!this->showModel) return;
        if (this->isHidden) return;

        if (!this->compiled) {
            compileDisplayList(scale);
        }
                
        if (pitch == 0.0f && yaw == 0.0f && roll == 0.0f) {
            if (x == 0.0f && y == 0.0f && z == 0.0f) {
                glCallList(displayList);
            } else {
                glTranslatef(x * scale, y * scale, z * scale);
                glCallList(displayList);
                glTranslatef(-x * scale, -y * scale, -z * scale);
            }
        } else {
            glPushMatrix();
            glTranslatef(x * scale, y * scale, z * scale);

            if (roll != 0.0f) glRotatef(roll * 57.295776f, 0.0f, 0.0f, 1.0f);
            if (yaw != 0.0f)  glRotatef(yaw * 57.295776f, 0.0f, 1.0f, 0.0f);
            if (pitch != 0.0f) glRotatef(pitch * 57.295776f, 1.0f, 0.0f, 0.0f);

            glCallList(displayList);
            glPopMatrix();
        }
    }


    void compileDisplayList(float scale) {

        displayList = glGenLists(1);
        glNewList(displayList, GL_COMPILE);

        glBegin(GL_TRIANGLES);

        for (size_t i = 0; i < quads.size(); ++i) {
            const auto& quad = quads[i];

            if (quad.vertices.size() != 4) continue;

            Vec3D v1 = quad.vertices[1].vector.subtract(quad.vertices[0].vector).normalize();
            Vec3D v2 = quad.vertices[1].vector.subtract(quad.vertices[2].vector).normalize();
            Vec3D normal = v1.cross(v2).normalize();

            if (std::isnan(normal.x)) normal = Vec3D(0.0f, 1.0f, 0.0f);

            int triIndices[6] = {0, 1, 2, 2, 3, 0};

            for (int k = 0; k < 6; k++) {
                int idx = triIndices[k];
                const auto& v = quad.vertices[idx];
                
                float vx = v.vector.x * scale;
                float vy = v.vector.y * scale;
                float vz = v.vector.z * scale;
                
                if (std::isnan(vx)) vx = 0.0f;
                if (std::isnan(vy)) vy = 0.0f;
                if (std::isnan(vz)) vz = 0.0f;
                
                glNormal3f(normal.x, normal.y, normal.z);
                glTexCoord2f(v.u, v.v);
                glVertex3f(vx, vy, vz);
            }
        }

        glEnd();
        glEndList();
        this->compiled = true;
    }
};