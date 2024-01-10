#include "player.hpp"

#include <raylib.h>
#include <complex>
#include <numbers>

#include "world.hpp"

constexpr float pi = static_cast<float>(std::numbers::pi);

b2Body *constructPlayerBody(b2World& world, b2Vec2 spawnPosition) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.position = std::move(spawnPosition);
    return world.CreateBody(&bodyDef);
}

b2Shape *constructPlayerShape() {
    b2CircleShape *shape = new b2CircleShape();
    shape->m_radius = Player::radius;
    return shape;
}

b2Fixture *constructPlayerFixture(b2Body *body, b2Shape *shape) {
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = Player::density;
    return body->CreateFixture(&fixtureDef);
}

Player::Player(b2World& world, b2Vec2 position):
    Entity(
        world,
        constructPlayerBody(world, position),
        constructPlayerShape(),
        Player::density,
        EntityType::PLAYER,
        EntityType::TERRAIN | EntityType::EXPLOSION
    ) {}

void drawAmmoDot(b2Vec2 worldPos, float reload) {
    float filledSectorAngle = (1 - (reload / Player::reloadTime)) * 360;
    Vector2 pos = box2dToRaylib(worldPos);
    DrawCircleV(pos, metersToPixels(Player::ammoCountEmptyDotRadius), GRAY);
    DrawCircleSector(
        pos,
        metersToPixels(Player::ammoCountFilledDotRadius),
        0,
        filledSectorAngle,
        Player::ammoCountSegments,
        WHITE
    );
}

void Player::render() const {
    using namespace std::complex_literals;
    constexpr std::complex<float> anglePerDot = (2.0if * pi) / static_cast<float>(Player::maxRockets);
    static const float halfRoot3 = 0.5f * sqrtf(3.0f);
    auto rlPos = raylibPosition();
    DrawCircleV(rlPos, metersToPixels(Player::radius), RED);
    b2Vec2 position = box2dPosition();
    std::complex<float> dotsCenter(position.x, position.y);

    // up is negative y
    dotsCenter -= std::complex<float>(0.0f, Player::radius + Player::ammoCountMarginBottom + Player::ammoCountTriangleRadius);

    for (int i = 0; i < Player::maxRockets; i++) {
        auto complexAngle = pi * -0.5if + static_cast<float>(i) * anglePerDot;
        auto direction = std::exp(complexAngle);
        std::complex<float> currentDotCenter = dotsCenter + direction * Player::ammoCountTriangleRadius;
        b2Vec2 dotWorldPosition(currentDotCenter.real(), currentDotCenter.imag());
        float reloadAmount;
        if (i < ammo) {
            reloadAmount = 0.0f;
        } else if (i == ammo) {
            reloadAmount = currentReload;
        } else {
            reloadAmount = Player::reloadTime;
        }
        drawAmmoDot(dotWorldPosition, reloadAmount);
    }
}

void Player::update(float deltaTime) {
    if (ammo >= Player::maxRockets) {
        // keep reload progress frozen while at full ammo
        currentReload = Player::reloadTime;
        return;
    }

    currentReload -= deltaTime;
    if (currentReload <= 0) {
        ammo++;
        currentReload = Player::reloadTime;
    }
}

int Player::getAmmo() const {
    return this->ammo;
}

float Player::getReload() const {
    return this->currentReload;
}

Rocket *Player::shootRocketTowards(b2Vec2 target) {
    if (ammo <= 0)
        return nullptr;
    auto pos = box2dPosition();
    b2Vec2 direction = target - pos;
    direction.Normalize();
    ammo--;
    return new Rocket(world, pos, direction);
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
