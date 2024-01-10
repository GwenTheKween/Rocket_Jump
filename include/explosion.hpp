#pragma once

#include "entity.hpp"

class Explosion: public Entity {
    float timeAliveRatio;

public:
    Explosion(b2World& world, b2Vec2 position);
    void render() const;
    void update(float deltaTime);
    bool isOver() const;
    float calculateStrength() const;
};
