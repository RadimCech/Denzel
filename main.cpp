#include <jack/jack.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <iostream>

void openJackClient(float *volume, std::atomic<bool> *running);

int main() {
    float volume = 0.5f;
    std::atomic<bool> running(true);
    std::thread t([&volume, &running]() { openJackClient(&volume, &running); });

    while (running.load()) {
        float newVolume;
        if (std::cin >> newVolume) {
            volume = newVolume;
        } else {
            running = false;
        }
    }

    return 0;
}
