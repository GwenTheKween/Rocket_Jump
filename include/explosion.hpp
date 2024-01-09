#pragma once

#include "entity.hpp"

class Explosion: public Entity {
    float timeAlive;

public:
    // TODO take into account easing function
    static constexpr float radiusIncreasePerSecond = 3;
    static constexpr float lifetime = 0.5f;
    static constexpr float initialRadius = 1;
    static constexpr float maxRadius = initialRadius + radiusIncreasePerSecond*lifetime;

    Explosion(b2World& world, b2Vec2 position);
    void render() const;
    void update(float deltaTime);
    bool isOver() const;
    float calculateStrength() const;
};
