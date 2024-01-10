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
    static constexpr float radius = 0.5f;

    // how much larger the triangle should be compared to one
    // inscribed in the hitbox
    static constexpr float triangleToRadiusRatio = 1.5f;
    static constexpr float mass = 1.0f;
    static constexpr float approxArea = radius * radius;
    static constexpr float density = mass / approxArea;
    static constexpr float speed = 30.0f;
    static constexpr float lifetime = 0.5f;

    // direction will be normalized internally, can accept any non-null vector
    Rocket(b2World& world, b2Vec2 position, b2Vec2 direction);

    void render() const;
    void update(float deltaTime);
    bool shouldExplodeByAge() const;
    bool hasExploded() const;
    void collide();
};
