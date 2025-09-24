#pragma once
#include "level/levelgen/synth/Synth.hpp"

class Distort : public Synth {
private:
    Synth* source;
    Synth* distort;
public:
    Distort(Synth* src, Synth* d) {
        this->source = src;
        this->distort = d;
    }
    double getValue(double x, double y) {
        return this->source->getValue(x + this->distort->getValue(x, y), y);
    }
};