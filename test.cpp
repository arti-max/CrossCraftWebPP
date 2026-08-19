#include <GL/gl.h>


int main() {

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // CCW
    glBegin(GL_TRIANGLES);

    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glEnd();

    return 0;
}