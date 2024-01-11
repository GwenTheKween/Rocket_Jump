#pragma once

#include <box2d/box2d.h>
#include <raylib.h>
#include <utility>

#include "entity.hpp"
#include "rocket.hpp"
#include "timer.hpp"

class Player: public Entity {
private:
    Timer rocketReload;
    Timer recoilReload;
    Timer recoilCharge;
    // float rocketReload = 0.0f;
    // float recoilReload = 0.0f;
    // float recoilCharge = 0.0f;
    // TODO create a class to manage all these progress bars

    bool chargingRecoil = false;
    int rocketAmmo = Player::maxRockets;

    void updateTimers(float deltaTime);
    void doRocketReload();
public:
    static constexpr float rocketReloadTime = 2.0f;
    static constexpr float recoilChargeTime = 2.0f;
    static constexpr float recoilReloadTime = 1.5f;
    static constexpr int maxRockets = 3;

    // TODO is this really necessary? might detract from fast gameplay
    static_assert(Player::rocketReloadTime > Rocket::lifetime);

    // TODO draw rocket count UI in a fixed position on the screen
    // TODO fix long interactions between player and explosions

    Player(b2World& world, b2Vec2 position);
    void update(float deltaTime);
    void render() const;
    int getRocketAmmo() const;
    float getRocketReload() const;
    Rocket *shootRocketTowards(b2Vec2 target);
    bool startChargingRecoil();
    void recoilFrom(b2Vec2 origin);
    void feelExplosion(const Explosion& explosion);
};
