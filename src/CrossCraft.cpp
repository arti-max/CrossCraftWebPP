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
    float fr = 0.92f;
    float fg = 0.98f;
    float fb = 1.0f;
    fogColor0 = {fr, fg, fb, 1.0};
    fogColor1 = {(float)14/255.0f, (float)11/255.0f, (float)10/255.0f, 1.0f};


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
    this->level = new Level();
    this->particleEngine = new ParticleEngine(this->level);

    if (!this->mpMode) {
        if (this->loadMapUser.empty() || this->loadMapId == -1) {
            if (this->user != nullptr) {
                this->levelGen->generateLevel(this->level, this->user->username.c_str(), 256, 256, 64);
            } else {
                this->levelGen->generateLevel(this->level, "noname", 256, 256, 64);
            }
        } else {
            this->loadLevel(this->loadMapUser.c_str(), this->loadMapId);
        }
    } else {
        this->level->isRemote = true;
        std::string wsUrl = "ws://" + this->serverAddress + ":" + std::to_string(this->serverPort);
        this->connectToServer(wsUrl);
    }

    this->levelRenderer = new LevelRenderer(this->level, this->textures);
    this->player = new Player(this->level);

    Mouse::init(window);
    Keyboard::init(window);

    Keyboard::enableRepeatEvents(false);

    this->hotbarSlots = {
        Tile::rock->id,
        Tile::cobblestone->id,
        Tile::dirt->id,
        Tile::wood->id,
        Tile::bush->id,
        Tile::log->id,
        Tile::leaves->id,
        Tile::sand->id,
        Tile::gravel->id,
    };
    this->hotbarIndex = 0;
    this->selectedTile = this->hotbarSlots[this->hotbarIndex];

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
    if (this->user != nullptr) {
        Logger::logf(PREFIX_CC, "CrossCraft started! canvas=%s, size=%dx%d, user=%s\n", 
            this->parent.c_str(), 
            this->width, 
            this->height, 
            this->user->username.c_str());
    } else {
        Logger::logf(PREFIX_CC, "CrossCraft started! canvas=%s, size=%dx%d, guest mode\n", 
            this->parent.c_str(), 
            this->width, 
            this->height);
    }
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
        // Logger::logf(PREFIX_DEBUG, "%s\n", this->fpsString.c_str());
        Chunk::updates = 0;
        this->frames = 0;
        this->lastFpsTime += 1000.0;
    }
}

void CrossCraft::emscriptenMainLoop(void* arg) {
    static_cast<CrossCraft*>(arg)->mainLoop();
}

bool CrossCraft::isFree(const AABB &aabb) {
    if (this->player->bb.intersects(aabb)) {
        return false;
    } else {
        for (int i = 0; i < this->level->entities.size(); ++i) {
            if (this->level->entities[i]->bb.intersects(aabb)) {
                return false;
            }
        }

        return true;
    }
}

