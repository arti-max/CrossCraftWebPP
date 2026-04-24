#pragma once
#include <vector>
#include <cmath>
#include "Entity.hpp"
#include "level/EntityMeshSlot.hpp"
#include "phys/AABB.hpp"
#include "render/Frustum.hpp"
#include "render/Textures.hpp"

class EntityMesh {
private:
    std::vector<Entity*> tmp;

public:
    int width, height, depth;
    std::vector<std::vector<Entity*>> grid; // entities
    std::vector<Entity*> all; // all entities

    EntityMeshSlot* slotStart = new EntityMeshSlot(this);
    EntityMeshSlot* slotEnd = new EntityMeshSlot(this);

    EntityMesh(int w, int h, int d);

    std::vector<Entity*> getEntities(Entity* ignore, float x0, float y0, float z0, float x1, float y1, float z1, std::vector<Entity*> result);
    std::vector<Entity*> getEntities(Entity* ignore, const AABB& box);

    void addEntity(Entity* e);
    void removeEntity(Entity* e);

    void render(Frustum& frustum, Textures* textures, float partialTicks);
    void tickAll();
};



/*
1. Куллинг по чанкам, если мы не смотрим на чанк, то и рендерить энтити не нужно
2. Сделать методы для спавна и деспавна энтити
3. Сделать метод получения энтити
4. Сделать метод рендера
*/