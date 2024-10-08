#include <stdlib.h>
#include <stdio.h>
#include "raylib.h"
#include "arena.h"


typedef enum {
    None,
    Player,
    Enemy,
} EntityType;

typedef struct {
    float max, val;
} Value;

typedef struct {
    EntityType type;
    Vector3 position;
    Value health;
    Value speed;
} Entity;

typedef struct {
    Camera3D *camera;
    Entity entities[1024];
    size_t entities_len;
} World;

size_t world_add_entity(World *world, Entity entity) {
    size_t id = world->entities_len;
    world->entities[id] = entity;
    world->entities_len++;
    return id;
}

Entity world_get_entity(World *world, size_t id) {
    return world->entities[id];
}

void player_update(World *w, Entity *e) {
    if (IsKeyDown(KEY_W)) {
        e->position.z += e->speed.val;
    }
    if (IsKeyDown(KEY_S)) {
        e->position.z -= e->speed.val;
    }
    if (IsKeyDown(KEY_A)) {
        e->position.x += e->speed.val;
    }
    if (IsKeyDown(KEY_D)) {
        e->position.x -= e->speed.val;
    }
}

void enemy_update(World *w, Entity *e) {
}

void world_update(World *world) {
    for (int i = 0; i < world->entities_len; i++) {
        Entity *e = &world->entities[i];
        switch (e->type) {
        case Player:
            player_update(world, e);
            break;
        case Enemy:
            enemy_update(world, e);
            break;
        case None: break;
        }
    }
}

void world_draw(World *world) {
    DrawGrid(10, 1.0f);

    for (int i = 0; i < world->entities_len; i++) {
        Entity e = world->entities[i];
        switch (e.type) {
        case Player:
            DrawCube(e.position, 2.0f, 2.0f, 2.0f, BLUE);
            break;
        case Enemy:
            DrawCube(e.position, 1.5f, 1.5f, 1.5f, RED);
            break;
        case None: break;
        }
    }
    /* DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED); */
    /* DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON); */
}

int main() {
    InitWindow(800, 600, "template");
    Arena *arena = arena_create();
    

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, -10.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
    SetTargetFPS(60);

    World world = {0};
    world.camera = &camera;
    Entity player = {
        .type = Player,
        .position = {0},
        .health = {.max = 100, .val = 100},
        .speed =  {.max = 1, .val = 0.5},
    };

    Entity enemy = {
        .type = Enemy,
        .position = {0},
        .health = {.max = 100, .val = 100},
        .speed =  {.max = 1, .val = 1},
    };

    size_t player_id = world_add_entity(&world, player);
    world_add_entity(&world, enemy);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        world_draw(&world);

        world_update(&world);
        Entity player = world_get_entity(&world, player_id);
        camera.target = player.position;

        EndMode3D();
        EndDrawing();
    }

    return 0;
}
