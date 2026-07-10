#pragma once
#include "Entity.hpp"
#include <cmath>
#include <vector>

class EntityMesh;

class EntityMeshSlot {
    friend class EntityMesh;
    int xSlot = 0, ySlot = 0, zSlot = 0;
    EntityMesh* mesh = nullptr;

    explicit EntityMeshSlot(EntityMesh* m) : mesh(m) {}

public:
    EntityMeshSlot& init(float x, float y, float z);

    void add(Entity* e);
    bool remove(Entity* e);

    static int getXSlot(EntityMeshSlot* slot);
    static int getYSlot(EntityMeshSlot* slot);
    static int getZSlot(EntityMeshSlot* slot);

    bool operator==(const EntityMeshSlot& other) const {
        return xSlot == other.xSlot &&
            ySlot == other.ySlot &&
            zSlot == other.zSlot &&
            mesh == other.mesh;
    }

    bool operator!=(const EntityMeshSlot& other) const {
        return !(*this == other);
    }
};