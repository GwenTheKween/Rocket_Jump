#include "player.hpp"

#include <raylib.h>

#include "world.hpp"

b2Body *constructPlayerBody(b2World& world, b2Vec2 spawnPosition) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = std::move(spawnPosition);
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
        Player::density,
        EntityType::PLAYER,
        EntityType::TERRAIN | EntityType::EXPLOSION
    ) {}

void Player::render() const {
    auto rlPos = raylibPosition();
    DrawCircleV(rlPos, metersToPixels(Player::radius), RED);
}

void Player::update(float deltaTime) {
    if (ammo >= Player::maxRockets) {
        // keep reload progress frozen while at full ammo
        currentReload = Player::reloadTime;
        return;
    }

    currentReload -= deltaTime;
    if (currentReload <= 0) {
        ammo++;
        currentReload = Player::reloadTime;
    }
}

int Player::getAmmo() const {
    return this->ammo;
}

float Player::getReload() const {
    return this->currentReload;
}

Rocket *Player::shootRocketTowards(b2Vec2 target) {
    if (ammo <= 0) {
        return nullptr;
    }
    auto pos = box2dPosition();
    b2Vec2 direction = target - pos;
    direction.Normalize();
    ammo--;
    return new Rocket(world, pos, direction);
}
