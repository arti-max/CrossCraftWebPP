#pragma once
#include "render/fx/TextureFX.hpp"
#include <cmath>
#include <cstdlib>

class LavaTextureFX : public TextureFX {
private:
    float current_heat[256];
    float next_heat[256];
    float extra_energy[256];
    float energy_delta[256];

public:
    LavaTextureFX() : TextureFX(30) {
        for (int i = 0; i < 256; ++i) {
            current_heat[i] = 0.0f;
            next_heat[i] = 0.0f;
            extra_energy[i] = 0.0f;
            energy_delta[i] = 0.0f;
        }
    }

    void tick() override {
        for (int x = 0; x < 16; ++x) {
            for (int y = 0; y < 16; ++y) {
                float accumulated_heat = 0.0f;
                int offset_x = static_cast<int>(
                    std::sin(static_cast<double>(y) * M_PI * 2.0 / 16.0) * 1.2
                );
                int offset_y = static_cast<int>(
                    std::sin(static_cast<double>(x) * M_PI * 2.0 / 16.0) * 1.2
                );

                for (int sample_x = x - 1; sample_x <= x + 1; ++sample_x) {
                    for (int sample_y = y - 1; sample_y <= y + 1; ++sample_y) {
                        int wrapped_x = (sample_x + offset_x) & 15;
                        int wrapped_y = (sample_y + offset_y) & 15;
                        accumulated_heat += current_heat[wrapped_x + (wrapped_y << 4)];
                    }
                }

                int index = x + (y << 4);
                float averaged_extra_energy = (
                    extra_energy[(x & 15) + ((y & 15) << 4)] +
                    extra_energy[(x + 1 & 15) + ((y & 15) << 4)] +
                    extra_energy[(x + 1 & 15) + ((y + 1 & 15) << 4)] +
                    extra_energy[(x & 15) + ((y + 1 & 15) << 4)]
                ) / 4.0f;

                next_heat[index] = accumulated_heat / 10.0f + averaged_extra_energy * 0.8f;

                extra_energy[index] += energy_delta[index] * 0.01f;
                if (extra_energy[index] < 0.0f) {
                    extra_energy[index] = 0.0f;
                }
                energy_delta[index] -= 0.06f;
                if (static_cast<float>(std::rand()) / RAND_MAX < 0.005f) {
                    energy_delta[index] = 1.5f;
                }
            }
        }

        for (int i = 0; i < 256; ++i) {
            float tmp = next_heat[i];
            next_heat[i] = current_heat[i];
            current_heat[i] = tmp;
        }

        for (int i = 0; i < 256; ++i) {
            float value = current_heat[i] * 2.0f;
            if (value > 1.0f) value = 1.0f;
            if (value < 0.0f) value = 0.0f;

            int red   = static_cast<int>(value * 100.0f + 155.0f);
            int green = static_cast<int>(value * value * 255.0f);
            int blue  = static_cast<int>(value * value * value * value * 128.0f);

            if (anaglyph) {
                int r = (red * 30 + green * 59 + blue * 11) / 100;
                int g = (red * 30 + green * 70) / 100;
                int b = (red * 30 + blue * 70) / 100;
                red = r;
                green = g;
                blue = b;
            }

            pixels[i * 4 + 0] = static_cast<unsigned char>(red);
            pixels[i * 4 + 1] = static_cast<unsigned char>(green);
            pixels[i * 4 + 2] = static_cast<unsigned char>(blue);
            pixels[i * 4 + 3] = 255;
        }
    }
};