#pragma once

class Biquad {
public:
    Biquad() = default;
    
    void configure(float sampleRate = 44100.0f, 
                   float freq = 1000.0f, 
                   float q = 1.4f, 
                   float gainDb = 0.0f);
    
    float process(float input);
    void reset();

private:
    float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;
    float a1_ = 0.0f, a2_ = 0.0f;
    
    float x1_ = 0.0f, x2_ = 0.0f;
    float y1_ = 0.0f, y2_ = 0.0f;
};
