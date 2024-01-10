#pragma once

#include "entity.hpp"

class Explosion: public Entity {
    float timeAliveRatio;

public:
    static constexpr float lifetime = 1.0f;
    static constexpr float initialRadius = 1.0f;
    static constexpr float maxRadius = 4.0f;
    static constexpr float baseStrength = 4000.0f;

    Explosion(b2World& world, b2Vec2 position);
    void render() const;
    void update(float deltaTime);
    bool isOver() const;
    float calculateStrength() const;
};
