#include "CrossCraft.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

CrossCraft* CrossCraft::instance = nullptr;

CrossCraft::CrossCraft(const char* canvas, int w, int h, bool fs) : 
    width(w), 
    height(h), 
    fullscreen(fs), 
    window(nullptr),
    textures()
{
    CrossCraft::instance = this;
    if (canvas) {
        parent = canvas;
    }

    this->textures = new Textures();
    this->sound = new SoundManager();

    textureEffects.push_back(new WaterTextureFX());
    textureEffects.push_back(new LavaTextureFX());
}

CrossCraft::~CrossCraft() {
    this->destroy();
    if (this->textures) {
        delete this->textures;
    }
    if (this->level) {
        delete this->level;
    }
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
    this->bgB = 0.92f;
    this->bgG = 0.98f;
    this->bgB = 1.0f;
    fogColor0 = {this->bgR, this->bgG, this->bgB, 1.0};
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
    this->sound->initOpenAL(this->settings);
    this->font = new Font("default", this->textures);
    this->chatGui = new ChatGui();
    this->hud = new Hud(this->textures, this->width*240/this->height, this->height*240/this->height);
    glViewport(0, 0, this->width, this->height);
    this->level = new Level();
    this->level->cc = this;
    this->particleEngine = new ParticleEngine(this->level);

    if (!this->mpMode) {
        if (this->loadMapUser.empty() || this->loadMapId == -1) {
            if (this->userData != nullptr) {
                this->levelGen->generateLevel(this->level, this->userData->username.c_str(), 256, 256, 64);
            } else {
                this->levelGen->generateLevel(this->level, "noname", 256, 256, 64);
            }
        } else {
            this->loadLevel(this->loadMapUser.c_str(), this->loadMapId);
        }
    } else {
        this->connectionUrls.push_back("ws://" + this->serverAddress + ":" + std::to_string(this->serverPort));
        
        this->connectionUrls.push_back("ws://" + this->serverAddress + "/ws/");

        this->currentUrlIndex = 0;
        this->connectToServer();
    }

    this->levelRenderer = new LevelRenderer(this->level, this->textures);
    this->player = new Player(this->level, this->settings);

    Mouse::init(window);
    Keyboard::init(window);
    Item::initModels();
    Data::initAllowedTiles();

    Keyboard::enableRepeatEvents(false);
    
    this->hotbarSlots = {
        Tile::rock->id,
        Tile::sponge->id,
        Tile::dirt->id,
        Tile::wood->id,
        Tile::bush->id,
        Tile::log->id,
        Tile::leaves->id,
        Tile::glass->id,
        Tile::gravel->id,
    };
    this->hotbarIndex = 0;
    this->selectedTile = this->hotbarSlots[this->hotbarIndex];

    this->checkGlError("Post startup");
}

void CrossCraft::setScreen(Screen* screen) {
    if (this->screen != nullptr) this->screen->onClose();
    if (this->screen == nullptr && screen != nullptr) {
        Keyboard::clearEvents();
        Mouse::clearEvents();
    }
    this->screen = screen;
    if (screen == nullptr) {
        Mouse::clearEvents();
        this->clickDelay = 10;
    }
    if (screen != nullptr) {
        int screenWidth = this->width * 240 / this->height;
        int screenHeight = this->height * 240 / this->height;
        screen->init(this, screenWidth, screenHeight);
    }
}

void CrossCraft::grabMouse() {
    if (!this->mouseGrabbed) {
        Logger::logf(PREFIX_DEBUG, "CrossCraft: Requesting mouse grab\n");
        Mouse::setGrabbed(true);
        if (!this->appletMode) {
            Mouse::setCursorPosition(width / 2, height / 2);
        }
        this->setScreen(nullptr);
    }
}

void CrossCraft::releaseMouse() {
    if (this->mouseGrabbed) {
        Logger::logf(PREFIX_DEBUG, "CrossCraft: Releasing mouse\n");
        Mouse::setGrabbed(false);
        this->player->releaseAllKeys();
        if (this->screen == nullptr) {
            this->setScreen((Screen*)(new PauseScreen()));
        }
    }
}
 
