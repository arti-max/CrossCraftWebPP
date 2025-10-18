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
    Logger::logf(PREFIX_CC, "CrossCraft destroyed.\n");
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
        Logger::logf(PREFIX_ERROR, "Failed to initialize GLFW\n");
    }

    const char* canvas = parent.empty() ? nullptr : parent.c_str();

    window = glfwCreateWindow(width, height, "CrossCraft", NULL, NULL);
    if (!window) {
        Logger::logf(PREFIX_ERROR, "Failed to create GLFW window\n");
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
    glAlphaFunc(GL_GREATER, 0.0f);
    glCullFace(GL_BACK);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    this->checkGlError("Startup");
    this->font = new Font("default", this->textures);
    glViewport(0, 0, this->width, this->height);
    this->level = new Level(256, 256, 64);
    this->particleEngine = new ParticleEngine(this->level);
    bool success = false;

    if (!success) {
        this->levelGen->generateLevel(this->level, this->user->username.c_str(), 256, 256, 64);
        success = true;
    }

    this->levelRenderer = new LevelRenderer(this->level, this->textures);
    this->player = new Player(this->level);

    for (int i = 0; i < 10; ++i) {
        Zombie* zombie = new Zombie(this->level, this->textures, 128.0f, 0.0f, 128.0f);
        zombie->resetPos();
        this->entities.push_back(zombie);
    }

    Mouse::init(window);
    Keyboard::init(window);

    Keyboard::enableRepeatEvents(false);

    this->checkGlError("Post startup");
}

void CrossCraft::setScreen(Screen* screen) {
    if (this->screen != nullptr) this->screen->onClose();
    this->screen = screen;
    if (screen != nullptr) {
        int screenWidth = this->width * 240 / this->height;
        int screenHeight = this->height * 240 / this->height;
        screen->init(this, screenWidth, screenHeight);
    }
}

void CrossCraft::grabMouse() {
    if (!this->mouseGrabbed) {
        Logger::logf(PREFIX_DEBUG, "CrossCraft: Grabbing mouse\n");
        this->mouseGrabbed = true;
        Mouse::setGrabbed(true);
        if (!this->appletMode) {
            Mouse::setCursorPosition(width / 2, height / 2);
        }
        this->setScreen(nullptr);
        Logger::logf(PREFIX_DEBUG, "Mouse grabbed successfully\n");
    }
}

void CrossCraft::releaseMouse() {
    if (this->mouseGrabbed) {
        Logger::logf(PREFIX_DEBUG, "CrossCraft: Releasing mouse\n");
        this->player->releaseAllKeys();
        this->mouseGrabbed = false;
        this->setScreen((Screen*)(new PauseScreen()));
        Mouse::setGrabbed(false);
        Logger::logf(PREFIX_DEBUG, "Mouse released successfully\n");
    }
}

void CrossCraft::stop() {
    this->running = false;
    emscripten_cancel_main_loop();
}

void CrossCraft::run() {
    Logger::logf(PREFIX_CC, "CrossCraft runned! %s, %i, %i, %s, %s\n", this->parent.c_str(), this->width, this->height, this->user->username.c_str(), this->user->sessionid.c_str());
    try {
        this->init();
    } catch (const std::exception& e) {
        Logger::logf(PREFIX_ERROR, "Failed to start CrossCraft: %s\n", e.what());
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
        Logger::logf(PREFIX_DEBUG, "%s\n", this->fpsString.c_str());
        Chunk::updates = 0;
        this->frames = 0;
        this->lastFpsTime += 1000.0;
    }
}

void CrossCraft::emscriptenMainLoop(void* arg) {
    static_cast<CrossCraft*>(arg)->mainLoop();
}

