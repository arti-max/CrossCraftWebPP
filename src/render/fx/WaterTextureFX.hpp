#pragma once
#include "render/fx/TextureFX.hpp"
#include <cmath>
#include <cstdlib>

class WaterTextureFX : public TextureFX {
private:
    float current_height[256];
    float next_height[256];
    float disturbance[256];
    float disturbance_delta[256];

    int tick_counter = 0;

public:
    WaterTextureFX() : TextureFX(14) {
        for (int i = 0; i < 256; ++i) {
            current_height[i] = 0.0f;
            next_height[i] = 0.0f;
            disturbance[i] = 0.0f;
            disturbance_delta[i] = 0.0f;
        }
    }

    void tick() override {
        ++tick_counter;

        for (int x = 0; x < 16; ++x) {
            for (int y = 0; y < 16; ++y) {
                float sum = 0.0f;

                for (int sample_x = x - 1; sample_x <= x + 1; ++sample_x) {
                    int wrapped_x = sample_x & 15;
                    int wrapped_y = y & 15;
                    sum += current_height[wrapped_x + (wrapped_y << 4)];
                }

                int index = x + (y << 4);
                next_height[index] = sum / 3.3f + disturbance[index] * 0.8f;
            }
        }

        for (int x = 0; x < 16; ++x) {
            for (int y = 0; y < 16; ++y) {
                int index = x + (y << 4);

                disturbance[index] += disturbance_delta[index] * 0.05f;
                if (disturbance[index] < 0.0f) {
                    disturbance[index] = 0.0f;
                }

                disturbance_delta[index] -= 0.1f;

                if (static_cast<float>(std::rand()) / (float)RAND_MAX < 0.05f) {
                    disturbance_delta[index] = 0.5f;
                }
            }
        }

        for (int i = 0; i < 256; ++i) {
            float tmp = next_height[i];
            next_height[i] = current_height[i];
            current_height[i] = tmp;
        }

        for (int i = 0; i < 256; ++i) {
            float value = current_height[i];

            if (value > 1.0f) value = 1.0f;
            if (value < 0.0f) value = 0.0f;

            float value2 = value * value;
            
            int red   = static_cast<int>(32.0f  + value2 * 32.0f);
            int green = static_cast<int>(50.0f  + value2 * 64.0f);
            int blue  = 255;
            int alpha = static_cast<int>(146.0f + value2 * 50.0f);

            if (anaglyph) {
                int r = (red * 30 + green * 59 + 2805) / 100;
                int g = (red * 30 + green * 70) / 100;
                int b = (red * 30 + 17850) / 100;
                red = r;
                green = g;
                blue = b;
            }

            pixels[i * 4 + 0] = static_cast<unsigned char>(red);
            pixels[i * 4 + 1] = static_cast<unsigned char>(green);
            pixels[i * 4 + 2] = static_cast<unsigned char>(blue);
            pixels[i * 4 + 3] = static_cast<unsigned char>(alpha);
        }
    }
};