#pragma once

#include "entity.hpp"

class Wall: public Entity {
    const Vector2 pixelDimensions;

public:
    Wall(b2World& world, b2Vec2 position, b2Vec2 dimensions);
    void update(float deltaTime) {}
    void render() const;
};
