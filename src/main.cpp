#include <raylib.h>
#include <box2d/box2d.h>
#include <array>
#include <sstream>
#include <iomanip>
#include "player.hpp"
#include "wall.hpp"
#include "rocket.hpp"
#include "world.hpp"

class ContactListener: public b2ContactListener {
    void BeginContact(b2Contact *contact) {
    }

    void EndContact(b2Contact *contact) {
    }
};

class ContactFilter: public b2ContactFilter {
    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
        using Types = Entity::EntityType;
        Entity *a = Entity::fromFixture(fixtureA);
        Entity *b = Entity::fromFixture(fixtureB);

        if (b->type == Types::PLAYER) {
            std::swap(a, b);
            std::swap(fixtureA, fixtureB);
        }

        // we dont want rockets colliding with players, as they would
        // push the player and/or explode immediately
        if (a->type == Types::PLAYER && b->type == Types::ROCKET)
            return false;

        if ((a->type == Types::ROCKET || a->type == Types::EXPLOSION)
            && (b->type == Types::ROCKET || b->type == Types::EXPLOSION))
            return false;
        return true;
    }
};

template<typename T>
void write(const T& what, int x, int y, float fontSize, Color color) {
    std::stringstream buf;
    buf << std::fixed << std::setprecision(2) << what;
    DrawText(buf.str().c_str(), x, y, fontSize, color);
}

int main(int argc, char *argv[]) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Rocket Jump!");
    SetTargetFPS(60);

    float timeSlice = 0;
    ContactListener myContactListener;
    ContactFilter myContactFilter;

    b2World world({0.0f, 10.0f});
    world.SetContactListener(&myContactListener);
    world.SetContactFilter(&myContactFilter);

    auto player = Player(world, b2Vec2{0, 0});
    auto wall = Wall(world, b2Vec2{-10, 10}, b2Vec2{20, 5});
    std::array<Rocket *, Player::maxRockets> rockets;
    int rocketIndex = 0;
    // TODO std::queue<Explosion *> explosions

    Camera2D camera;
    camera.zoom = 1.0f;
    camera.offset = { screenWidth/2, screenHeight/2 };
    std::stringstream buf;

    while (!WindowShouldClose()) {
        timeSlice += GetFrameTime();
        if (timeSlice >= SIMULATION_STEP_INTERVAL) {
            world.Step(SIMULATION_STEP_INTERVAL, SIMULATION_VELOCITY_ITER, SIMULATION_POSITION_ITER);
            player.update(SIMULATION_STEP_INTERVAL);
            for (Rocket *rocket: rockets) {
                if (rocket != nullptr)
                    rocket->update(SIMULATION_STEP_INTERVAL);
            }
            // explosions.update(SIMULATION_STEP_INTERVAL);
            timeSlice -= SIMULATION_STEP_INTERVAL;
        }

        // TODO more refined camera movement
        // camera.target = player.raylibPosition();

        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
            Vector2 mousePositionScreen = GetMousePosition();
            Vector2 mousePositionCamera = GetScreenToWorld2D(mousePositionScreen, camera);
            b2Vec2 mousePositionWorld = raylibToBox2d(mousePositionCamera);
            Rocket *newRocket = player.shootRocketTowards(mousePositionWorld);
            if (newRocket != nullptr) {
                rockets.at(rocketIndex) = newRocket;
                rocketIndex = (rocketIndex + 1) % 3;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                player.render();
                wall.render();
                for (const Rocket *rocket: rockets) {
                    if (rocket != nullptr)
                        rocket->render();
                }
                // rocket.render();
            EndMode2D();

            write(player.getAmmo(), 0, 0, 32, WHITE);
            write(player.getReload(), 0, 32, 32, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
