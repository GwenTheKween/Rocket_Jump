#include "entity.hpp"

#include <utility>

#include "world.hpp"

b2Fixture *make_fixture(b2Body *body, b2Shape *shape, float fixtureDensity) {
    return body->CreateFixture(shape, fixtureDensity);
}

void Entity::swap(Entity& other) {
    body = std::exchange(other.body, body);
    shape = std::exchange(other.shape, shape);
    fixture = std::exchange(other.fixture, fixture);
    world = std::exchange(other.world, world);
}

b2BodyDef Entity::defaultBodyDef() {
    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_dynamicBody;
    return def;
}

Entity::Entity(b2World& world, b2Body *body, b2Shape *shape, float fixtureDensity):
    world(world),
    body(body),
    shape(shape),
    fixture(make_fixture(body, shape, fixtureDensity)) {
}

Entity::Entity(Entity&& e): world(e.world) {
    this->swap(e);
}

Entity& Entity::operator=(Entity&& p) {
    this->swap(p);
    return *this;
}

Entity::~Entity() {
    if (fixture != nullptr) body->DestroyFixture(fixture);
    if (body != nullptr) world.get().DestroyBody(body);
    delete shape;
}

b2Vec2 Entity::box2dPosition() const {
    return body->GetPosition();
}

Vector2 Entity::raylibPosition() const {
    return box2dToRaylib(box2dPosition());
}