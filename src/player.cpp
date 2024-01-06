#include "player.hpp"

#include <raylib.h>

#include "world.hpp"

b2Body *constructPlayerBody(b2World& world, b2Vec2 spawnPosition) {
    b2BodyDef bodyDef;
    bodyDef.position = std::move(spawnPosition);
    bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructPlayerShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = Player::radius;
    return shape;
}

b2Fixture *constructPlayerFixture(b2Body *body, b2Shape *shape) {
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = Player::density;
    return body->CreateFixture(&fixtureDef);
}

Player::Player(b2World& world, b2Vec2 position):
    Entity(
        world,
        constructPlayerBody(world, position),
        constructPlayerShape(),
        Player::density
    ) {
}

void Player::render() const {
    auto rlPos = raylibPosition();
    DrawCircle(rlPos.x, rlPos.y, metersToPixels(Player::radius), RED);
}
