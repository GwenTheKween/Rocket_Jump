#include "player.hpp"

#include <raylib.h>
#include <complex>
#include <numbers>

#include "world.hpp"

// in meters
constexpr float radius = 1.0f;
constexpr float mass = 1.0f;
constexpr float approxArea = radius * radius;
constexpr float density = mass / approxArea;
constexpr float rocketCountMarginBottom = 0.1f;
constexpr float rocketCountTriangleRadius = 0.4f;
constexpr float rocketCountFilledDotRadius = 0.2f;
constexpr float rocketCountEmptyDotRadius = 0.1f;
constexpr int rocketCountSegments = 20;

constexpr float pi = static_cast<float>(std::numbers::pi);

struct PlayerAmmoDotPositions {
    std::array<b2Vec2, Player::maxRockets> dots;
    PlayerAmmoDotPositions() {
        using namespace std::complex_literals;
        constexpr std::complex<float> anglePerDot = (2.0if * pi) / static_cast<float>(Player::maxRockets);

        // up is negative y
        auto dotsCenter = -std::complex<float>(
            0.0f,
            radius + rocketCountMarginBottom + rocketCountTriangleRadius
        );

        for (int i = 0; i < Player::maxRockets; i++) {
            auto complexAngle = pi * -0.5if + static_cast<float>(i) * anglePerDot;
            auto direction = std::exp(complexAngle);
            auto complexPoint = dotsCenter + direction * rocketCountTriangleRadius;
            dots.at(i) = b2Vec2(complexPoint.real(), complexPoint.imag());
        }
    }
};
const static PlayerAmmoDotPositions playerAmmoDotPositions;

b2Body *constructPlayerBody(b2World& world, b2Vec2 spawnPosition) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = std::move(spawnPosition);
    return world.CreateBody(&bodyDef);
}

b2Shape *constructPlayerShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = radius;
    return shape;
}

b2Fixture *constructPlayerFixture(b2Body *body, b2Shape *shape) {
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = density;
    return body->CreateFixture(&fixtureDef);
}

Player::Player(b2World& world, b2Vec2 position):
    Entity(
        world,
        constructPlayerBody(world, position),
        constructPlayerShape(),
        density,
        EntityType::PLAYER,
        EntityType::TERRAIN | EntityType::EXPLOSION
    ),
    rocketReload(Player::rocketReloadTime, [this](){this->doRocketReload();}),
    recoilReload(Player::recoilReloadTime),
    recoilCharge(Player::recoilChargeTime)
{
    recoilReload.setToComplete();
}

// reload: [0, 1]
void drawAmmoDot(b2Vec2 worldPos, float reload) {
    float filledSectorAngle = -90 + reload * 360;
    Vector2 pos = box2dToRaylib(worldPos);
    DrawCircleV(pos, metersToPixels(rocketCountEmptyDotRadius), GRAY);
    DrawCircleSector(
        pos,
        metersToPixels(rocketCountFilledDotRadius),
        -90,
        filledSectorAngle,
        rocketCountSegments,
        WHITE
    );
}

void Player::render() const {
    auto rlPos = raylibPosition();
    DrawCircleV(rlPos, metersToPixels(radius), RED);
    b2Vec2 position = box2dPosition();

    for (int i = 0; i < maxRockets; i++) {
        auto dotRelativePosition = playerAmmoDotPositions.dots.at(i);
        float reloadAmount;
        if (i < rocketAmmo) {
            reloadAmount = 1.0f;
        } else if (i == rocketAmmo) {
            reloadAmount = rocketReload.progress();
        } else {
            reloadAmount = 0.0f;
        }
        drawAmmoDot(position + dotRelativePosition, reloadAmount);
    }
}

void Player::updateTimers(float deltaTime) {
    if (rocketAmmo < Player::maxRockets)
        rocketReload.update(deltaTime);
    if (chargingRecoil) {
        recoilCharge.update(deltaTime);
    } else {
        recoilReload.update(deltaTime);
    }
}

void Player::update(float deltaTime) {
    updateTimers(deltaTime);
}

int Player::getRocketAmmo() const {
    return rocketAmmo;
}

float Player::getRocketReload() const {
    return rocketReload.timeLeft();
}

void Player::doRocketReload() {
    if (rocketAmmo < Player::maxRockets) {
        rocketAmmo++;
        rocketReload.reset();
    }
}

Rocket *Player::shootRocketTowards(b2Vec2 target) {
    if (rocketAmmo <= 0)
        return nullptr;
    auto pos = box2dPosition();
    b2Vec2 direction = target - pos;
    direction.Normalize();
    rocketAmmo--;
    return new Rocket(world, pos, direction);
}

bool Player::startChargingRecoil() {
    if (recoilReload.done()) {
        chargingRecoil = true;
        return true;
    } else {
        return false;
    }
}

void Player::recoilFrom(b2Vec2 origin) {
    if (!chargingRecoil) return;
    chargingRecoil = false;
    recoilCharge.reset();
}

void Player::feelExplosion(const Explosion& explosion) {
    // this direction approximation is only valid since both the player and
    // the explosions are circles.
    // otherwise we'd need to pass in the contact point as well, to determine
    // the direction
    b2Vec2 direction = box2dPosition() - explosion.box2dPosition();
    direction.Normalize();
    body->ApplyForceToCenter(explosion.calculateStrength() * direction, true);
}
