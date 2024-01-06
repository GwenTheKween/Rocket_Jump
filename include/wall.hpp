#pragma once

#include "entity.hpp"

class Wall: public Entity {
    const Vector2 pixelDimensions;

public:
    static constexpr float density = 1.0f;

    Wall(b2World& world, b2Vec2 position, b2Vec2 dimensions);
    void render() const;
};