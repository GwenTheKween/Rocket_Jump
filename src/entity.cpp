#include "entity.hpp"

#include <utility>

#include "world.hpp"

b2Fixture *make_fixture(
    Entity *entity,
    b2Body *body,
    b2Shape *shape,
    float fixtureDensity,
    int collisionCategory,
    int collisionMask
) {
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = fixtureDensity;
    fixtureDef.userData.pointer = entity->toUserDataPointer();
    fixtureDef.isSensor = collisionCategory == Entity::EntityType::EXPLOSION;
    fixtureDef.filter.categoryBits = collisionCategory;
    fixtureDef.filter.maskBits = collisionMask;
    return body->CreateFixture(&fixtureDef);
}

void Entity::swap(Entity& other) {
    body = std::exchange(other.body, body);
    fixture = std::exchange(other.fixture, fixture);
    world = std::exchange(other.world, world);
}

b2BodyDef Entity::defaultBodyDef() {
    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_dynamicBody;
    return def;
}

Entity::Entity(
    b2World& world,
    b2Body *body,
    b2Shape *shape,
    float fixtureDensity,
    EntityType type,
    int collisionMask
):
    world(world),
    body(body),
    fixture(make_fixture(
        this,
        body,
        shape,
        fixtureDensity,
        type,
        collisionMask
    )),
    type(type) {
    delete shape;
}

Entity::Entity(Entity&& e): world(e.world), type(e.type) {
    this->swap(e);
}

Entity& Entity::operator=(Entity&& p) {
    this->swap(p);
    return *this;
}

Entity::~Entity() {
    if (fixture != nullptr) body->DestroyFixture(fixture);
    if (body != nullptr) world.get().DestroyBody(body);
}

b2Vec2 Entity::box2dPosition() const {
    return body->GetPosition();
}

Vector2 Entity::raylibPosition() const {
    return box2dToRaylib(box2dPosition());
}

Entity *Entity::fromUserDataPointer(uintptr_t pointer) {
    return reinterpret_cast<Entity *>(pointer);
}

Entity *Entity::fromFixture(const b2Fixture *fixture) {
    return Entity::fromUserDataPointer(fixture->GetUserData().pointer);
}

uintptr_t Entity::toUserDataPointer() {
    return reinterpret_cast<uintptr_t>(this);
}
