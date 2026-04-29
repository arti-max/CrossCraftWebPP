#include "mob/HumanMob.hpp"
#include "level/Level.hpp"

HumanMob::HumanMob(Level* level, float x, float y, float z) : Mob(level) {
    this->modelName = "human";
    this->setPos(x, y, z);
}

void HumanMob::renderModel(Textures* textures, float time, float speed, float tick, float headYRot, float headXRot, float scale) {
    Mob::renderModel(textures, time, speed, tick, headYRot, headXRot, scale);
    Model* model = this->modelManager->getModel(this->modelName);
    glEnable(GL_ALPHA_TEST);
    if (this->allowAlpha) {
        glEnable(GL_CULL_FACE);
    }

    glBindTexture(GL_TEXTURE_2D, textures->loadTexture("/armor/plate.png", GL_NEAREST));
    glDisable(GL_CULL_FACE);
    HumanModel* armorModel = (HumanModel*)this->modelManager->getModel("human.armor");
    HumanModel* hmodel = (HumanModel*)model;
    armorModel->head->yaw = hmodel->head->yaw;
    armorModel->head->pitch = hmodel->head->pitch;
    armorModel->arm0->pitch = hmodel->arm0->pitch;
    armorModel->arm0->roll = hmodel->arm0->roll;
    armorModel->arm1->pitch = hmodel->arm1->pitch;
    armorModel->arm1->roll = hmodel->arm1->roll;
    armorModel->leg0->pitch = hmodel->leg0->pitch;
    armorModel->leg1->pitch = hmodel->leg1->pitch;
    armorModel->head->render(scale);
    armorModel->body->render(scale);
    armorModel->arm0->render(scale);
    armorModel->arm1->render(scale);
    armorModel->leg0->render(scale);
    armorModel->leg1->render(scale);
    glEnable(GL_CULL_FACE);

    glDisable(GL_ALPHA_TEST);
}