#pragma once
#include <vector>
#include "util/Random.hpp"

class PerlinNoiseFilter {
private:
    static constexpr int FUZZINESS = 16;
    int octave;

public:
    /**
     * Perlin noise generator
     *
     * @param octave The strength of the noise
     */
    PerlinNoiseFilter(int octave);
    
    /**
     * Read random noise values with given amount
     *
     * @param width  Noise width
     * @param height Noise height
     * @return Noise map
     */
    std::vector<int> read(int width, int height);
};
