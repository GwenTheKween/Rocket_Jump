#include "recoilwave.hpp"

#include <cmath>
#include <numbers>

#include "world.hpp"

constexpr float speed = 30.0f;
constexpr float hitboxOffset = 1.0f;
constexpr float hitboxWidth = 1.0f;
constexpr float hitboxDepth = 0.2f;
constexpr float linearDamping = 10.0f;

b2Body *constructRecoilWaveBody(b2World& world) {
    b2BodyDef bodyDef = Entity::defaultBodyDef();
    bodyDef.fixedRotation = true;
    bodyDef.gravityScale = 0.0f;
    bodyDef.enabled = false;
    bodyDef.linearDamping = linearDamping;
    return world.CreateBody(&bodyDef);
}

b2Shape *constructRecoilWaveShape() {
    b2PolygonShape *shape = new b2PolygonShape();
    shape->SetAsBox(hitboxDepth * 0.5f, hitboxWidth * 0.5f);
    for (int i = 0; i < 4; i++)
        shape->m_vertices[i].x -= hitboxOffset;
    return shape;
}

RecoilWave::RecoilWave(b2World& world)
    : Entity(
        world,
        constructRecoilWaveBody(world),
        constructRecoilWaveShape(),
        1.0f,
        Entity::EntityType::RECOIL_WAVE,
        0
    ),
    duration(RecoilWave::lifetime, [this](){this->disable();})
{
    duration.setToComplete();
}


void RecoilWave::moveTo(b2Vec2 position, b2Vec2 direction) {
    direction.Normalize();
    float angle = std::atan2(direction.y, direction.x);
    body->SetEnabled(true);
    body->SetTransform(position, angle);
    body->SetLinearVelocity(-speed * direction);
    duration.reset();
}

void RecoilWave::update(float deltaTime) {
    duration.update(deltaTime);
}

void RecoilWave::render() const {
    if (!body->IsEnabled()) return;
    Vector2 arcCenter = box2dToRaylib(box2dPosition());
    static float arcRadius = metersToPixels(hitboxOffset);
    float angle = 180.0f + body->GetAngle() * (180.0f / std::numbers::pi);
    Color color = WHITE;
    color.a = duration.timeLeft() * 255;
    DrawCircleSectorLines(arcCenter, arcRadius, angle - 30, angle + 30, 90, color);

#ifdef DEBUG
    b2PolygonShape *shape = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
    b2Vec2 center = box2dPosition();
    float s = std::sin(body->GetAngle());
    float c = std::cos(body->GetAngle());

    for (int i = 1; i < 5; i++) {
        b2Vec2 prevVert = shape->m_vertices[i-1];
        b2Vec2 currVert = shape->m_vertices[i % 4];

        b2Vec2 rotPrevVert = b2Vec2(c*prevVert.x - s*prevVert.y, s*prevVert.x + c*prevVert.y);
        b2Vec2 rotCurrVert = b2Vec2(c*currVert.x - s*currVert.y, s*currVert.x + c*currVert.y);

        Vector2 prevPx = box2dToRaylib(center + rotPrevVert);
        Vector2 currPx = box2dToRaylib(center + rotCurrVert);
        DrawLineEx(prevPx, currPx, 1.0f, RED);
    }
#endif
}

void RecoilWave::disable() {
    body->SetEnabled(false);
}
