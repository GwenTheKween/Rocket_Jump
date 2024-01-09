#include "explosion.hpp"

#include <cmath>

#include "world.hpp"

float clamp01(float t) {
    if (t < 0) return 0;
    if (t > 1) return 1;
    return t;
}

float explosionExpansionEasing(float t) {
    t = clamp01(t);
    return 1 - std::pow(1 - t, 3);
}

float lerp(float a, float b, float t) {
    return a * (1-t) + b * t;
}

b2Body *constructExplosionBody(b2World& world, b2Vec2 position) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = position;
    bodyDef.gravityScale = 0;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructExplosionShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = Explosion::initialRadius;
    return shape;
}

Explosion::Explosion(b2World& world, b2Vec2 position):
    Entity(
        world,
        constructExplosionBody(world, position),
        constructExplosionShape(),
        1,
        Entity::EntityType::EXPLOSION,
        Entity::EntityType::PLAYER
    ),
    timeAliveRatio(0) {}

void Explosion::update(float deltaTime) {
    timeAliveRatio += deltaTime / Explosion::lifetime;
    float easedExpansionTime = explosionExpansionEasing(timeAliveRatio);
    fixture->GetShape()->m_radius = lerp(Explosion::initialRadius, Explosion::maxRadius, easedExpansionTime);
}

void Explosion::render() const {
    Vector2 center = raylibPosition();
    float raylibRadius = metersToPixels(fixture->GetShape()->m_radius);
    DrawCircleLinesV(center, raylibRadius, YELLOW);
}

bool Explosion::isOver() const {
    return timeAliveRatio >= 1.0f;
}

float Explosion::calculateStrength() const {
    return Explosion::baseStrength / fixture->GetShape()->m_radius;
}