void CrossCraft::handleMouseClick() {
    if (this->editMode == 0) {
        if (this->hitResult != nullptr) {
            Tile* previousTile = Tile::tiles[this->level->getTile(this->hitResult->x, this->hitResult->y, this->hitResult->z)];

            bool tileChanged = this->level->setTile(this->hitResult->x, this->hitResult->y, this->hitResult->z, 0);
            if (previousTile != nullptr && tileChanged) {
                previousTile->onDestroy(this->level, this->hitResult->x, this->hitResult->y, this->hitResult->z, this->particleEngine);
            }
        }
    } else if (this->hitResult != nullptr) {
        int x = this->hitResult->x;
        int y = this->hitResult->y;
        int z = this->hitResult->z;

        if (this->hitResult->f == 0) y--;
        if (this->hitResult->f == 1) y++;
        if (this->hitResult->f == 2) z--;
        if (this->hitResult->f == 3) z++;
        if (this->hitResult->f == 4) x--;
        if (this->hitResult->f == 5) x++;
        Tile* prevTile = Tile::tiles[this->level->getTile(x, y, z)];
        if (prevTile == Tile::empty) {
            this->level->setTile(x, y, z, this->selectedTile);
        }
    }
}

void CrossCraft::tick() {
    if (this->mouseGrabbed && !Mouse::isGrabbed()) {
        printf("CrossCraft: Pointer lock released by browser (probably ESC)\n");
        this->releaseMouse();
    }

    if (this->screen != nullptr) {
        this->screen->updateEvents();
        if (this->screen != nullptr) {
            this->screen->tick();
        }
        
        while (Mouse::next()) {}
        while (Keyboard::next()) {}
        
        goto update_world;
    }

    if (this->screen == nullptr) {
        if (Keyboard::next()) {
            this->player->setKey();
            if (Keyboard::getEventKeyState()) {
                if (Keyboard::getEventKey() == GLFW_KEY_ESCAPE) {
                    this->releaseMouse();
                }
                if (Keyboard::getEventKey() == GLFW_KEY_R) {
                    this->player->resetPos();
                }
                if (Keyboard::getEventKey() == GLFW_KEY_1) {
                    this->selectedTile = Tile::rock->id;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_2) {
                    this->selectedTile = Tile::dirt->id;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_3) {
                    this->selectedTile = Tile::cobblestone->id;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_4) {
                    this->selectedTile = Tile::wood->id;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_6) {
                    this->selectedTile = Tile::bush->id;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_F) {
                    this->levelRenderer->toggleDrawDistance();
                }
                if (Keyboard::getEventKey() == GLFW_KEY_Y) {
                    this->yMouseAxis *= -1;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_G) {
                    this->entities.push_back(new Zombie(this->level, this->textures, this->player->x, this->player->y, this->player->z));
                }
            }
        }
        while (Mouse::next()) {
            if (!this->mouseGrabbed && Mouse::getEventButtonState()) {
                this->grabMouse();
            } else {
                if (Mouse::getEventButton() == 0 && Mouse::getEventButtonState()) {
                    this->handleMouseClick();
                }
                if (Mouse::getEventButton() == 2 && Mouse::getEventButtonState()) {
                    this->editMode = (this->editMode + 1) % 2;
                }
            }
        }
    }
update_world:
    this->level->tick();
    this->particleEngine->tick();

    for (int i = static_cast<int>(this->entities.size()) - 1; i >= 0; --i) {
        this->entities[i]->tick();
        
        if (this->entities[i]->removed) {
            delete this->entities[i];
            this->entities.erase(this->entities.begin() + i);
        }
    }

    this->player->tick();
}

void CrossCraft::raycast(float partialTicks) {
    if (this->hitResult != nullptr) {
        delete this->hitResult;
        this->hitResult = nullptr;
    }
    
    Ray ray = Ray::fromPlayer(this->player);
    
    this->hitResult = ray.trace(this->level, 5.0f, &this->entities);
}

