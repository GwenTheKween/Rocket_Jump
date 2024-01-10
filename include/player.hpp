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
    static constexpr float radius = 1.0f;
    static constexpr float mass = 1.0f;
    static constexpr float approxArea = radius * radius;
    static constexpr float density = mass / approxArea;
    static constexpr float reloadTime = 2.0f;
    static constexpr int maxRockets = 3;
    static constexpr float ammoCountMarginBottom = 0.1f;
    static constexpr float ammoCountTriangleRadius = 0.4f;
    static constexpr float ammoCountFilledDotRadius = 0.2f;
    static constexpr float ammoCountEmptyDotRadius = 0.1f;
    static constexpr int ammoCountSegments = 20;

    // TODO secondary shot with (maybe) chargeable effect
    // TODO draw rocket count UI in a fixed position on the screen

    // TODO is this really necessary? might detract from fast gameplay
    static_assert(Player::reloadTime > Rocket::lifetime);

    Player(b2World& world, b2Vec2 position);
    void update(float deltaTime);
    void render() const;
    int getAmmo() const;
    float getReload() const;
    Rocket *shootRocketTowards(b2Vec2 target);
    void feelExplosion(const Explosion& explosion);
};
