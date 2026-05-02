#include "level/EntityMeshSlot.hpp"
#include "level/EntityMesh.hpp"
#include <vector>

EntityMeshSlot& EntityMeshSlot::init(float x, float y, float z) {
    if (std::isnan(x) || std::isinf(x)) x = 0.0f;
    if (std::isnan(y) || std::isinf(y)) y = 0.0f;
    if (std::isnan(z) || std::isinf(z)) z = 0.0f;
    
    this->xSlot = (int)(x / 16.0f);
    this->ySlot = (int)(y / 16.0f);
    this->zSlot = (int)(z / 16.0f);

    this->xSlot = std::max(0, this->xSlot);
    this->ySlot = std::max(0, this->ySlot);
    this->zSlot = std::max(0, this->zSlot);

    if (this->xSlot >= this->mesh->width) this->xSlot = this->mesh->width - 1;
    if (this->ySlot >= this->mesh->depth) this->ySlot = this->mesh->depth - 1;
    if (this->zSlot >= this->mesh->height) this->zSlot = this->mesh->height - 1;

    return *this;
}

void EntityMeshSlot::add(Entity* e) {
    if (this->xSlot >= 0 && this->ySlot >= 0 && this->zSlot >= 0) {
        this->mesh->grid[(this->ySlot * this->mesh->height + this->zSlot) * this->mesh->width + this->xSlot].push_back(e);
    }
}

void EntityMeshSlot::remove(Entity* e) {
    if (this->xSlot >= 0 && this->ySlot >= 0 && this->zSlot >= 0) {
        auto& cell = this->mesh->grid[(this->ySlot * this->mesh->height + this->zSlot) * this->mesh->width + this->xSlot];
        for (size_t i = 0; i < cell.size(); ++i) {
            if (cell[i] == e) {
                cell.erase(cell.begin() + i);
                break;
            }
        }
    }
}

int EntityMeshSlot::getXSlot(EntityMeshSlot* slot) {
    return slot->xSlot;
}

int EntityMeshSlot::getYSlot(EntityMeshSlot* slot) {
    return slot->ySlot;
}

int EntityMeshSlot::getZSlot(EntityMeshSlot* slot) {
    return slot->zSlot;
}