void CrossCraft::render(float partialTicks) {
    if (this->mouseGrabbed) {
        glViewport(0, 0, this->width, this->height);
        float xo = Mouse::getDX();
        float yo = Mouse::getDY();
        this->player->turn(xo, yo * static_cast<float>(this->yMouseAxis));
    }
    
    this->checkGlError("Set viewport");
    this->raycast(partialTicks);
    this->checkGlError("Rasycasted");
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
    int i;
    Entity* zombie;
    for (i = 0; i < this->entities.size(); ++i) {
        zombie = this->entities[i];
        if (zombie->isLit() && frustum.isVisible(zombie->bb)) {
            this->entities[i]->render(partialTicks);
        }
    }
    this->checkGlError("Rendered entities");
    this->particleEngine->render(this->player, partialTicks, 0, this->textures);
    this->checkGlError("Rendered particles (0)");
    this->setupFog(1);
    this->levelRenderer->render(this->player, 1);
    for (i = 0; i < this->entities.size(); ++i) {
        zombie = this->entities[i];
        if (!zombie->isLit() && frustum.isVisible(zombie->bb)) {
            this->entities[i]->render(partialTicks);
        }
    }
    this->checkGlError("Render entities");
    this->particleEngine->render(this->player, partialTicks, 1, this->textures);
    this->checkGlError("Rendered particles (1)");
    this->levelRenderer->renderSurroundingGround();
    this->checkGlError("Render surrounding Ground");
    if (this->hitResult != nullptr) {
        glDisable(GL_LIGHTING);
        glDisable(GL_ALPHA_TEST);
        this->levelRenderer->renderHit(this->hitResult, this->player, this->editMode, this->selectedTile);
        this->levelRenderer->renderHitOutline(this->hitResult, this->player, this->editMode, this->selectedTile);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_LIGHTING);
    }
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

    if (this->hitResult != nullptr) {
        glDepthFunc(GL_LESS);
        glDisable(GL_ALPHA_TEST);
        this->levelRenderer->renderHit(this->hitResult, this->player, this->editMode, this->selectedTile);
        this->levelRenderer->renderHitOutline(this->hitResult, this->player, this->editMode, this->selectedTile);
        glEnable(GL_ALPHA_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    this->drawGui(partialTicks);
    this->checkGlError("Rendered gui");
    glfwSwapBuffers(window);
}

void CrossCraft::drawGui(float partialTicks) {
    int screenWidth = this->width * 240 / this->height;
    int screenHeight = this->height * 240 / this->height;
    int xMouse = Mouse::getX() * screenWidth / this->width;
    int yMouse = Mouse::getY() * screenHeight / this->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, screenWidth, screenHeight, 0.0f, 100.0f, 300.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
    this->checkGlError("GUI: Init");
    glPushMatrix();
    glTranslatef(screenWidth - 16, 16.0f, -50.0f);
    Tessellator& t = Tessellator::getInstance();
    glScalef(16.0f, 16.0f, 16.0f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(45.0, 0.0f, 1.0f, 0.0f);
    glTranslatef(-1.5f, 0.5f, 0.5f);
    glScalef(-1.0f, -1.0f, -1.0f);
    GLuint id = this->textures->loadTexture("terrain", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable(GL_TEXTURE_2D);
    t.begin();
    Tile::tiles[this->selectedTile]->render(t, this->level, 0, -2, 0, 0);
    t.end();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    this->checkGlError("GUI: Draw selected");
    this->font->drawShadow(VERSION_STRING, 2, 2, 0xFFFFFF);
    this->font->drawShadow(this->fpsString, 2, 12, 0xFFFFFF);
    this->checkGlError("GUI: Draw text");
    int wc = screenWidth / 2;
    int hc = screenHeight / 2;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    t.begin();
    t.vertex((float)(wc + 1), (float)(hc - 4), 0.0F);
    t.vertex((float)(wc - 0), (float)(hc - 4), 0.0F);
    t.vertex((float)(wc - 0), (float)(hc + 5), 0.0F);
    t.vertex((float)(wc + 1), (float)(hc + 5), 0.0F);
    t.vertex((float)(wc + 5), (float)(hc - 0), 0.0F);
    t.vertex((float)(wc - 4), (float)(hc - 0), 0.0F);
    t.vertex((float)(wc - 4), (float)(hc + 1), 0.0F);
    t.vertex((float)(wc + 5), (float)(hc + 1), 0.0F);
    t.end();
    this->checkGlError("GUI: Draw crosshair");
    if (this->screen != nullptr) {
        this->screen->render(xMouse, yMouse);
    }
}

void CrossCraft::setupCamera(float partialTicks) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = static_cast<float>(this->width) / static_cast<float>(this->height);
    gluPerspective(70.0, aspectRatio, 0.05, 1024.0);
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
        this->stop();
    }
}

void CrossCraft::setupFog(int layer) {
    if (layer == 0) {
        glFogi(GL_FOG_MODE, GL_VIEWPORT_BIT);
        glFogf(GL_FOG_DENSITY, 0.001f);
        glFogfv(GL_FOG_COLOR, this->fogColor0.data());
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f));
    } else if (layer == 1) {
        glFogi(GL_FOG_MODE, GL_VIEWPORT_BIT);
        glFogf(GL_FOG_DENSITY, 0.01f);
        glFogfv(GL_FOG_COLOR, this->fogColor1.data());
        float br = 0.6f;
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(br, br, br, 1.0f));
    }

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glEnable(GL_LIGHTING);   
}

