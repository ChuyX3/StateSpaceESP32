#pragma once
#include <Arduino.h>
#include <stdint.h>

class StepTimer
{
public:
    StepTimer()
        : m_targetElapsedTime(0)
        , m_lastTime(0)
        , m_elapsedTime(0)
        , m_totalTime(0)
        , m_leftOverTime(0)
        , m_tickCount(0) {
    }

    uint64_t elapsedTime() const{
        return m_elapsedTime;
    }
    uint64_t totalTime() const{
        return m_totalTime;
    }
    uint32_t tickCount() const{
        return m_tickCount;
    }
    void ticksPerSecond(uint64_t ts) {
        m_targetElapsedTime = 1000000 / ts;
    }
    void fixedStepTime(uint64_t us) {
        m_targetElapsedTime = us;
    }

    void reset() {
        m_lastTime = micros();
        m_elapsedTime = 0;
        m_totalTime = 0;
        m_leftOverTime = 0;
        m_tickCount = 0;
    }
    void update() {
        uint64_t currentTime = micros();
        m_elapsedTime = currentTime - m_lastTime;
        m_lastTime = currentTime;
        m_totalTime += m_elapsedTime;
        m_leftOverTime = 0;
        m_tickCount++;
    }

    template<typename T>
    inline void tick(T&& func) {
        uint64_t currentTime = micros();
        uint64_t timeDelta = currentTime - m_lastTime;
        m_lastTime = currentTime;

        if (m_targetElapsedTime > 0) {
            m_leftOverTime += timeDelta;
            while (m_leftOverTime >= m_targetElapsedTime) {
                m_elapsedTime = m_targetElapsedTime;
                m_totalTime += m_targetElapsedTime;
                m_leftOverTime -= m_targetElapsedTime;
                m_tickCount++;
                func();
            }
        }
        else {
            m_elapsedTime = timeDelta;
            m_totalTime += timeDelta;
            m_leftOverTime = 0;
            m_tickCount++;
            func();
        }
    }

private:
    uint64_t m_targetElapsedTime;
    uint64_t m_lastTime;
    uint64_t m_elapsedTime;
    uint64_t m_totalTime;
    uint64_t m_leftOverTime;
    uint32_t m_tickCount;
};

