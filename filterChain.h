#pragma once

#include "biquad.h"
#include <array>
#include <atomic>
#include <vector>

class FilterChain {
public:
  static constexpr int kScopeBufferSize = 512;

  FilterChain();

  void configure(float sampleRate, const std::vector<float> &gains);
  float process(float input);
  void reset();

  void writeScopeSample(float sample);
  const float* getScopeBuffer() const { return scopeBuffer_.data(); }
  int getScopeWriteIndex() const { return writeIndex_.load(std::memory_order_acquire); }

private:
  static constexpr float kFrequencies_[10] = {
      31.0f,   63.0f,   125.0f,  250.0f,  500.0f,
      1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f};
  static constexpr float kQ_ = 1.4f;

  std::array<Biquad, 10> filters_;
  std::array<float, kScopeBufferSize> scopeBuffer_;
  std::atomic<int> writeIndex_{0};
};