void CrossCraft::stop() {
    this->running = false;
    emscripten_cancel_main_loop();
}

void CrossCraft::run() {
    if (this->userData != nullptr) {
        Logger::logf(PREFIX_CC, "CrossCraft started! canvas=%s, size=%dx%d, user=%s\n", 
            this->parent.c_str(), 
            this->width, 
            this->height, 
            this->userData->username.c_str());
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

    if (this->inErrorState) {
        this->drawErrorScreen();
    } else if (this->canRender) {
        this->checkGlError("Pre render");
        this->render(this->timer->partialTicks);
        this->checkGlError("Post render");
    }
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
    if (this->clickDelay > 0) {
        return;
    }

    if (this->editMode == 0) {
        if (this->hitResult != nullptr) {
            Tile* previousTile = Tile::tiles[this->level->getTile(this->hitResult->x, this->hitResult->y, this->hitResult->z)];

            bool tileChanged = this->level->setTile(this->hitResult->x, this->hitResult->y, this->hitResult->z, 0);
            if (previousTile != nullptr && tileChanged) {
                if (this->mpMode && this->client && this->client->isConnected()) {
                    this->level->setTile(this->hitResult->x, this->hitResult->y, this->hitResult->z, previousTile->id);
                    BlockChangePacket* packet = new BlockChangePacket(
                        this->hitResult->x, this->hitResult->y, this->hitResult->z, 
                        0, false);
                    client->sendPacket(packet);
                }
                if (previousTile->st != &SoundType::none) {
                    this->level->playSound("step." + previousTile->st->name, (float)this->hitResult->x, (float)this->hitResult->y, (float)this->hitResult->z, (previousTile->st->getVolume() + 1.0f) / 2.0f, previousTile->st->getPitch() * 0.8f);
                }
                previousTile->onDestroy(this->level, this->hitResult->x, this->hitResult->y, this->hitResult->z, this->particleEngine, this->isDrop);
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

        AABB* aabb = Tile::tiles[this->player->inventory->getCurrentBlock()]->getAABB(x, y, z);
        
        if (aabb == nullptr || this->isFree(*aabb)) {
            if (this->mpMode && client && client->isConnected()) {
                BlockChangePacket* packet = new BlockChangePacket(
                        x, y, z, 
                        this->player->inventory->getCurrentBlock(), true);
                client->sendPacket(packet);
            } else {
                this->level->setTile(x, y, z, this->player->inventory->getCurrentBlock());
            }
        }
        
        if (aabb != nullptr) {
            delete aabb;
        }
    }
}

void CrossCraft::tick() {
    if (this->sound != nullptr) {
        this->sound->tick();
    }

    if (this->settings) {
        this->levelRenderer->drawDistance = this->settings->renderDistance;
    }

    if (this->attackTime > 0) {
        this->attackTime--;
    }
    if (this->clickDelay > 0) {
        this->clickDelay--;
    }

    bool isActuallyGrabbed = Mouse::isGrabbed();

    double dWheel = Mouse::getDWheel();
    if (dWheel != 0.0) {
        const double threshold = 5.0; 
        
        int scrollDirection = 0;
        if (dWheel > threshold) scrollDirection = 1;
        else if (dWheel < -threshold) scrollDirection = -1;

        if (scrollDirection != 0) {
            int steps = static_cast<int>(dWheel / 100.0);
            if (steps == 0) steps = scrollDirection;

            this->player->inventory->selectedSlot += steps;
            int numSlots = this->player->inventory->slots.size();
            this->player->inventory->selectedSlot = (this->player->inventory->selectedSlot % numSlots + numSlots) % numSlots;
            // this->selectedTile = this->hotbarSlots[this->hotbarIndex];
        }
    }

    if (this->mouseGrabbed && !isActuallyGrabbed) {
        printf("CrossCraft: Pointer lock lost\n");
        this->mouseGrabbed = false;
        this->player->releaseAllKeys();
        
        if (this->screen == nullptr) {
            this->setScreen(new PauseScreen());
        }
    } 
    
    else if (!this->mouseGrabbed && isActuallyGrabbed) {
        printf("CrossCraft: Pointer lock acquired\n");
        this->mouseGrabbed = true;
        
        if (this->screen != nullptr && dynamic_cast<PauseScreen*>(this->screen)) {
            this->setScreen(nullptr);
        }
    }

    this->chatGui->tick();

    if (this->waitingForFocus) {
        if (Mouse::getEventButtonState()) {
            this->grabMouse();
        }
        
        if (Mouse::isGrabbed()) {
            this->waitingForFocus = false;
            this->mouseGrabbed = true;
        }
        
        return; 
    }

    if (this->mouseGrabbed && !Mouse::isGrabbed()) {
        printf("CrossCraft: Pointer lock released by browser (probably ESC)\n");
        this->releaseMouse();
    }

    if (this->screen == nullptr) {
        this->raycast();
    }

    if (this->screen != nullptr) {
        this->screen->updateEvents();
        if (this->screen != nullptr) {
            this->screen->tick();
        }
        
        goto update_world;
    }

    if (this->screen == nullptr) {
        while (Keyboard::next()) { 
            this->player->setKey();
            if (Keyboard::getEventKeyState()) {
                if (Keyboard::getEventKey() == GLFW_KEY_ESCAPE) {
                    this->releaseMouse();
                    break;
                }

                if (Keyboard::getEventKey() == this->settings->key_chat->keyCode && this->mpMode == true) {
                    this->player->releaseAllKeys();
                    this->setScreen(new ChatScreen());
                    this->releaseMouse();
                    break;
                }

                if (Keyboard::getEventKey() == GLFW_KEY_C) {
                    this->isDrop == true ? this->isDrop = false : this->isDrop = true;
                }

                if (Keyboard::getEventKey() == this->settings->key_build->keyCode) {
                    this->player->releaseAllKeys();
                    this->setScreen(new BlockSelectScreen());
                    this->releaseMouse();
                    break;
                }

                if (Keyboard::getEventKey() == this->settings->key_save->keyCode) {
                    this->level->setSpawnPos((int)this->player->x, (int)this->player->y, (int)this->player->z, (int)this->player->yRot);
                    this->player->resetPos();
                }

                if (Keyboard::getEventKey() == this->settings->key_load->keyCode) {
                    this->player->resetPos();
                }
                if (Keyboard::getEventKey() >= GLFW_KEY_1 && Keyboard::getEventKey() <= GLFW_KEY_9) {
                    int keyIndex = Keyboard::getEventKey() - GLFW_KEY_1;
                    if (keyIndex < this->player->inventory->slots.size()) {
                        this->player->inventory->selectedSlot = keyIndex;
                        // this->selectedTile = this->hotbarSlots[this->hotbarIndex];
                    }
                }
                if (Keyboard::getEventKey() == this->settings->key_fog->keyCode) {
                    this->settings->toggleSetting(4, 1);
                }
                if (Keyboard::getEventKey() == GLFW_KEY_G && !this->mpMode && this->level->entities.size() < 256) {
                    this->level->entities.push_back(new Zombie(this->level, this->textures, this->player->x, this->player->y, this->player->z));
                }
            }
        }
        while (Mouse::next()) {
            if (!this->mouseGrabbed && Mouse::getEventButtonState()) {
                this->grabMouse();
                break;
            }

            if (!this->mouseGrabbed && Mouse::getEventButtonState()) {
                this->grabMouse();
            } else {
                if (Mouse::getEventButton() == 0 && Mouse::getEventButtonState()) {
                    this->handleMouseClick();
                    this->attackTime = 5;
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

                        if (pickedID == Tile::unbreakable->id) {
                            pickedID = Tile::rock->id;
                        }

                        this->player->inventory->pickTile(pickedID);

                        // if (pickedID > 0 && Tile::tiles[pickedID] != nullptr && Tile::tiles[pickedID]->mayPick()) {
                        //     for (int i = 0; i < this->player->inventory->slots.size(); ++i) {
                        //         if (this->player->inventory->slots[i] == pickedID) {
                        //             this->player->inventory->selectedSlot = i;
                        //             // this->selectedTile = this->hotbarSlots[this->hotbarIndex];
                        //             break;
                        //         }
                        //     }
                        // }
                    }
                }
            }
        }

        if (this->screen == nullptr && glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (this->attackTime <= 0) {
                this->handleMouseClick();
                this->attackTime = 5; 
            }
        }

    }
update_world:
    for (TextureFX* fx : textureEffects) {
        fx->tick();
        textures->updateTextureFX(fx->pixels, fx->textureId);
    }
    ++this->levelRenderer->cloudTicks;
    this->level->tick();
    this->particleEngine->tick();

    this->player->tick();
    this->sound->updateListener(this->player->x, this->player->y, this->player->z, this->player->xRot, this->player->yRot);

    for (auto const& [id, net_player] : this->level->networkPlayers) {
        if (net_player != nullptr) {
            net_player->tick();
        }
    }

    if (mpMode && client->isConnected()) { 
        PositionPacket* pos_packet = new PositionPacket(
            this->player->x, this->player->y, this->player->z,
            this->player->yRot, this->player->xRot
        );
        client->sendPacket(pos_packet);
    }
}
 
void CrossCraft::raycast() {
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
        int iy = -1;
        if (this->settings->invertYMouse) iy = 1;
        this->player->turn(xo, yo * static_cast<float>(iy));
    }
    
    this->checkGlError("Set viewport");
    this->checkGlError("Rasycasted");
    glClearColor(this->bgR, this->bgG, this->bgB, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->fogDistance = (float)(512 >> (this->levelRenderer->drawDistance << 1));
    this->setupCamera(partialTicks);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
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
    Entity* entity;
    for (i = 0; i < this->level->entities.size(); ++i) {
        entity = this->level->entities[i];
        if (frustum.isVisible(entity->bb)) {
            this->level->entities[i]->render(partialTicks, this->textures);
        }
    }
    for (auto const& [id, net_player] : this->level->networkPlayers) {
        if (net_player != nullptr) {
            net_player->render(this->textures, partialTicks, this->font, this->player);
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
        this->levelRenderer->renderHit(this->hitResult, this->player, this->editMode, this->player->inventory->getCurrentBlock());
        this->levelRenderer->renderHitOutline(this->hitResult, this->player, this->editMode, this->player->inventory->getCurrentBlock());
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
        this->levelRenderer->renderHit(this->hitResult, this->player, this->editMode, this->player->inventory->getCurrentBlock());
        this->levelRenderer->renderHitOutline(this->hitResult, this->player, this->editMode, this->player->inventory->getCurrentBlock());
        glEnable(GL_ALPHA_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
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
    Tessellator& t = Tessellator::getInstance();
    this->checkGlError("GUI: Init");
    glPushMatrix();
    // glTranslatef(screenWidth - 16, 16.0f, -50.0f);
    // glScalef(16.0f, 16.0f, 16.0f);
    // glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    // glRotatef(45.0, 0.0f, 1.0f, 0.0f);
    // glTranslatef(-1.5f, 0.5f, 0.5f);
    // glScalef(-1.0f, -1.0f, -1.0f);
    // GLuint id = this->textures->loadTexture("terrain", GL_NEAREST);
    // glBindTexture(GL_TEXTURE_2D, id);
    // glEnable(GL_TEXTURE_2D);
    // t.begin();
    // Tile::tiles[this->player->inventory->getCurrentBlock()]->render(t, this->level, 0, -2, 0, 0);
    // t.end();
    // glDisable(GL_TEXTURE_2D);
    if (this->hud != nullptr) {
        this->hud->render(this->player, this->level, partialTicks);
    }
    glPopMatrix();
    this->checkGlError("GUI: Draw selected");
    this->font->drawShadow(VERSION_STRING, 2, 2, 0xFFFFFF);
    if (this->settings->showFPS)
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
    this->chatGui->render(this->font, screenWidth, screenHeight);
        if (this->mpMode && Keyboard::isKeyDown(GLFW_KEY_TAB)) {
        playerListScreen->init(this, this->width * 240 / this->height, this->height * 240 / this->height);
        playerListScreen->render(Mouse::getX(), Mouse::getY());
    }
    if (this->screen != nullptr) {
        this->screen->render(xMouse, yMouse);
    }
}

void CrossCraft::fill(int x0, int y0, int x1, int y1, int col) {
    float a = (col >> 24 & 255) / 255.0f;
    float r = (col >> 16 & 255) / 255.0f;
    float g = (col >> 8 & 255) / 255.0f;
    float b = (col & 255) / 255.0f;
    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    t.begin();
    t.vertex((float)x0, (float)y1, 0.0f);
    t.vertex((float)x1, (float)y1, 0.0f);
    t.vertex((float)x1, (float)y0, 0.0f);
    t.vertex((float)x0, (float)y0, 0.0f);
    t.end();
    glDisable(GL_BLEND);
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
            this->bgR = 0.02f;
            this->bgG = 0.02f;
            this->bgB = 0.2f;
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.3f, 0.3f, 0.7f, 1.0f));
        } else if (currentTile != nullptr && currentTile->getLiquidType() == 2) {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, 2.0f);
            glFogfv(GL_FOG_COLOR, getBuffer(0.6f, 0.1f, 0.0f, 1.0f));
            this->bgR = 0.6f;
            this->bgG = 0.1f;
            this->bgB = 0.0f;
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(0.4f, 0.3f, 0.3f, 1.0f));
        } else if (layer == 0) {
            this->bgR = 0.92f;
            this->bgG = 0.98f;
            this->bgB = 1.0f;
            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, 0.0f);
            glFogf(GL_FOG_END, this->fogDistance);
            glFogfv(GL_FOG_COLOR, getBuffer(0.92f, 0.98f, 1.0f, 1.0f));
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, getBuffer(1.0f, 1.0f, 1.0f, 1.0f));
        } else if (layer == 1) {
            this->bgR = 0.92f;
            this->bgG = 0.98f;
            this->bgB = 1.0f;
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
    this->lastProgress = -1;
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
    if (progress == this->lastProgress) {
        return;
    }
    this->lastProgress = progress;
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
    this->levelIO->saveOnline(this->level, this->serverHost, this->userData->username, this->userData->sessionid, levelname, levelId);
}

