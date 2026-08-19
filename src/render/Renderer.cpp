#include "render/Renderer.hpp"
#include "CrossCraft.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <gc.h>
#include "model/Vec3D.hpp"

Renderer::Renderer(CrossCraft* cc) {
    this->cc = cc;
}

void Renderer::setLighting(bool enable) {
    if (!enable) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    } else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        Vec3D vec = Vec3D(0.0f, -1.0f, 0.5f).normalize();
        glLightfv(GL_LIGHT0, GL_POSITION, this->getBuffer(vec.x, vec.y, vec.z, 0.0f));
        glLightfv(GL_LIGHT0, GL_DIFFUSE, this->getBuffer(0.3f, 0.3f, 0.3f, 1.0f));
        glLightfv(GL_LIGHT0, GL_AMBIENT, this->getBuffer(0.0f, 0.0f, 0.0f, 1.0f));
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, this->getBuffer(0.7f, 0.7f, 0.7f, 1.0f));
    }
}