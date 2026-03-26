#include "filterChain.h"
#include <vector>

FilterChain::FilterChain() {
    for (auto& filter : filters_) {
        filter.configure(44100.0f, 1000.0f, kQ_, 0.0f);
    }
    scopeBuffer_.fill(0.0f);
}

void FilterChain::configure(float sampleRate, const std::vector<float>& gains) {
    for (int i = 0; i < 10; ++i) {
        filters_[i].configure(sampleRate, kFrequencies_[i], kQ_, gains[i]);
    }
}

float FilterChain::process(float input) {
    float output = input;
    for (auto& filter : filters_) {
        output = filter.process(output);
    }
    return output;
}

void FilterChain::writeScopeSample(float sample) {
    scopeBuffer_[writeIndex_.fetch_add(1, std::memory_order_relaxed) % kScopeBufferSize] = sample;
}

void FilterChain::reset() {
    for (auto& filter : filters_) {
        filter.reset();
    }
}
