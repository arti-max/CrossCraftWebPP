#include "CrossCraft.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

CrossCraft::CrossCraft(const char* canvas, int w, int h, bool fs) : 
    width(w), 
    height(h), 
    fullscreen(fs), 
    window(nullptr),
    textures()
{
    if (canvas) {
        parent = canvas;
    }

    this->textures = new Textures();
}

CrossCraft::~CrossCraft() {
    this->destroy();
}

void CrossCraft::destroy() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
    Mouse::destroy();
    Keyboard::destroy();
    printf("CrossCraft destroyed.\n");
}

void CrossCraft::init() {
    int col1 = 920330;
    float fr = 0.5f;
    float fg = 0.8f;
    float fb = 1.0f;
    fogColor0 = {fr, fg, fb, 1.0};
    fogColor1 = {
        (float)(col1 >> 16 & 255) / 255.0f,
        (float)(col1 >> 8 & 255) / 255.0f,
        (float)(col1 & 255) / 255.0f,
        1.0
    };


    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
    }

    const char* canvas = parent.empty() ? nullptr : parent.c_str();

    window = glfwCreateWindow(width, height, "CrossCraft", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    Keyboard::create();
    Mouse::create();
    this->checkGlError("Pre startup");
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(fr, fg, fb, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0);
    glCullFace(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    this->checkGlError("Startup");

    glViewport(0, 0, this->width, this->height);
    this->level = new Level(256, 256, 64);
    this->levelRenderer = new LevelRenderer(this->level, this->textures);
    this->player = new Player(this->level);

    Mouse::init(window);
    Keyboard::init(window);

    this->checkGlError("Post startup");
}

void CrossCraft::stop() {
    this->running = false;
    emscripten_cancel_main_loop();
}

void CrossCraft::run() {
    printf("CrossCraft runned! %s, %i, %i\n", this->parent.c_str(), this->width, this->height);
    try {
        this->init();
    } catch (const std::exception& e) {
        printf("Failed to start CrossCraft: %s\n", e.what());
        return;
    }

    this->running = true;
    this->lastFpsTime = emscripten_get_now();
    emscripten_set_main_loop_arg(emscriptenMainLoop, this, 0, 1);
}

void CrossCraft::mainLoop() {
    if (paused) {
        return;
    }

    if (glfwWindowShouldClose(window)) {
        this->stop();
        return;
    }
    
    this->timer->advanceTime();

    for (int i = 0; i < this->timer->ticks; ++i) {
        this->tick();
    }

    this->checkGlError("Pre render");
    this->render(this->timer->partialTicks);
    this->checkGlError("Post render");
    ++this->frames;
    glfwPollEvents();

    double now = emscripten_get_now();
    if (now >= this->lastFpsTime + 1000.0) {
        this->fpsString = std::to_string(this->frames) + " fps, " + std::to_string(Chunk::updates) + " chunk updates";
        printf("%s\n", this->fpsString.c_str());
        Chunk::updates = 0;
        this->frames = 0;
        this->lastFpsTime += 1000.0;
    }
}

void CrossCraft::emscriptenMainLoop(void* arg) {
    static_cast<CrossCraft*>(arg)->mainLoop();
}

void CrossCraft::tick() {
    if (Keyboard::next()) {
        this->player->setKey(Keyboard::getEventKey(), Keyboard::getEventKeyState());
    }
    this->player->tick();
}

void CrossCraft::render(float partialTicks) {
    glViewport(0, 0, this->width, this->height);
    float xo = Mouse::getDX();
    float yo = Mouse::getDY();
    this->player->turn(xo, yo * static_cast<float>(this->yMouseAxis));
    
    this->checkGlError("Set viewport");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->setupCamera(partialTicks);
    glEnable(GL_CULL_FACE);
    Frustum& frustum = Frustum::getFrustum();
    this->levelRenderer->cull(frustum);
    this->levelRenderer->updateDirtyChunks(this->player);
    this->checkGlError("Update chunks");
    this->setupFog(0);
    glEnable(GL_FOG);
    this->levelRenderer->render(this->player, 0);
    this->checkGlError("Rendered level");
    this->setupFog(1);
    this->levelRenderer->render(this->player, 1);
    this->levelRenderer->renderSurroundingGround();
    this->checkGlError("Render surrounding Ground");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    this->setupFog(0);
    this->levelRenderer->renderSurroundingWater();
    this->checkGlError("Render surrounding Water");
    glEnable(GL_BLEND);
    glColorMask(false, false, false, false);
    this->levelRenderer->render(this->player, 2);
    glColorMask(true, true, true, true);
    this->levelRenderer->render(this->player, 2);
    this->checkGlError("Color Mask");
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    glfwSwapBuffers(window);
}

void CrossCraft::setupCamera(float partialTicks) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, this->width / this->height, 0.05, 1024.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    this->moveCameraToPlayer(partialTicks);
}

void CrossCraft::moveCameraToPlayer(float partialTicks) {
    glTranslatef(0.0f, 0.0f, -0.3f);
    glRotatef(this->player->xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(this->player->yRot, 0.0f, 1.0f, 0.0f);
    float x = this->player->xo + (this->player->x - this->player->xo) * partialTicks;
    float y = this->player->yo + (this->player->y - this->player->yo) * partialTicks;
    float z = this->player->zo + (this->player->z - this->player->zo) * partialTicks;
    glTranslatef(-x, -y, -z);
}

void CrossCraft::checkGlError(const char str[]) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        const GLubyte* errorString = gluErrorString(errorCode);
        printf("########## GL ERROR ##########\n");
        printf("%s\n", str);
        printf("@ %s\n", errorString);
        printf("%i: %s\n", errorCode, errorString);
        glfwTerminate();
    }
}

void CrossCraft::setupFog(int layer) {
    if (layer == 0) {
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogf(GL_FOG_DENSITY, 0.001f);
        glFogfv(GL_FOG_COLOR, this->fogColor0.data());
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f));
    } else if (layer == 1) {
        glFogi(GL_FOG_MODE, GL_EXP);
        glFogf(GL_FOG_DENSITY, 0.01f);
        glFogfv(GL_FOG_COLOR, this->fogColor1.data());
        float br = 0.6f;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(br, br, br, 1.0f));
    }
}
