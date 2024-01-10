#include <raylib.h>
#include <box2d/box2d.h>
#include <array>
#include <sstream>
#include <iomanip>
#include <deque>
#include <optional>
#include "player.hpp"
#include "wall.hpp"
#include "rocket.hpp"
#include "world.hpp"
#include "explosion.hpp"

Explosion *spawnExplosion(b2World& world, b2Vec2 position) {
    return new Explosion(world, position);
}

void explode(b2World& world, std::deque<Explosion *>& explosions, Rocket *rocket, b2Vec2 position) {
    // if it explodes in the air, spawn explosion at its center
    auto explosion = spawnExplosion(world, position);
    explosions.push_back(explosion);
    rocket->collide();
    // DO NOT DELETE ROCKET
    // this is done in the cleanup stage of spawning explosions
}

b2Vec2 getMousePositionInWorld(Camera2D& camera) {
    Vector2 mousePositionScreen = GetMousePosition();
    Vector2 mousePositionCamera = GetScreenToWorld2D(mousePositionScreen, camera);
    return raylibToBox2d(mousePositionCamera);
}

class ContactListener: public b2ContactListener {
    b2World& world;
    std::deque<Explosion *>& explosions;
    Rocket *explodingRocket = nullptr;
    bool shouldCreateExplosion = false;
    b2Vec2 explosionLocation;
public:
    ContactListener(b2World& world, std::deque<Explosion *>& explosions):
        world(world),
        explosions(explosions),
        shouldCreateExplosion(false)
        {}

    void BeginContact(b2Contact *contact) {
        using Type = Entity::EntityType;
        Entity *a = Entity::fromFixture(contact->GetFixtureA());
        Entity *b = Entity::fromFixture(contact->GetFixtureB());

        if (b->type == Type::ROCKET) {
            std::swap(a, b);
        }

        if (a->type == Type::ROCKET && b->type == Type::TERRAIN) {
            // set variables to create explosion later in the frame

            b2WorldManifold manifold;
            contact->GetWorldManifold(&manifold);

            // always spawns explosions in the first contact point
            // maybe fix this later, but it probably doesn't matter
            setupForExplosion(dynamic_cast<Rocket *>(a), manifold.points[0]);
            return;
        }

        if (b->type == Type::EXPLOSION) {
            std::swap(a, b);
        }

        if (a->type == Type::EXPLOSION) {
            switch (b->type) {
                Rocket *r;
                case Type::PLAYER:
                    dynamic_cast<Player *>(b)->feelExplosion(*dynamic_cast<Explosion *>(a));
                    break;
                case Type::ROCKET:
                    r = dynamic_cast<Rocket *>(b);
                    setupForExplosion(r, r->box2dPosition());
                    break;
                case Type::EXPLOSION:
                    break;
                case Type::TERRAIN:
                    // TODO
                    break;
            }
            return;
        }
    }

    void EndContact(b2Contact *contact) {}

    void setupForExplosion(Rocket *rocket, b2Vec2 position) {
        shouldCreateExplosion = true;
        explodingRocket = rocket;
        explosionLocation = position;
    }

    void processQueuedExplosionIfAny() {
        if (shouldCreateExplosion) {
            explode(world, explosions, explodingRocket, explosionLocation);
            shouldCreateExplosion = false;
        }
    }
};

template<typename T>
void write(const T& what, int x, int y, float fontSize, Color color) {
    std::stringstream buf;
    buf << std::fixed << std::setprecision(2) << what;
    DrawText(buf.str().c_str(), x, y, fontSize, color);
}

int main(int argc, char *argv[]) {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Rocket Jump!");
    SetTargetFPS(60);

    float timeSlice = 0;

    b2World world({0.0f, 20.0f});

    auto player = Player(world, b2Vec2{0, 0});
    auto wall = Wall(world, b2Vec2{-10, 10}, b2Vec2{20, 5});
    std::array<Rocket *, Player::maxRockets> rockets;
    int rocketIndex = 0;
    std::deque<Explosion *> explosions;

    ContactListener cl(world, explosions);
    world.SetContactListener(&cl);

    Camera2D camera;
    camera.zoom = 2.0f;
    camera.offset = { screenWidth/2, screenHeight/2 };

    auto updateExplosions = [&]() {
        for (Explosion *explosion: explosions)
            explosion->update(SIMULATION_STEP_INTERVAL);
        while (explosions.size() > 0 && explosions.front()->isOver()) {
            auto endedExplosion = explosions.front();
            explosions.pop_front();
            delete endedExplosion;
        }
    };

    auto updateRockets = [&]() {
        for (Rocket *rocket: rockets) {
            if (rocket != nullptr) {
                rocket->update(SIMULATION_STEP_INTERVAL);
                if (rocket->shouldExplodeByAge())
                    explode(world, explosions, rocket, rocket->box2dPosition());
            }
        }
        cl.processQueuedExplosionIfAny();
        for (Rocket *&rocketRef: rockets) {
            if (rocketRef != nullptr && rocketRef->hasExploded()) {
                delete rocketRef;
                rocketRef = nullptr;
            }
        }
    };

    auto handleInputs = [&]() {
        std::optional<b2Vec2> mousePosInWorld = std::nullopt;
        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
            mousePosInWorld = getMousePositionInWorld(camera);
            Rocket *newRocket = player.shootRocketTowards(mousePosInWorld.value());
            if (newRocket != nullptr) {
                rockets.at(rocketIndex) = newRocket;
                rocketIndex = (rocketIndex + 1) % 3;
            }
        }

        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT)) {
            player.startChargingRecoil();
        }

        if (IsMouseButtonReleased(MouseButton::MOUSE_BUTTON_RIGHT)) {
            if (!mousePosInWorld)
                mousePosInWorld = getMousePositionInWorld(camera);
            player.recoilFrom(mousePosInWorld.value());
        }
    };

    while (!WindowShouldClose()) {
        timeSlice += GetFrameTime();
        if (timeSlice >= SIMULATION_STEP_INTERVAL) {
            world.Step(SIMULATION_STEP_INTERVAL, SIMULATION_VELOCITY_ITER, SIMULATION_POSITION_ITER);
            // process explosions first to allow frame-1-explosion interactions to happen
            updateExplosions();
            player.update(SIMULATION_STEP_INTERVAL);
            updateRockets();

            timeSlice -= SIMULATION_STEP_INTERVAL;
        }

        // TODO more refined camera movement
        // camera.target = player.raylibPosition();

        handleInputs();

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                for (const Explosion *explosion: explosions) {
                    explosion->render();
                }
                for (const Rocket *rocket: rockets) {
                    if (rocket != nullptr)
                        rocket->render();
                }
                player.render();
                wall.render();
            EndMode2D();

#ifdef DEBUG
            write(player.getAmmo(), 0, 0, 32, WHITE);
            write(player.getReload(), 0, 32, 32, WHITE);
#endif
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
