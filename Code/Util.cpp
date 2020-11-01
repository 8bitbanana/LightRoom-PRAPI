#include "Util.h"

#include <random>
#include <chrono>

namespace Util {

    std::minstd_rand engine;

    void init_random() {
        auto time = std::chrono::high_resolution_clock().now();

        // Cast the time_point directly to an int to use as the random seed
        unsigned int seed = *(unsigned int*)(void*)&time;

        printf("Seed: %u\n", seed);

        engine = std::minstd_rand();
        engine.seed(seed);
    }

    unsigned int random() {
        return engine();
    }

    float random_float(float min, float max) {
        auto rand = (float)random();
        return min + (rand / (engine.max()/(max-min)));
    }

    float poly_interpolation(float value, int index) {
        if (value < 0) value = 0;
        if (value > 1) value = 1;
        return abs(pow(value, index));
    }
}