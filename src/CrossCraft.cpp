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
}

CrossCraft::~CrossCraft() {
    this->destroy();
}

void CrossCraft::destroy() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
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
    if (pause) {
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

void CrossCraft::render(float partialTicks) {

    glViewport(0, 0, this->width, this->height);
    this->checkGlError("Set viewport");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->setupCamera();

    glFinish();
    glfwSwapBuffers(window);
}

void CrossCraft::setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, this->width/this->height, 0.05, 1024.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -2.0f);
}

void moveCameraToPlayer(float partialTicks) {
    glTranslatef(0.0f, 0.0f, -0.3f);
}

void CrossCraft::checkGlError(const char str[]) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        const GLubyte* errorString = gluErrorString(errorCode);
        printf("########## GL ERROR ##########\n");
        printf("@ %s\n", errorString);
        printf("%i: %s", errorCode, errorString);
        glfwTerminate();
    }
}