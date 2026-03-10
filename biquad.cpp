#include "biquad.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void Biquad::configure(float sampleRate, float freq, float q, float gainDb) {
    float A = std::pow(10.0f, gainDb / 40.0f);
    float omega = 2.0f * M_PI * freq / sampleRate;
    float sinO = std::sin(omega);
    float cosO = std::cos(omega);
    float alpha = sinO / (2.0f * q);
    
    float a1 = -2.0f * cosO;
    float a2 = 1.0f - alpha / A;
    float norm = 1.0f + alpha / A;
    
    b0_ = (1.0f + alpha * A) / norm;
    b1_ = (-2.0f * cosO) / norm;
    b2_ = (1.0f - alpha * A) / norm;
    a1_ = a1 / norm;
    a2_ = a2 / norm;
}

float Biquad::process(float input) {
    float output = b0_ * input + b1_ * x1_ + b2_ * x2_
                 - a1_ * y1_ - a2_ * y2_;
    
    x2_ = x1_;
    x1_ = input;
    y2_ = y1_;
    y1_ = output;
    
    return output;
}

void Biquad::reset() {
    x1_ = x2_ = y1_ = y2_ = 0.0f;
}
