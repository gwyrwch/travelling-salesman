#pragma once

#include <chrono>

class Timer {
private:
    std::chrono::time_point<std::chrono::system_clock> start;

public:
    Timer() {
        Reset();
    }

    double Passed() {
        const auto delta = std::chrono::system_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
    }

    void Reset() {
        start = std::chrono::system_clock::now();
    }

    double PassedAndReset() {
        return Reset(), Passed();
    }
};
