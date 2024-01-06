#pragma once

#include <box2d/box2d.h>
#include <raylib.h>
#include <utility>

#include "entity.hpp"

class Player: public Entity {
public:
    // in meters
    static constexpr float radius = 1;
    static constexpr float mass = 1;
    static constexpr float approxArea = radius * radius;
    static constexpr float density = mass / approxArea;

    Player(b2World& world, b2Vec2 position);

    void render() const;
};
