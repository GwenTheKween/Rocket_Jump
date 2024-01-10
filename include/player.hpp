#pragma once

#include <box2d/box2d.h>
#include <raylib.h>
#include <utility>

#include "entity.hpp"
#include "rocket.hpp"

class Player: public Entity {
private:
    float currentReload;
    float recoilCharge;
    float recoilCooldown;
    bool chargingRecoil;
    int ammo;
public:
    // TODO is this really necessary? might detract from fast gameplay
    static constexpr float rocketReloadTime = 2.0f;
    static constexpr int maxRockets = 3;

    static_assert(Player::rocketReloadTime > Rocket::lifetime);

    // TODO secondary shot with (maybe) chargeable effect
    // TODO draw rocket count UI in a fixed position on the screen
    // TODO fix long interactions between player and explosions

    Player(b2World& world, b2Vec2 position);
    void update(float deltaTime);
    void render() const;
    int getAmmo() const;
    float getReload() const;
    Rocket *shootRocketTowards(b2Vec2 target);
    void startChargingRecoil();
    void recoilFrom(b2Vec2 origin);
    void feelExplosion(const Explosion& explosion);
};
