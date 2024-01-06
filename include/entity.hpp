#pragma once

#include <box2d/box2d.h>
#include <functional>

#include <raymath.h>

class Entity {
    b2Body *body;
    b2Shape *shape;
    b2Fixture *fixture;
    std::reference_wrapper<b2World> world;

    void swap(Entity& other);

public:
    Entity(b2World& world, b2Body *body, b2Shape *shape, float fixtureDensity);

    Entity(Entity&& p);
    Entity(const Entity& p) = delete;

    Entity& operator=(Entity&& p);
    Entity& operator=(const Entity& p) = delete;

    ~Entity();

    b2Vec2 box2dPosition() const;
    Vector2 raylibPosition() const;

    static b2BodyDef defaultBodyDef();
};