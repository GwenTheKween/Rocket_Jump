#include "world.hpp"

Vector2 box2dToRaylib(b2Vec2 position) {
    return { position.x * PIXELS_PER_METER, position.y * PIXELS_PER_METER };
}

b2Vec2 raylibToBox2d(Vector2 position) {
    return { position.x * METERS_PER_PIXEL, position.y * METERS_PER_PIXEL };
}