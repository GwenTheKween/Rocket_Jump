#pragma once

#include <box2d/box2d.h>
#include <raylib.h>
#include <utility>

#include "entity.hpp"
#include "rocket.hpp"
#include "timer.hpp"
#include "recoilwave.hpp"

class Player: public Entity {
private:
    Timer rocketReload;
    Timer recoilReload;
    Timer recoilCharge;

    bool chargingRecoil = false;
    int rocketAmmo = Player::maxRockets;

    void updateTimers(float deltaTime);
    void doRocketReload();
public:
    static constexpr int maxRockets = 3;
    static constexpr float rocketReloadTime = 2.0f;

    static_assert(Player::rocketReloadTime > Rocket::lifetime);

    // TODO draw rocket count UI in a fixed position on the screen
    // TODO fix long interactions between player and explosions

    Player(b2World& world, b2Vec2 position);
    void update(float deltaTime);
    void render() const;
    int getRocketAmmo() const;
    float getRocketReload() const;
    float getRecoilReload() const;
    Rocket *shootRocketTowards(b2Vec2 target);
    bool startChargingRecoil();
    void recoilFrom(b2Vec2 origin, RecoilWave& recoilWave);
    void feelExplosion(const Explosion& explosion);
};
