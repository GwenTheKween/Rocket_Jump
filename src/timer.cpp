#include "timer.hpp"

void nop() {}

Timer::Timer(float length): Timer(length, &nop) {}

Timer::Timer(float length, Timer::Action autoAction):
    len(length),
    action(autoAction)
{
    reset();
}

bool Timer::done() {
    return elapsed >= len;
}

void Timer::reset() {
    didAction = false;
    elapsed = 0;
}

void Timer::performAction() {
    didAction = true;
    action();
}

bool Timer::triggerActionAgainIfDone() {
    if (done()) {
        performAction();
        return true;
    } else {
        return false;
    }
}

void Timer::setToComplete() {
    if (!done()) {
        elapsed = len;
        performAction();
    }
}

float Timer::length() const {
    return len;
}

void Timer::update(float deltaTime) {
    if (elapsed < len)
        elapsed += deltaTime;
    if (elapsed > len)
        elapsed = len;
    if (done() && !didAction) {
        performAction();
    }
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