void CrossCraft::beginLevelLoading(const char title[]) {
    this->title = title;
    int screenWidth = this->width * 240 / this->height;
    int screenHeight = this->height * 240 / this->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (double)screenWidth, (double)screenHeight, 0.0f, 100.0f, 300.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
}

void CrossCraft::levelLoadUpdate(const char status[]) {
    int screenWidth = this->width * 240 / this->height;
    int screenHeight = this->height * 240 / this->height;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_TEXTURE_2D);
    int id = this->textures->loadTexture("dirt", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    t.begin();
    t.color(0.5f, 0.5f, 0.5f);
    float s = 32.0f;
    t.vertexUV(0.0f, (float)screenHeight, 0.0f, 0.0f, (float)screenHeight / s);
    t.vertexUV((float)screenWidth, (float)screenHeight, 0.0f, (float)screenWidth / s, (float)screenHeight / s);
    t.vertexUV((float)screenWidth, 0.0f, 0.0f, (float)screenWidth / s, 0.0f);
    t.vertexUV(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    t.end();
    glEnable(GL_TEXTURE_2D);
    this->font->drawShadow(this->title, (screenWidth - this->font->width(this->title)) / 2, screenHeight / 2 - 4 - 8, 0x00FFFFFF);
    this->font->drawShadow(status, (screenWidth - this->font->width(status)) / 2, screenHeight / 2 - 4 + 4, 0x00FFFFFF);
    glfwSwapBuffers(window);

    emscripten_sleep(100);
}

bool CrossCraft::loadLevel(const char username[], int levelid) {
    if (!this->levelIO->loadOnline(this->level, this->serverHost, username, levelid)) {
        return false;
    } else {
        if (this->player != nullptr) {
            this->player->resetPos();
        }

        return true;
    }
}

void CrossCraft::saveLevel(int levelId, const char levelname[]) {
    this->levelIO->saveOnline(this->level, this->serverHost, this->user->username, this->user->sessionid, levelname, levelId);
}

void CrossCraft::generateNewLevel(int width, int height, int depth) {
    this->levelGen->generateLevel(this->level, this->user->username.c_str(), width, height, depth);
    this->player->resetPos();
    for (int i = static_cast<int>(this->entities.size()) - 1; i >= 0; --i) {
        this->entities.erase(this->entities.begin() + i);
    }
}