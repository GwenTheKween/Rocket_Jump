#pragma once

#include "entity.hpp"
#include "explosion.hpp"

class Rocket: public Entity {
    b2Vec2 direction;
    float remainingTime;
public:
    static constexpr float radius = 0.5;

    // how much larger the triangle should be compared to one
    // inscribed in the hitbox
    static constexpr float triangleToRadiusRatio = 1.2;
    static constexpr float mass = 1;
    static constexpr float approxArea = radius * radius;
    static constexpr float density = mass / approxArea;
    static constexpr float speed = 30;
    static constexpr float lifetime = 0.5;

    // direction will be normalized internally, can accept any non-null vector
    Rocket(b2World& world, b2Vec2 position, b2Vec2 direction);

    void render() const;
    void update(float deltaTime);
    void collide();
    bool shouldExplodeByAge();
    Explosion *spawnExplosion(b2Vec2 position);
};
