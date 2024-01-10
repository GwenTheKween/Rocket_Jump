#pragma once

#include <array>

#include "entity.hpp"
#include "explosion.hpp"

class Rocket: public Entity {
    b2Vec2 direction;
    float remainingTime;
    bool wasDestroyed;

    std::array<b2Vec2, 3> localCoordsVertices;
public:
    static constexpr float lifetime = 0.3f;
    // TODO should rockets inherit velocity from player?

    // direction will be normalized internally, can accept any non-null vector
    Rocket(b2World& world, b2Vec2 position, b2Vec2 direction);

    void render() const;
    void update(float deltaTime);
    bool shouldExplodeByAge() const;
    bool hasExploded() const;
    void collide();
};
