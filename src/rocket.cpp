#include "rocket.hpp"
#include "world.hpp"

b2Body *constructRocketBody(b2World& world, b2Vec2 position, b2Vec2 direction) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = position;
    bodyDef.gravityScale = 0;
    direction.Normalize();
    bodyDef.linearVelocity = Rocket::speed * direction;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructRocketShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = Rocket::radius;
    return shape;
}

Rocket::Rocket(b2World& world, b2Vec2 position, b2Vec2 direction)
    : Entity(
        world,
        constructRocketBody(world, position, direction),
        constructRocketShape(),
        Rocket::density,
        Entity::EntityType::ROCKET,
        Entity::EntityType::TERRAIN
    ),
    direction(direction),
    remainingTime(Rocket::lifetime) {

    this->direction.Normalize();
}

void Rocket::render() const {
    static const float halfRoot3 = 0.5f * sqrt(3);

    // head
    auto v1 = Rocket::triangleToRadiusRatio * Rocket::radius * direction;
    // head rotated by 60 degrees
    b2Vec2 v2{
        -0.5f*v1.x - halfRoot3*v1.y,
        halfRoot3*v1.x  - 0.5f*v1.y
    };
    // last vertex
    auto v3 = -(v1 + v2);

    auto pos = box2dPosition();
    auto r1 = box2dToRaylib(pos + v1);
    auto r2 = box2dToRaylib(pos + v2);
    auto r3 = box2dToRaylib(pos + v3);
    DrawTriangleLines(r1, r3, r2, BLUE);
}

void Rocket::update(float deltaTime) {
    remainingTime -= deltaTime;
}

bool Rocket::shouldExplodeByAge() const {
    return remainingTime <= 0;
}

bool Rocket::hasExploded() const {
    return wasDestroyed;
}

Explosion *Rocket::spawnExplosion(b2Vec2 position) {
    wasDestroyed = true;
    // TODO spawn explosion
    return nullptr;
}
