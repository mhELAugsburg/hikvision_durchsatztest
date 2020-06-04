#pragma once

#include <chrono>
#include <iostream>
#include <atomic>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

class HighResClock {
public:
    HighResClock(std::string _name, uint64_t _maxMeasurements) : name(_name), maxMeasurements(_maxMeasurements) { reset(); }

    void start(uint64_t _id) {
        std::chrono::high_resolution_clock::time_point time_start = std::chrono::high_resolution_clock::now();

        if (id > 0) {
            return;
        }

        id = _id;
        m_time_start = time_start;
    }

    void stop(uint64_t _id) {
        std::chrono::high_resolution_clock::time_point time_stop = std::chrono::high_resolution_clock::now();
        uint64_t time = std::chrono::duration_cast<std::chrono::microseconds>(time_stop - m_time_start).count();

        // maximum measure time 10s
        if (time > 10000000 /*10s*/) {
            reset();
            return;
        }

        if (_id != id) {
            return;
        }

        measurementTimes += time;

        if (time < minimumTime) {
            minimumTime = time;
        }
        if (time > maximumTime) {
            maximumTime = time;
        }

        if (++measurementCounter >= maxMeasurements) {

            std::cerr << measurementCounter << " measurements completed in avg:" << (measurementTimes / measurementCounter) <<
                         "us, min:" << minimumTime << "us, max:" << maximumTime << "us" << std::endl;
            reset();
        }

        id = 0;
    }

    void reset() {
        minimumTime = 0xffffffffffffffff;
        measurementCounter = 0;
        measurementTimes = 0;
        maximumTime = 0;
        id = 0;
    }

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point m_time_start;

    std::atomic<uint64_t> maxMeasurements;
    std::atomic<uint64_t> measurementCounter;
    std::atomic<uint64_t> measurementTimes;
    std::atomic<uint64_t> minimumTime;
    std::atomic<uint64_t> maximumTime;
    std::atomic<uint64_t> id;
};
