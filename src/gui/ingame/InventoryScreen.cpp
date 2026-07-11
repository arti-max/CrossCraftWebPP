#include "gui/ingame/InventoryScreen.hpp"
#include "util/UVMapper.hpp"
#include "render/Tessellator.hpp"
#include "model/HumanModel.hpp"
#include "CrossCraft.hpp"
#include "util/Random.hpp"

InventoryScreen::InventoryScreen() {

}

void InventoryScreen::init() {
    this->grabMouse = true;

    this->timeOffs = Random::random() * 1239813.0f;
}

void InventoryScreen::mouseClicked(int x, int y, int button) {
    this->cc->setScreen((Screen*)nullptr);
    this->cc->waitingForFocus = true;
}

void InventoryScreen::tick() {
    this->animStepO = this->animStep;
    this->runO = this->run;

    this->run += (1.0f - this->run) * 0.3f;

    float speed = std::sqrt(0.1*0.1 + 0.2*0.2);

    this->animStep += (speed * 2.0f);
}

void InventoryScreen::render(int xMouse, int yMouse) {
    glDisable(GL_DEPTH_TEST);
    UVCoords uv = UVMapper::map(0, 0, 256, 154, 256, 256);
    Tessellator& t = Tessellator::getInstance();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->cc->textures->loadTexture("/gui/inventory.png", GL_NEAREST));

    glPushMatrix();
    
    t.begin();
    t.vertexUV(this->width/2-140, 30, 0.0f, uv.u0, uv.v0);
    t.vertexUV(this->width/2-140, 184, 0.0f, uv.u0, uv.v1);
    t.vertexUV(this->width/2+140, 184, 0.0f, uv.u1, uv.v1);
    t.vertexUV(this->width/2+140, 30, 0.0f, uv.u1, uv.v0);
    t.end();

    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_COLOR_MATERIAL);

    glPushMatrix();

    this->renderPlayerModel(this->cc->timer->partialTicks);

    glPopMatrix();

    glCullFace(GL_BACK);

    glDisable(GL_RESCALE_NORMAL);
    glDisable(GL_COLOR_MATERIAL);
}

void InventoryScreen::renderPlayerModel(float partialTicks) {
    float interpAnimStep = this->animStepO + (this->animStep - this->animStepO) * partialTicks;
    float interpRun = this->runO + (this->run - this->runO) * partialTicks;

    glTranslatef(128.0f, 90.0f, -60.0f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-160.0f, 0.0f, 1.0f, 0.0f);

    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    double time = (static_cast<double>(milliseconds) / 1000.0 * 10.0 * static_cast<double>(1.0f)) + static_cast<double>(this->timeOffs);

    this->cc->player->bindTexture(this->cc);
    this->models->getModel("Human")->render(interpAnimStep, interpRun, (float)this->cc->ticks+partialTicks, std::sin(time * 0.83), std::sin(time) * 0.8f, 3.0f);
}