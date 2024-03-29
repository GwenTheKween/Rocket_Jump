#include <raylib.h>
#include <box2d/box2d.h>
#include <array>
#include <sstream>
#include <iomanip>
#include <deque>
#include <optional>
#include <set>
#include <fstream>
#include <iostream>

#include "player.hpp"
#include "wall.hpp"
#include "rocket.hpp"
#include "world.hpp"
#include "explosion.hpp"
#include "recoilwave.hpp"
#include "json11.hpp"

void explode(b2World& world, std::deque<Explosion *>& explosions, b2Vec2 position) {
    // if it explodes in the air, spawn explosion at its center
    auto explosion = new Explosion(world, position);
    explosions.push_back(explosion);
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
    std::set<uintptr_t>& explosionsWithinRangeOfPlayer;
    Rocket *explodingRocket = nullptr;
    bool shouldCreateExplosion = false;
    b2Vec2 explosionLocation;
public:
    ContactListener(
        b2World& world,
        std::deque<Explosion *>& explosions,
        std::set<uintptr_t>& explosionsWithinRangeOfPlayer
    ):
        world(world),
        explosions(explosions),
        explosionsWithinRangeOfPlayer(explosionsWithinRangeOfPlayer),
        shouldCreateExplosion(false) {}

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
            Rocket *r;
            switch (b->type) {
            case Type::PLAYER:
                explosionsWithinRangeOfPlayer.insert(a->toUserDataPointer());
                break;
            case Type::ROCKET:
                r = dynamic_cast<Rocket *>(b);
                setupForExplosion(r, r->box2dPosition());
                break;
            case Type::EXPLOSION:
                break;
            case Type::TERRAIN:
                // TODO destructible terrain, maybe on separate entity type
                break;
            case Type::RECOIL_WAVE:
                break;
            }
            return;
        }
    }

    void EndContact(b2Contact *contact) {
        using Type = Entity::EntityType;
        Entity *a = Entity::fromFixture(contact->GetFixtureA());
        Entity *b = Entity::fromFixture(contact->GetFixtureB());

        if (b->type == Type::PLAYER) {
            std::swap(a, b);
        }

        if (a->type == Type::PLAYER && b->type == Type::EXPLOSION) {
            explosionsWithinRangeOfPlayer.erase(b->toUserDataPointer());
        }
    }

    void setupForExplosion(Rocket *rocket, b2Vec2 position) {
        if (rocket->hasExploded()) return;
        rocket->collide();
        shouldCreateExplosion = true;
        explodingRocket = rocket;
        explosionLocation = position;
    }

    void processQueuedExplosionIfAny() {
        if (shouldCreateExplosion) {
            explode(world, explosions, explosionLocation);
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

// Obtained from stack overflow
// https://stackoverflow.com/a/116220
auto readFile(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path.data());
    stream.exceptions(std::ios_base::badbit);

    if (not stream) {
        throw std::ios_base::failure("file does not exist");
    }

    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

std::optional<json11::Json> parseJson(const std::string_view& filename) {
    std::string dump = readFile(filename);
    std::string err;
    json11::Json obj = json11::Json::parse(dump, err);
    if (obj.is_null()) {
        std::cerr << err << std::endl;
        return std::nullopt;
    } else {
        return obj;
    }
}

int main(int argc, char *argv[]) {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // {
    //     std::optional<json11::Json> jv = parseJson(".vscode/c_cpp_properties.json");
    //     std::string jsonObjDump;
    //     jv.value_or(json11::Json::NUL).dump(jsonObjDump);
    //     std::cout << jsonObjDump << std::endl;
    //     return 0;
    // }

    // TODO reset button
    InitWindow(screenWidth, screenHeight, "Rocket Jump!");
    SetTargetFPS(60);

    float timeSlice = 0;

    b2World world({0.0f, 20.0f});

    auto player = Player(world, b2Vec2{0, 0});
    auto wall = Wall(world, b2Vec2{-10, 10}, b2Vec2{20, 5});
    std::array<Rocket *, Player::maxRockets> rockets;
    int rocketIndex = 0;
    std::deque<Explosion *> explosions;
    std::set<uintptr_t> explosionsWithinRangeOfPlayer;
    RecoilWave recoilWave(world);

    ContactListener cl(world, explosions, explosionsWithinRangeOfPlayer);
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

    auto updatePlayer = [&]() {
        for (uintptr_t explosionUserData: explosionsWithinRangeOfPlayer) {
            auto explosion = reinterpret_cast<Explosion *>(explosionUserData);
            player.feelExplosion(*explosion);
        }
        player.update(SIMULATION_STEP_INTERVAL);
    };

    auto updateRockets = [&]() {
        for (Rocket *rocket: rockets) {
            if (rocket != nullptr) {
                rocket->update(SIMULATION_STEP_INTERVAL);
                if (rocket->shouldExplodeByAge())
                    explode(world, explosions, rocket->box2dPosition());
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
            // use cached value if available
            if (!mousePosInWorld)
                mousePosInWorld = getMousePositionInWorld(camera);
            player.recoilFrom(mousePosInWorld.value(), recoilWave);
        }
    };

    while (!WindowShouldClose()) {
        timeSlice += GetFrameTime();
        if (timeSlice >= SIMULATION_STEP_INTERVAL) {
            world.Step(SIMULATION_STEP_INTERVAL, SIMULATION_VELOCITY_ITER, SIMULATION_POSITION_ITER);
            // process explosions first to allow frame-1-explosion interactions to happen
            updateExplosions();
            updatePlayer();
            updateRockets();
            recoilWave.update(SIMULATION_STEP_INTERVAL);

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
                // render recoil wave before player so it hides it spawning in
                recoilWave.render();
                player.render();
                wall.render();
            EndMode2D();

#ifdef DEBUG
            write(player.getRocketAmmo(), 0, 0, 32, WHITE);
            write(player.getRocketReload(), 0, 32, 32, WHITE);
            write(player.getRecoilReload(), 0, 64, 32, WHITE);
#endif
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