void CrossCraft::generateNewLevel(int width, int height, int depth) {
    const char* username = (this->userData != nullptr) ? this->userData->username.c_str() : "noname";
    this->levelGen->generateLevel(this->level, username, width, height, depth);
    this->player->resetPos();
    for (int i = static_cast<int>(this->level->entities.size()) - 1; i >= 0; --i) {
        this->level->entities.erase(this->level->entities.begin() + i);
    }
}

void CrossCraft::showErrorScreen(const std::string& title, const std::string& reason) {
    this->inErrorState = true;
    this->errorTitle = title;
    this->errorReason = reason;
}

void CrossCraft::drawErrorScreen() {
    int screenWidth = this->width * 240 / this->height;
    int screenHeight = this->height * 240 / this->height;

    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, screenWidth, screenHeight, 0.0f, 100.0f, 300.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);

    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    
    glColor4f(0.42f, 0.08f, 0.07f, 1.0f);  // #6b1412
    glVertex2f(screenWidth, 0);
    glVertex2f(0, 0);
    
    glColor4f(0.24f, 0.02f, 0.02f, 1.0f); // #3d0505
    glVertex2f(0, screenHeight);
    glVertex2f(screenWidth, screenHeight);
    
    glEnd();

    glPushMatrix();
    glTranslatef(screenWidth / 2.0f, screenHeight / 2.0f - 20.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    this->font->drawCentered(this->errorTitle, 0, 0, 0xFFFFFFFF);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(screenWidth / 2.0f, screenHeight / 2.0f + 10.0f, 0.0f);
    this->font->drawCentered(this->errorReason, 0, 0, 0xFFFFFFFF);
    glPopMatrix();
}

