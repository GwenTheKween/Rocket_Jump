#pragma once

#include <box2d/box2d.h>
#include <raylib.h>
#include <utility>

#include "entity.hpp"
#include "rocket.hpp"

class Player: public Entity {
private:
    float currentReload;
    int ammo;
public:
    // in meters
    static constexpr float radius = 1;
    static constexpr float mass = 1;
    static constexpr float approxArea = radius * radius;
    static constexpr float density = mass / approxArea;
    static constexpr float reloadTime = 2;
    static constexpr int maxRockets = 3;
    static_assert(Player::reloadTime > Rocket::lifetime);

    Player(b2World& world, b2Vec2 position);
    void update(float deltaTime);

    void render() const;
    Rocket *shootRocketTowards(b2Vec2 target);
};
