#pragma once

#include <optional>
#include <functional>

class Timer {
    const float len;
    float elapsed;
    std::function<void()> action;
public:
    using Action = decltype(action);

    /**
     * @brief Construct a new Timer object.
     *
     * @param length Time length of the Timer.
     * @param autoAction Action to take automatically when timer concludes, if any.
     */
    Timer(float length, std::optional<Action> autoAction=std::nullopt);

    /**
     * @brief Checks whether or not the timer has finished.
     *
     * @return true The timer has finished.
     * @return false The timer is still running.
     */
    bool done();

    /**
     * @brief Reset progress to 0.
     */
    void reset();

    /**
     * @brief Checks the full length of the Timer.
     * @return float The length of time the Timer takes to complete from a reset.
     */
    float length() const;

    /**
     * @brief Progress along timer a given amount of time.
     * @param deltaTime How much time has elapsed since construction or last call to update.
     */
    void update(float deltaTime);

    /**
     * @brief Checks how far along the timer is.
     * @return [0, 1] range, 0 being at the start and 1 being at the end.
     */
    float progress() const;

    /**
     * @brief Checks how much time is left until the timer is triggered.
     * @return float The amount of time that needs to pass to trigger the timer.
     */
    float timeLeft() const;

    /**
     * @brief Checks how much time has passed since the last call to reset().
     * @return float The amount of time that has passed.
     */
    float timeElapsed() const;
};
