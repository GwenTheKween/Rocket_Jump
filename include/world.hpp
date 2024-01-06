#pragma once

#include <raymath.h>
#include <box2d/box2d.h>

constexpr float SIMULATION_STEP_INTERVAL = 1.0f / 60.0f;
constexpr float SIMULATION_POSITION_ITER = 10;
constexpr float SIMULATION_VELOCITY_ITER = 10;

constexpr float PIXELS_PER_METER = 10;
constexpr float METERS_PER_PIXEL = 1.0f / PIXELS_PER_METER;

constexpr float pixelsToMeters(float pixels) {
    return METERS_PER_PIXEL * pixels;
}

constexpr float metersToPixels(float meters) {
    return PIXELS_PER_METER * meters;
}

Vector2 box2dToRaylib(b2Vec2 position);
b2Vec2 raylibToBox2d(Vector2 position);