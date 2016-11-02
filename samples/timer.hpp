#ifndef SAMPLE_TIMER_HPP_
#define SAMPLE_TIMER_HPP_
#include <chrono>

class Timer
{
    std::chrono::high_resolution_clock::time_point startPoint, endPoint;

public:
    Timer() {
        start();
    }

    void start() {
        startPoint = std::chrono::high_resolution_clock::now();
    }
    double get() {
        return 1000.0 * std::chrono::duration_cast<std::chrono::duration<double>>(endPoint - startPoint).count();
    }
    void stop() {
        endPoint = std::chrono::high_resolution_clock::now();
    }
};

#endif // SAMPLE_TIMER_HPP_

