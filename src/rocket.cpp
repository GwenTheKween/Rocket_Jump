#include "rocket.hpp"
#include "world.hpp"

constexpr float radius = 0.5f;

// how much larger the triangle should be compared to one
// inscribed in the hitbox
constexpr float triangleToRadiusRatio = 1.5f;
constexpr float mass = 1.0f;
constexpr float approxArea = radius * radius;
constexpr float density = mass / approxArea;
constexpr float speed = 30.0f;

b2Body *constructRocketBody(b2World& world, b2Vec2 position, b2Vec2 direction) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = position;
    bodyDef.gravityScale = 0;
    direction.Normalize();
    bodyDef.linearVelocity = speed * direction;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructRocketShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = radius;
    return shape;
}

Rocket::Rocket(b2World& world, b2Vec2 position, b2Vec2 direction)
    : Entity(
        world,
        constructRocketBody(world, position, direction),
        constructRocketShape(),
        density,
        Entity::EntityType::ROCKET,
        Entity::EntityType::TERRAIN | Entity::EntityType::EXPLOSION
    ),
    direction(direction),
    remainingTime(lifetime),
    wasDestroyed(false) {

    this->direction.Normalize();

    static const float halfRoot3 = 0.5f * sqrtf(3.0f);

    // head
    auto v1 = triangleToRadiusRatio * radius * direction;
    // head rotated by 60 degrees
    auto v2 = b2Vec2 {
        -0.5f*v1.x - halfRoot3*v1.y,
        halfRoot3*v1.x  - 0.5f*v1.y
    };
    // last vertex
    auto v3 = -(v1 + v2);
    localCoordsVertices.at(0) = v1;
    localCoordsVertices.at(1) = v2;
    localCoordsVertices.at(2) = v3;
}

void Rocket::render() const {
    auto pos = box2dPosition();
    auto r1 = box2dToRaylib(pos + localCoordsVertices.at(0));
    auto r2 = box2dToRaylib(pos + localCoordsVertices.at(1));
    auto r3 = box2dToRaylib(pos + localCoordsVertices.at(2));
    DrawTriangleLines(r1, r3, r2, BLUE);

#ifdef DEBUG
    DrawCircleLinesV(raylibPosition(), metersToPixels(radius), WHITE);
#endif
}

void Rocket::update(float deltaTime) {
    remainingTime -= deltaTime;
}

bool Rocket::shouldExplodeByAge() const {
    return !hasExploded() && remainingTime <= 0;
}

bool Rocket::hasExploded() const {
    return wasDestroyed;
}

void Rocket::collide() {
    wasDestroyed = true;
}
