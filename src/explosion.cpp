#include "explosion.hpp"

#include <cmath>
#include <algorithm>

#include "world.hpp"

constexpr float lifetime = 1.0f;
constexpr float initialRadius = 1.0f;
constexpr float maxRadius = 4.0f;
constexpr float hitboxRadius = 3.0f;
constexpr float baseStrength = 600.0f;


float explosionExpansionEasing(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return 1 - std::pow(1 - t, 3);
}

b2Body *constructExplosionBody(b2World& world, b2Vec2 position) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = position;
    bodyDef.gravityScale = 0;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructExplosionShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = hitboxRadius;
    return shape;
}

Explosion::Explosion(b2World& world, b2Vec2 position):
    Entity(
        world,
        constructExplosionBody(world, position),
        constructExplosionShape(),
        1,
        Entity::EntityType::EXPLOSION,
        Entity::EntityType::PLAYER | Entity::EntityType::ROCKET
    ),
    timeAliveRatio(0),
    animRadius(initialRadius) {}

void Explosion::update(float deltaTime) {
    timeAliveRatio += deltaTime / lifetime;
    float easedExpansionTime = explosionExpansionEasing(timeAliveRatio);
    animRadius = std::lerp(initialRadius, maxRadius, easedExpansionTime);
}

void Explosion::render() const {
    Vector2 center = raylibPosition();
    float raylibRadius = metersToPixels(animRadius);
    DrawCircleLinesV(center, raylibRadius, YELLOW);
}

bool Explosion::isOver() const {
    return timeAliveRatio >= 1.0f;
}

float Explosion::calculateStrength() const {
    return baseStrength / animRadius;
}
