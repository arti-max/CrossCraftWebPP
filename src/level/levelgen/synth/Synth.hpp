#pragma once
#include <vector>

class Synth {
public:
    virtual ~Synth() = default;
    
    virtual double getValue(double x, double y) = 0;
    
    std::vector<double> create(int width, int height) {
        std::vector<double> result(width * height);
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                result[x + y * width] = this->getValue(static_cast<double>(x), static_cast<double>(y));
            }
        }
        
        return result;
    }
};
