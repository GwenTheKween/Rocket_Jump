#include <raylib.h>
#include <box2d/box2d.h>
#include "player.hpp"
#include "wall.hpp"
#include "world.hpp"

class ContactListener: public b2ContactListener {
    void BeginContact(b2Contact *contact) {
        ;
    }

    void EndContact(b2Contact *contact) {
    }
};

int main(int argc, char *argv[]) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60);
    float timeSlice = 0;
    ContactListener myContactListener;

    b2World world({0.0f, 10.0f});
    world.SetContactListener(&myContactListener);

    auto player = Player(world, b2Vec2{0, 0});
    auto wall = Wall(world, b2Vec2{-10, 10}, b2Vec2{20, 5});
    Camera2D camera;
    camera.zoom = 1.0f;
    camera.offset = { screenWidth/2, screenHeight/2 };

    while (!WindowShouldClose()) {
        // TODO: Update your variables here
        timeSlice += GetFrameTime();
        if (timeSlice >= SIMULATION_STEP_INTERVAL) {
            world.Step(SIMULATION_STEP_INTERVAL, SIMULATION_VELOCITY_ITER, SIMULATION_POSITION_ITER);
            timeSlice -= SIMULATION_STEP_INTERVAL;
        }

        // camera.target = player.raylibPosition();

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                player.render();
                wall.render();
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}