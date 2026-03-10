#pragma once

#include "biquad.h"
#include <array>
#include <vector>

class FilterChain {
public:
  FilterChain();

  void configure(float sampleRate, const std::vector<float> &gains);
  float process(float input);
  void reset();

private:
  static constexpr float kFrequencies_[10] = {
      31.0f,   63.0f,   125.0f,  250.0f,  500.0f,
      1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f};
  static constexpr float kQ_ = 1.4f;

  std::array<Biquad, 10> filters_;
};
