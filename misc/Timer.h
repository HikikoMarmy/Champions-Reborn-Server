#pragma once

#include <chrono>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::chrono::high_resolution_clock::time_point m_stopTime;
    bool m_running;

public:
    Timer() : m_running(false) {}

    void Start() {
        if (!m_running) {
            m_startTime = std::chrono::high_resolution_clock::now();
            m_running = true;
        }
    }

    void Stop() {
        if (m_running) {
            m_stopTime = std::chrono::high_resolution_clock::now();
            m_running = false;
        }
    }

    void Reset() {
        m_startTime = std::chrono::high_resolution_clock::now();
        m_stopTime = m_startTime;
        m_running = false;
    }

    double GetElapsedTime() const
    {
        if (m_running) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double>(currentTime - m_startTime).count();
        }
        else {
            return std::chrono::duration<double>(m_stopTime - m_startTime).count();
        }
    }

    long long GetElapsedTimeMilliseconds() const
    {
        if( m_running )
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( currentTime - m_startTime );
            return duration.count();
        }
        else
        {
            auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( m_stopTime - m_startTime );
            return duration.count();
        }
    }
};
