#pragma once

#include <box2d/box2d.h>
#include <functional>

// avoids double definition of vector types
#include <raylib.h>
#include <raymath.h>

class Entity {
protected:
    b2Body *body;
    b2Shape *shape;
    b2Fixture *fixture;
    std::reference_wrapper<b2World> world;

    void swap(Entity& other);

public:
    enum EntityType {
        PLAYER = 0x0001,
        TERRAIN = 0x0002,
        ROCKET = 0x0004,
        EXPLOSION = 0x0008
    };
    const EntityType type;

    Entity(
        b2World& world,
        b2Body *body,
        b2Shape *shape,
        float fixtureDensity,
        EntityType type,
        int collisionMask
    );

    Entity(Entity&& p);
    Entity(const Entity& p) = delete;

    Entity& operator=(Entity&& p);
    Entity& operator=(const Entity& p) = delete;

    ~Entity();

    virtual void update(float deltaTime) = 0;
    b2Vec2 box2dPosition() const;
    Vector2 raylibPosition() const;

    virtual void render() const = 0;

    static b2BodyDef defaultBodyDef();

    static Entity *fromUserDataPointer(uintptr_t pointer);
    static Entity *fromFixture(const b2Fixture *fixture);
    uintptr_t toUserDataPointer();
};
