#pragma once

#include "entity.hpp"
#include "timer.hpp"

class RecoilWave: public Entity {
    Timer duration;
    void disable();
public:
    static constexpr float lifetime = 1.0f;

    RecoilWave(b2World& world);
    void update(float deltaTime);
    void render() const;
    void moveTo(b2Vec2 position, b2Vec2 direction);
};
