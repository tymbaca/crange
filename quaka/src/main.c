#include <stdlib.h>
#include <stdio.h>
#include "arena.h"

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct Player {
    Vector3 position;
    Vector3 direction;
    Camera3D *camera;
} Player;

void player_camera_update(Player *p) {
    p->camera->position = p->position;
    p->camera->target = Vector3Add(p->position, p->direction);
}

void player_update(Player *p) {
    Vector2 mouse = GetMouseDelta();
    p->direction = Vector3RotateByAxisAngle(p->direction, (Vector3){.y = 1}, -mouse.x*0.01);
    p->direction = Vector3RotateByAxisAngle(p->direction, (Vector3){.x = 1}, mouse.y*0.01);

    player_camera_update(p);
}

void debug(Camera3D camera) {
    printf("camera: pos: {%f, %f, %f}, target: {%f, %f, %f}\n", 
           camera.position.x, camera.position.y, camera.position.z,
           camera.target.x, camera.target.y, camera.target.z
    );
    Vector2 mouse = GetMouseDelta();
    printf("mouse delta: {%f, %f}\n", mouse.x, mouse.y);
}

void draw_gizmo(float size) {
    DrawLine3D((Vector3){.x = -size/2}, (Vector3){.x = size/2}, RED);
    DrawSphere((Vector3){.x = size/2}, 0.07, RED);
    DrawLine3D((Vector3){.y = -size/2}, (Vector3){.y = size/2}, GREEN);
    DrawSphere((Vector3){.y = size/2}, 0.07, GREEN);
    DrawLine3D((Vector3){.z = -size/2}, (Vector3){.z = size/2}, BLUE);
    DrawSphere((Vector3){.z = size/2}, 0.07, BLUE);
}

int main() {
    InitWindow(800, 600, "template");
    SetTargetFPS(60);

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, -10.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    Player player = {.camera = &camera, .direction = (Vector3){.x = 1}, .position = (Vector3){ 0.0f, 10.0f, -10.0f }};
    DisableCursor();

    while (!WindowShouldClose()) {
        debug(camera);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        
        player_update(&player);
        
        DrawGrid(10, 1);
        draw_gizmo(10);
        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();
    }

    return 0;
}
