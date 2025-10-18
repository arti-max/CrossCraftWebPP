#pragma once
#include "particle/Particle.hpp"
#include "level/Level.hpp"
#include "Player.hpp"
#include "render/Tessellator.hpp"
#include "Textures.hpp"
#include <vector>
#include <memory>

class ParticleEngine {
private:
    Level* level;
    std::vector<Particle*> particles;

public:
    ParticleEngine(Level* level);
    ~ParticleEngine();

    /**
     * Add particle to engine
     * @param particle The particle to add
     */
    void add(Particle* particle);

    /**
     * Update all particles
     */
    void tick();

    /**
     * Render all particles
     * @param player       The player
     * @param partialTicks Ticks for interpolation
     * @param layer        Shadow layer (0 or 1)
     * @param textures     Texture manager
     */
    void render(Player* player, float partialTicks, int layer, Textures* textures);

private:
    /**
     * Convert degrees to radians
     */
    inline float toRad(float degrees) {
        return degrees * M_PI / 180.0f;
    }
};