void CrossCraft::handleMouseClick() {
    if (this->editMode == 0) {
        if (this->hitResult != nullptr) {
            Tile* previousTile = Tile::tiles[this->level->getTile(this->hitResult->x, this->hitResult->y, this->hitResult->z)];

            bool tileChanged = this->level->setTile(this->hitResult->x, this->hitResult->y, this->hitResult->z, 0);
            if (previousTile != nullptr && tileChanged) {
                if (this->mpMode && client && client->isConnected()) {
                    this->level->setTile(this->hitResult->x, this->hitResult->y, this->hitResult->z, previousTile->id);
                    BlockChangePacket* packet = new BlockChangePacket(
                        this->hitResult->x, this->hitResult->y, this->hitResult->z, 
                        0, false);
                    client->sendPacket(packet);
                } else {
                    previousTile->onDestroy(this->level, this->hitResult->x, this->hitResult->y, this->hitResult->z, this->particleEngine);
                }
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

        AABB* aabb = Tile::tiles[this->selectedTile]->getAABB(x, y, z);
        
        if (aabb == nullptr || this->isFree(*aabb)) {
            if (this->mpMode && client && client->isConnected()) {
                BlockChangePacket* packet = new BlockChangePacket(
                        x, y, z, 
                        this->selectedTile, true);
                client->sendPacket(packet);
            } else {
                this->level->setTile(x, y, z, this->selectedTile);
            }
        }
        
        if (aabb != nullptr) {
            delete aabb;
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
                if (Keyboard::getEventKey() == GLFW_KEY_ENTER) {
                    this->level->setSpawnPos((int)this->player->x, (int)this->player->y, (int)this->player->z, (int)this->player->yRot);
                    this->player->resetPos();
                }

                if (Keyboard::getEventKey() == GLFW_KEY_R) {
                    this->player->resetPos();
                }
                if (Keyboard::getEventKey() >= GLFW_KEY_1 && Keyboard::getEventKey() <= GLFW_KEY_9) {
                    int keyIndex = Keyboard::getEventKey() - GLFW_KEY_1;
                    if (keyIndex < this->hotbarSlots.size()) {
                        this->hotbarIndex = keyIndex;
                        this->selectedTile = this->hotbarSlots[this->hotbarIndex];
                    }
                }
                if (Keyboard::getEventKey() == GLFW_KEY_F) {
                    this->levelRenderer->toggleDrawDistance();
                }
                if (Keyboard::getEventKey() == GLFW_KEY_Y) {
                    this->yMouseAxis *= -1;
                }
                if (Keyboard::getEventKey() == GLFW_KEY_G && !this->mpMode && this->level->entities.size() < 256) {
                    this->level->entities.push_back(new Zombie(this->level, this->textures, this->player->x, this->player->y, this->player->z));
                }
            }
        }
        while (Mouse::next()) {
            double dWheel = Mouse::getEventDWheel();
            if (dWheel != 0.0) {
                int scrollDirection = (dWheel > 0) ? 1 : -1;
                this->hotbarIndex += scrollDirection;

                int numSlots = this->hotbarSlots.size();
                this->hotbarIndex = (this->hotbarIndex % numSlots + numSlots) % numSlots;
                this->selectedTile = this->hotbarSlots[this->hotbarIndex];
            }

            if (!this->mouseGrabbed && Mouse::getEventButtonState()) {
                this->grabMouse();
            } else {
                if (Mouse::getEventButton() == 0 && Mouse::getEventButtonState()) {
                    this->handleMouseClick();
                }
                if (Mouse::getEventButton() == 2 && Mouse::getEventButtonState()) {
                    this->editMode = (this->editMode + 1) % 2;
                }
                if (Mouse::getEventButton() == 1 && Mouse::getEventButtonState()) {
                    if (this->hitResult != nullptr) {
                        int pickedID = this->level->getTile(this->hitResult->x, this->hitResult->y, this->hitResult->z);
                        
                        if (pickedID == Tile::grass->id) {
                            pickedID = Tile::dirt->id;
                        }

                        if (pickedID > 0 && Tile::tiles[pickedID] != nullptr && Tile::tiles[pickedID]->mayPick()) {
                            for (int i = 0; i < this->hotbarSlots.size(); ++i) {
                                if (this->hotbarSlots[i] == pickedID) {
                                    this->hotbarIndex = i;
                                    this->selectedTile = this->hotbarSlots[this->hotbarIndex];
                                    Logger::logf(PREFIX_DEBUG, "Picked block: %d, hotbar slot: %d\n", pickedID, i + 1);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
update_world:
    ++this->levelRenderer->cloudTicks;
    this->level->tick();
    this->particleEngine->tick();

    this->player->tick();

    if (mpMode && client->isConnected()) {
        // TODO: Send position to server
    }
}

void CrossCraft::raycast(float partialTicks) {
    if (this->hitResult != nullptr) {
        delete this->hitResult;
        this->hitResult = nullptr;
    }
    
    Ray ray = Ray::fromPlayer(this->player);
    
    this->hitResult = ray.trace(this->level, 5.0f, &this->level->entities);
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
    this->fogDistance = (float)(1024 >> (this->levelRenderer->drawDistance << 1));
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
    for (i = 0; i < this->level->entities.size(); ++i) {
        zombie = this->level->entities[i];
        if (frustum.isVisible(zombie->bb)) {
            this->level->entities[i]->render(partialTicks);
        }
    }
    this->checkGlError("Rendered entities");
    this->particleEngine->render(this->player, partialTicks, 0, this->textures);
    this->checkGlError("Rendered particles (0)");
    this->levelRenderer->renderSurroundingGround();
    this->checkGlError("Render surrounding Ground");
    glDisable(GL_LIGHTING);
    this->setupFog(-1);
    this->levelRenderer->renderClouds(partialTicks);
    this->checkGlError("Rendered clouds");
    this->setupFog(1);
    glEnable(GL_LIGHTING);
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
    glEnable(GL_FOG);
    glColorMask(false, false, false, false);
    this->levelRenderer->render(this->player, 1);
    glColorMask(true, true, true, true);
    this->levelRenderer->render(this->player, 1);
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
    gluPerspective(70.0, aspectRatio, 0.05, this->fogDistance);
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
    }
}

void CrossCraft::setupFog(int layer) {
    if (layer == -1) {
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogf(GL_FOG_START, 0.0f);
        glFogf(GL_FOG_END, this->fogDistance);
        glFogfv(GL_FOG_COLOR, getBuffer(0.92f, 0.98f, 1.0f, 1.0f));
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f));
    } else {
        Tile* currentTile = Tile::tiles[this->level->getTile((int)this->player->x, (int)(this->player->y+0.12f), (int)this->player->z)];
        if (currentTile != nullptr && currentTile->getLiquidType() == 1) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 0.1f);
            glFogfv(GL_FOG_COLOR, getBuffer(0.02f, 0.02f, 0.2f, 1.0f));
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.3f, 0.3f, 0.7f, 1.0f));
        } else if (currentTile != nullptr && currentTile->getLiquidType() == 2) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 2.0f);
            glFogfv(GL_FOG_COLOR, getBuffer(0.6f, 0.1f, 0.0f, 1.0f));
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.3f, 0.3f, 1.0f));
        } else if (layer == 0) {
            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, 0.0f);
            glFogf(GL_FOG_END, this->fogDistance);
            glFogfv(GL_FOG_COLOR, getBuffer(0.92f, 0.98f, 1.0f, 1.0f));
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f));
        } else if (layer == 1) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 0.01f);
            glFogfv(GL_FOG_COLOR, this->fogColor1.data());
            float br = 0.6f;
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(br, br, br, 1.0f));
        }

        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT);
        glEnable(GL_LIGHTING);
    }
}