void CrossCraft::connectToServer() {
    if (currentUrlIndex >= connectionUrls.size()) {
        this->showErrorScreen("Connection Failed", "All connection methods failed.");
        return;
    }

    std::string urlToTry = connectionUrls[currentUrlIndex];
    currentUrlIndex++;

    std::string status = "Connecting... (Attempt " + std::to_string(currentUrlIndex) + "/" + std::to_string(connectionUrls.size()) + ")";
    this->beginLevelLoading("Connecting to server...");
    this->levelLoadUpdate(status.c_str()); 
    this->levelLoadProgress(10);

    if (client == nullptr) {
        client = new Client();
    }
    
    client->setOnConnect([this]() {
        this->canRender = false;
        Logger::logf(PREFIX_NETWORK, "WebSocket connection opened, waiting for Server ID...\n");
    });
    
    client->setOnPacket([this](Packet* packet) {
        handleNetworkPacket(packet);
    });

    client->setOnError([this](std::string error) {
        Logger::logf(PREFIX_NETWORK, "Attempt %d failed: %s\n", this->currentUrlIndex, error.c_str());
        this->connectToServer(); 
    });

    client->setOnClose([this](std::string reason) {
        this->showErrorScreen("Disconnected", reason);
    });
    
    this->beginLevelLoading("Connecting to server...");
    this->levelLoadUpdate("Establishing connection"); 
    this->levelLoadProgress(10);
    client->connect(urlToTry);
}

