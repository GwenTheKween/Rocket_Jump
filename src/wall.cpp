#include "wall.hpp"

#include <raylib.h>

#include "world.hpp"

b2Body *constructWallBody(b2World& world, b2Vec2 position) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = std::move(position);
    bodyDef.type = b2_staticBody;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructWallShape(b2Vec2 dimensions) {
    auto box = new b2PolygonShape();
    b2Vec2 vertices[4] = {
        {0, 0},
        {0, dimensions.y},
        {dimensions.x, dimensions.y},
        {dimensions.x, 0}
    };
    box->Set(vertices, 4);
    return box;
}

Wall::Wall(b2World& world, b2Vec2 position, b2Vec2 dimensions)
    : Entity(
        world,
        constructWallBody(world, position),
        constructWallShape(dimensions),
        Wall::density
    ),
    pixelDimensions(box2dToRaylib(dimensions)) {}

void Wall::render() const {
    Vector2 position = raylibPosition();
    DrawRectangleLines(position.x, position.y, pixelDimensions.x, pixelDimensions.y, WHITE);
}