void CrossCraft::beginLevelLoading(const char title[]) {
    this->title = title;
    this->status = "";
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

void CrossCraft::levelLoadUpdate(const char* status) {
    this->status = status;
}

void CrossCraft::levelLoadProgress(int progress) {
    int screenWidth = this->width * 240 / this->height;
    int screenHeight = this->height * 240 / this->height;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_TEXTURE_2D);
    int id = this->textures->loadTexture("dirt", GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, id);
    t.begin();
    t.color(64.0f / 255.0f, 64.0f / 255.0f, 64.0f / 255.0f);
    float s = 32.0f;
    t.vertexUV(0.0f, (float)screenHeight, 0.0f, 0.0f, (float)screenHeight / s);
    t.vertexUV((float)screenWidth, (float)screenHeight, 0.0f, (float)screenWidth / s, (float)screenHeight / s);
    t.vertexUV((float)screenWidth, 0.0f, 0.0f, (float)screenWidth / s, 0.0f);
    t.vertexUV(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    t.end();

    if (progress >= 0) {
        glDisable(GL_TEXTURE_2D);
        int barX = screenWidth / 2 - 50;
        int barY = screenHeight / 2 + 16;
        int barWidth = 100;
        int barHeight = 2;

        glColor3f(128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f);
        glBegin(GL_QUADS);
            glVertex2f(barX, barY);
            glVertex2f(barX, barY + barHeight);
            glVertex2f(barX + barWidth, barY + barHeight);
            glVertex2f(barX + barWidth, barY);
        glEnd();

        glColor3f(128.0f / 255.0f, 255.0f / 255.0f, 128.0f / 255.0f);
        glBegin(GL_QUADS);
            glVertex2f(barX, barY);
            glVertex2f(barX, barY + barHeight);
            glVertex2f(barX + progress, barY + barHeight);
            glVertex2f(barX + progress, barY);
        glEnd();
        

        glEnable(GL_TEXTURE_2D);
    }
    
    this->font->drawShadow(this->title, (screenWidth - this->font->width(this->title)) / 2, screenHeight / 2 - 4 - 16, 0xFFFFFF);
    this->font->drawShadow(this->status, (screenWidth - this->font->width(this->status)) / 2, screenHeight / 2 - 4 + 8, 0xFFFFFF);
    
    glfwSwapBuffers(window);
    
    emscripten_sleep(1); 
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
    const char* username = (this->user != nullptr) ? this->user->username.c_str() : "noname";
    this->levelGen->generateLevel(this->level, username, width, height, depth);
    this->player->resetPos();
    for (int i = static_cast<int>(this->level->entities.size()) - 1; i >= 0; --i) {
        this->level->entities.erase(this->level->entities.begin() + i);
    }
}

void CrossCraft::connectToServer(const std::string& serverUrl) {
    if (client == nullptr) {
        client = new Client();
    }
    
    client->setOnConnect([this]() {
        Logger::logf(PREFIX_NETWORK, "Sending login packet...\n");
        this->levelLoadUpdate("Sending login...");
        LoginPacket* packet = new LoginPacket(this->user->username, this->user->sessionid);
        client->sendPacket(packet);
    });
    
    client->setOnPacket([this](Packet* packet) {
        handleNetworkPacket(packet);
    });

    client->setOnError([this](std::string error) {
        connectError();
    });
    
    this->beginLevelLoading("Connecting to server");
    this->levelLoadUpdate("Try to connect...");
    client->connect(serverUrl);
}

void CrossCraft::connectError() {
    Logger::logf(PREFIX_NETWORK, "Connection error!\n");
    this->mpMode = false;
    this->init();
}

void CrossCraft::handleNetworkPacket(Packet* packet) {
    switch (packet->getType()) {
        case PacketType::LOGIN_RESPONSE:
            Logger::logf(PREFIX_NETWORK, "Login successful!\n");
            this->levelLoadUpdate("Successfuly logged!");
            break;

        case PacketType::LEVEL_DATA: {
            this->levelLoadUpdate("Load level...");
            LevelLoadPacket* p = static_cast<LevelLoadPacket*>(packet);
            
            this->level->isRemote = true;
            std::vector<uint8_t> levelData = this->levelIO->decompressGzip(p->compressedData.data(), p->compressedData.size());
            this->level->setData(p->width, p->depth, p->height, levelData);

            this->player->resetPos();
        
            Logger::logf(PREFIX_NETWORK, "Level data received and processed.\n");
            break;
        }
            
        case PacketType::BLOCK_UPDATE: {
            BlockUpdatePacket* blockPacket = static_cast<BlockUpdatePacket*>(packet);
            this->level->setTile(blockPacket->x, blockPacket->y, blockPacket->z, blockPacket->blockType);
            Logger::logf(PREFIX_NETWORK, "Block updated at %i, %i, %i\n", blockPacket->x, blockPacket->y, blockPacket->z);
            break;
        }
        
        // TODO: Other packets
        
        default:
            Logger::logf(PREFIX_WARNING, "Unknown packet type: %d\n", static_cast<int>(packet->getType()));
    }
}