void CrossCraft::connectError(std::string error) {
    Logger::logf(PREFIX_NETWORK, "Connection error!\n");
    this->showErrorScreen("Connection Error!", error);
}

void CrossCraft::handleNetworkPacket(Packet* packet) {
    switch (packet->getType()) {
        case PacketType::SERVER_IDENTIFICATION: {
            ServerIdentificationPacket* p = static_cast<ServerIdentificationPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Server Identification received: %s\n", p->serverName.c_str());
            
            this->beginLevelLoading(p->serverName.c_str());
            this->levelLoadUpdate(p->serverMotd.c_str());
            this->levelLoadProgress(25);
            
            Logger::logf(PREFIX_NETWORK, "Sending login packet...\n");
            this->levelLoadProgress(40);
            if (this->userData != nullptr) {
                    LoginPacket* loginPacket = new LoginPacket(this->userData->username, this->userData->sessionid); // тут так-же ещё внутри отправляется версия протокола, главное не забывать менять, хд.
                    client->sendPacket(loginPacket);
                } else {
                    Logger::logf(PREFIX_WARNING, "User is null, sending guest login\n");
                    LoginPacket* loginPacket = new LoginPacket("", ""); 
                    client->sendPacket(loginPacket);
                }
            break;
        }

        case PacketType::LOGIN_RESPONSE: {
            LoginResponsePacket* p = static_cast<LoginResponsePacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Login successful! Server assigned name: %s\n", p->username.c_str());
            this->playerId = p->playerId;
            this->client->loggedIn = true;
            if (this->userData == nullptr) {
                this->userData = new Data(p->username, ""); 
            } else {
                this->userData->username = p->username;
            }
            this->levelLoadProgress(60);
            break;

        }

        case PacketType::LEVEL_DATA: {
            this->levelLoadProgress(80);
            LevelLoadPacket* p = static_cast<LevelLoadPacket*>(packet);
            
            this->level->isRemote = true;
            std::vector<uint8_t> levelData = this->levelIO->decompressGzip(p->compressedData.data(), p->compressedData.size());
            this->level->setData(p->width, p->depth, p->height, levelData); // fff

            this->levelLoadProgress(100);
            emscripten_sleep(1000);
            this->canRender = true;
        
            Logger::logf(PREFIX_NETWORK, "Level data received and processed.\n");
            RequestSpawnPositionPacket* requestPacket = new RequestSpawnPositionPacket();
            this->client->sendPacket(requestPacket);
            break;
        }
            
        case PacketType::BLOCK_UPDATE: {
            BlockUpdatePacket* blockPacket = static_cast<BlockUpdatePacket*>(packet);
            this->level->setTile(blockPacket->x, blockPacket->y, blockPacket->z, blockPacket->blockType);
            // Logger::logf(PREFIX_NETWORK, "Block updated at %i, %i, %i\n", blockPacket->x, blockPacket->y, blockPacket->z);
            break;
        }

        case PacketType::PLAYER_SPAWN: {
            SpawnPlayerPacket* p = static_cast<SpawnPlayerPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Spawning player %s (ID: %d)\n", p->username.c_str(), p->playerId);
            
            NetworkPlayer* new_player = new NetworkPlayer(this->level, p->playerId, p->username, p->x, p->y, p->z, p->yaw, p->pitch);
            Logger::logf(PREFIX_NETWORK, "Username in player: %s", new_player->username.c_str());
            this->level->networkPlayers[p->playerId] = new_player;
            break;
        }

        case PacketType::PLAYER_POSITION: {
            PositionPacket* p = static_cast<PositionPacket*>(packet);
            
            if (this->player != nullptr && p->playerId == this->playerId && p->type == 1) {
                this->player->setPos(p->x, p->y, p->z);
                this->player->yRot = p->yaw;
                this->player->xRot = p->pitch;
                Logger::logf(PREFIX_NETWORK, "Teleported by server to %f, %f, %f\n", p->x, p->y, p->z);
            } 
            else {
                auto it = this->level->networkPlayers.find(p->playerId);
                if (it != this->level->networkPlayers.end()) {
                    it->second->queue(p->x, p->y, p->z, p->yaw, p->pitch);
                }
            }
            break;
        }

        case PacketType::PLAYER_DESPAWN: {
            DespawnPlayerPacket* p = static_cast<DespawnPlayerPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Despawning player (ID: %d)\n", p->playerId);

            auto it = this->level->networkPlayers.find(p->playerId);
            if (it != this->level->networkPlayers.end()) {
                delete it->second;
                this->level->networkPlayers.erase(it);
            }
            break;
        }

        case PacketType::SET_SPAWN_POSITION: {
            SetSpawnPositionPacket* p = static_cast<SetSpawnPositionPacket*>(packet);
            Logger::logf(PREFIX_NETWORK, "Received spawn position: %d, %d, %d\n", p->x, p->y, p->z);
            
            this->level->setSpawnPos(p->x, p->y, p->z, p->yaw);
            
            this->player->resetPos();
            Logger::logf(PREFIX_NETWORK, "New spawn pos: %d, %d, %d\n", this->level->xSpawn, this->level->ySpawn, this->level->zSpawn);
            break;
        }

        case PacketType::SERVER_CHAT_MESSAGE: {
            ChatMessagePacket* p = static_cast<ChatMessagePacket*>(packet);
            this->chatGui->addMessage(p->message);
            break;
        }
            
        // TODO: Other packets
        
        default:
            Logger::logf(PREFIX_WARNING, "Unknown packet type: %d\n", static_cast<int>(packet->getType()));
    }
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void showCrashScreen(const char* msg) {
        if (CrossCraft::instance) {
            printf("Creating crash screen for: %s\n", msg);
            emscripten_cancel_main_loop(); 
            CrossCraft::instance->showErrorScreen("Runtime Error", msg);
            glfwMakeContextCurrent(CrossCraft::instance->window);
            CrossCraft::instance->drawErrorScreen();
            glfwSwapBuffers(CrossCraft::instance->window);
            glFlush();
            glFinish();
            printf("Crash screen rendered.\n");
        }
    }
}