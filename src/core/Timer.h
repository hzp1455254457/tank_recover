#pragma once

#include <chrono>
#include <functional>

namespace BattleCity {

// Time management for 60 FPS game loop
class Timer {
private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::milliseconds;

    static constexpr double FRAME_TIME = 1000.0 / 60.0; // 16.67ms per frame
    static constexpr int MAX_FRAME_SKIP = 5;

    TimePoint lastFrameTime_;
    double accumulator_;
    uint64_t frameCount_;
    bool isPaused_;

public:
    Timer() : accumulator_(0.0), frameCount_(0), isPaused_(false) {
        lastFrameTime_ = Clock::now();
    }

    // Update timer and call update function for each frame
    void update(std::function<void()> updateFunc, std::function<void(double)> renderFunc = nullptr) {
        if (isPaused_) {
            // Still update last frame time to prevent large jumps when unpaused
            lastFrameTime_ = Clock::now();
            return;
        }

        TimePoint currentTime = Clock::now();
        Duration deltaTime = std::chrono::duration_cast<Duration>(currentTime - lastFrameTime_);
        lastFrameTime_ = currentTime;

        accumulator_ += deltaTime.count();

        int framesProcessed = 0;
        while (accumulator_ >= FRAME_TIME && framesProcessed < MAX_FRAME_SKIP) {
            updateFunc();
            accumulator_ -= FRAME_TIME;
            frameCount_++;
            framesProcessed++;
        }

        // Render with interpolation
        if (renderFunc) {
            double alpha = accumulator_ / FRAME_TIME;
            renderFunc(alpha);
        }
    }

    // Get current frame count
    uint64_t getFrameCount() const {
        return frameCount_;
    }

    // Get time elapsed in seconds
    double getElapsedSeconds() const {
        return frameCount_ * FRAME_TIME / 1000.0;
    }

    // Get time elapsed in frames
    uint64_t getElapsedFrames() const {
        return frameCount_;
    }

    // Check if it's time for a specific interval (in frames)
    bool everyNFrames(uint64_t n) const {
        return frameCount_ % n == 0;
    }

    // Pause/unpause timer
    void setPaused(bool paused) {
        isPaused_ = paused;
        if (!paused) {
            lastFrameTime_ = Clock::now();
        }
    }

    bool isPaused() const {
        return isPaused_;
    }

    // Reset timer
    void reset() {
        frameCount_ = 0;
        accumulator_ = 0.0;
        lastFrameTime_ = Clock::now();
        isPaused_ = false;
    }
};

} // namespace BattleCity