#include "render/Tessellator.hpp"
#include <stdio.h>

Tessellator& Tessellator::getInstance() {
    static Tessellator instance;
    return instance;
}

Tessellator::Tessellator() {
    vertices = 0;
    p = 0; len = 3;
    u = 0.0f; v = 0.0f;
    r = 0.0f; g = 0.0f; b = 0.0f;
    nx = 0.0f; ny = 0.0f; nz = 0.0f;
    hasColor = false;
    hasTexture = false;
    noColor = false;
    hasNormal = false;
    buffer.reserve(MAX_FLOATS);
}

void Tessellator::end() {
    // printf("FLUSH GEOMETRY %i, p: %i, len: %i\n", this->vertices, this->p, this->len);
    if (this->vertices == 0) {
        return;
    }

    int stride = this->len * sizeof(float);
    float* data = buffer.data();

    int offset = 0;

    if (hasTexture) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, stride, data + offset);
        offset += 2;
    } else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (hasColor) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, stride, data + offset);
        offset += 3;
    } else {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    if (hasNormal) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, stride, data + offset);
        offset += 3;
    } else {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, stride, data + offset);

    glDrawArrays(GL_QUADS, 0, this->vertices);
    glDisableClientState(GL_VERTEX_ARRAY);
    if (hasTexture) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (hasColor) {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    if (hasNormal)  {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    clear();
}

void Tessellator::begin() {
    clear();
    hasColor = false;
    hasTexture = false;
    noColor = false;
    hasNormal = false;
    len = 3;
}

void Tessellator::clear() {
    vertices = 0;
    buffer.clear();
    p = 0;
}

void Tessellator::texture(float u, float v) {
    if (!hasTexture) {
        this->len += 2;
    }
    this->hasTexture = true;
    this->u = u;
    this->v = v;
}

void Tessellator::color(float r, float g, float b) {
    if (!noColor) {
        if (!hasColor) {
            this->len += 3;
        }
        this->hasColor = true;
        this->r = r;
        this->g = g;
        this->b = b;
    }
}

void Tessellator::normal(float x, float y, float z) {
    if (!hasNormal) {
        this->len += 3;
    }
    this->hasNormal = true;
    this->nx = x;
    this->ny = y;
    this->nz = z;
}

void Tessellator::vertex(float x, float y, float z) {
    // printf("VERTEX %f, %f, %f\n", x, y, z);
    if (hasTexture) {
        buffer.push_back(u);
        buffer.push_back(v);
    }
    if (hasColor) {
        buffer.push_back(r);
        buffer.push_back(g);
        buffer.push_back(b);
    }
    if (hasNormal) {
        buffer.push_back(nx);
        buffer.push_back(ny);
        buffer.push_back(nz);
    }

    buffer.push_back(x);
    buffer.push_back(y);
    buffer.push_back(z);
    this->p += len;
    this->vertices++;

    if (vertices % 4 == 0 && p >= MAX_FLOATS - len * 4) {
        end();
    }
}

void Tessellator::vertexUV(float x, float y, float z, float u, float v) {
    this->texture(u, v);
    this->vertex(x, y, z);
}

void Tessellator::_noColor() {
    this->noColor = true;
}