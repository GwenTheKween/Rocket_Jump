#include "explosion.hpp"

#include "world.hpp"

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
    timeAlive(0) {}

void Explosion::update(float deltaTime) {
    timeAlive += deltaTime;
    // TODO apply easing function here
    shape->m_radius = Explosion::initialRadius + Explosion::radiusIncreasePerSecond * timeAlive;
}

void Explosion::render() const {
    Vector2 center = raylibPosition();
    float raylibRadius = metersToPixels(shape->m_radius);
    DrawCircleLinesV(center, raylibRadius, YELLOW);
}

bool Explosion::isOver() const {
    return timeAlive >= Explosion::lifetime;
}

float Explosion::calculateStrength() const {
    return 1.0f / shape->m_radius;
}
