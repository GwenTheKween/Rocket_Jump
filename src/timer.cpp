#include "timer.hpp"

void nop() {}

Timer::Timer(float length, std::optional<Timer::Action> autoAction):
    len(length),
    action(autoAction.value_or(&nop))
{
    reset();
}

bool Timer::done() {
    return elapsed >= len;
}

void Timer::reset() {
    elapsed = 0;
}

float Timer::length() const {
    return len;
}

void Timer::update(float deltaTime) {
    if (elapsed < len)
        elapsed += deltaTime;
    if (elapsed > len)
        elapsed = len;
}

float Timer::progress() const {
    return elapsed / len;
}

float Timer::timeLeft() const {
    return len - elapsed;
}

float Timer::timeElapsed() const {
    return elapsed;
}
