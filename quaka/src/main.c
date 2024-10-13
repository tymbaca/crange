#include <stdlib.h>
#include <stdio.h>
#include "arena.h"

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "collider.c"

void printf_quaternion(Quaternion q) {
    /* printf("Quaternion: x = %f, y = %f, z = %f, w = %f\n", q.x, q.y, q.z, q.w); */
    Vector3 euler = QuaternionToEuler(q);
    printf("Euler: x = %f, y = %f, z = %f\n", euler.x, euler.y, euler.z);
}

const Vector3 Vector3UP = {.y = 1};
const Vector3 Vector3DOWN = {.y = -1};

Vector3 Vector3MultiplyValue(Vector3 v, float mul) {
    return Vector3Multiply(v, (Vector3){.x = mul, .y = mul, .z = mul});
}

typedef struct Direction {
    float pitch; // radians, up/down [-89°; 89°]
    float yaw; // radians, left/right [0;360)
} Direction;

float fwarp(float val, float max) {
    return fmodf(fmodf(val, max) + max, max);
}

Direction direction_yaw(Direction dir, float angle) {
    dir.yaw += angle;
    dir.yaw = fwarp(dir.yaw, 360*DEG2RAD);
    return dir;
}

Direction direction_pitch(Direction dir, float angle) {
    dir.pitch += angle;
    dir.pitch = Clamp(dir.pitch, -89*DEG2RAD, 89*DEG2RAD);
    return dir;
}

Direction direction_apply(Direction dir, float pitch, float yaw) {
    dir = direction_pitch(dir, pitch);
    dir = direction_yaw(dir, yaw);
    return dir;
}

Vector3 DirectionToVector3(Direction dir) {
    Vector3 dir_vec = {.x = 1};
    dir_vec = Vector3RotateByAxisAngle(dir_vec, (Vector3){.z = 1}, -dir.pitch);
    dir_vec = Vector3RotateByAxisAngle(dir_vec, (Vector3){.y = 1}, dir.yaw);
    dir_vec = Vector3Normalize(dir_vec);
    return dir_vec;
}

// UP is {0,1,0} (wtf)
Quaternion Vector3ToQuaternion(Vector3 vec) {
    Quaternion q = QuaternionFromMatrix(MatrixLookAt((Vector3){0}, vec, Vector3UP));
    q = QuaternionMultiply(q, QuaternionFromEuler(0, -90*DEG2RAD, 0));

    Vector3 axis = {0};
    float angle = 0;
    QuaternionToAxisAngle(q, &axis, &angle);
    axis = Vector3RotateByAxisAngle(axis, (Vector3){.y=1}, -90*DEG2RAD);

    return QuaternionFromAxisAngle(axis, -angle);
}

Matrix MatrixFromDirection(Direction dir) {
    Vector3 vec = DirectionToVector3(dir);
    Quaternion q = Vector3ToQuaternion(vec);
    return QuaternionToMatrix(q);
}

typedef struct FValue {
    float max, val;
} FValue;

typedef struct IValue {
    int max, val;
} IValue;

#define MAX_WEAPON_MODELS 1024
Model weapon_model_by_type[MAX_WEAPON_MODELS];

typedef enum WaeponType {
    WEAPON_NONE,
    WEAPON_RAILGUN,
} WaeponType;

typedef struct Weapon {
    WaeponType type;
    Vector3 barrel;
    IValue ammo;
} Weapon;

const Weapon NEW_RAILGUN = {
    .type = WEAPON_RAILGUN,
    .ammo = {.max = 10, .val = 10},
};

typedef struct Player {
    FValue speed;
    Vector3 position;
    Direction direction;
    Camera3D *camera;
    Weapon weapon;
    ColliderBox collider;
} Player;

void player_camera_update(Player *p) {
    if (p->camera == NULL) return;
    p->camera->position = p->position;
    p->camera->target = Vector3Add(p->position, DirectionToVector3(p->direction));
}

const float SENSITIVITY = 0.003;

void player_update_diraction(Player *p) {
    Vector2 mouse = GetMouseDelta();
    /* printf("mouse: {%f, %f}\n", mouse.x, mouse.y); */
    p->direction = direction_apply(p->direction, mouse.y * SENSITIVITY, -mouse.x * SENSITIVITY);
}

Vector3 get_direction_forward(Direction dir) {
    dir.pitch = 0; // we don't need pitch
    return DirectionToVector3(dir);
}

Vector3 get_direction_backward(Direction dir) {
    return Vector3Negate(get_direction_forward(dir));
}

Vector3 get_direction_left(Direction dir) {
    dir.pitch = 0; // we don't need pitch
    dir = direction_yaw(dir, 90*DEG2RAD);
    return DirectionToVector3(dir);
}

Vector3 get_direction_right(Direction dir) {
    return Vector3Negate(get_direction_left(dir));
}

void player_update_position(Player *p) {
    Vector3 mov = {0};
    if (IsKeyDown(KEY_W)) {
        mov = Vector3Add(mov, get_direction_forward(p->direction));
    }
    if (IsKeyDown(KEY_S)) {
        mov = Vector3Add(mov, get_direction_backward(p->direction));
    }
    if (IsKeyDown(KEY_A)) {
        mov = Vector3Add(mov, get_direction_left(p->direction));
    }
    if (IsKeyDown(KEY_D)) {
        mov = Vector3Add(mov, get_direction_right(p->direction));
    }
    if (IsKeyDown(KEY_R)) {
        mov = Vector3Add(mov, Vector3UP);
    }
    if (IsKeyDown(KEY_F)) {
        mov = Vector3Add(mov, Vector3DOWN);
    }

    p->position = Vector3Add(p->position, Vector3MultiplyValue(mov, p->speed.val));
}

void player_update(Player *p) {
    player_update_position(p);
    player_update_diraction(p);
    player_camera_update(p);
}

/* #define WEAPON_OFFSET (Vector3){.x = 0.1, .y = -0.2, .z = 0.2} */

void player_weapon_draw(Player *p) {
    Model model = weapon_model_by_type[p->weapon.type];
    Vector3 pos = p->position;

    model.transform = MatrixFromDirection(p->direction);
    /* DrawModelEx(model, pos, axis, angle, Vector3One(), WHITE); */
    DrawModel(model, pos, .1, WHITE);
}

void player_draw(Player *p) {
    player_weapon_draw(p);
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

void load_waepon_models() {
    weapon_model_by_type[WEAPON_RAILGUN] = LoadModel("assets/railgun.obj");
}

Direction debug_dir = {0};
void debug_direction() {
    if (IsKeyDown(KEY_LEFT)) {
        debug_dir = direction_yaw(debug_dir, 0.04);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        debug_dir = direction_yaw(debug_dir, -0.04);
    }
    if (IsKeyDown(KEY_UP)) {
        debug_dir = direction_pitch(debug_dir, -0.04);
    }
    if (IsKeyDown(KEY_DOWN)) {
        debug_dir = direction_pitch(debug_dir, 0.04);
    }
    
    // Draw direction
    DrawCircle3D((Vector3){0}, 1, (Vector3){.x = 1}, 90, PURPLE);
    Vector3 vec = DirectionToVector3(debug_dir);
    DrawLine3D((Vector3){0}, vec, PURPLE);
    DrawSphere(vec, 0.02, PURPLE);

    Model railgun_debug = weapon_model_by_type[WEAPON_RAILGUN];
    /* railgun_debug.transform = MatrixLookAt((Vector3){0}, vec, Vector3UP); */
    Quaternion q = QuaternionFromMatrix(MatrixLookAt((Vector3){0}, vec, Vector3UP));
    q = QuaternionMultiply(q, QuaternionFromEuler(0, -90*DEG2RAD, 0));

    /* // Draw it's angle axis */
    /* Quaternion q = QuaternionFromEuler(0, debug_dir.yaw, -debug_dir.pitch); */

    Vector3 axis = {0};
    float angle = 0;
    QuaternionToAxisAngle(q, &axis, &angle);
    axis = Vector3RotateByAxisAngle(axis, (Vector3){.y=1}, -90*DEG2RAD);
    DrawLine3D((Vector3){0}, axis, YELLOW);
    printf("angle: %f, axis: {%f, %f, %f}\n", angle, axis.x, axis.y, axis.z);

    q = QuaternionFromAxisAngle(axis, -angle);

    railgun_debug.transform = QuaternionToMatrix(q);

    /* DrawModelEx(railgun_debug, (Vector3){0}, axis, -angle*RAD2DEG, Vector3One(), WHITE); */
    DrawModel(railgun_debug, (Vector3){0}, 1, RED);
}

#define Array(type)                                             \
typedef struct type##Array {                                    \
    size_t len;                                                 \
    size_t cap;                                                 \
    type *data;                                                 \
} type##Array;                                                  \
                                                                \
type##Array type##Array_create(size_t size) {                   \
    type *data = malloc(sizeof(type)*size);                     \
    return (type##Array){.len = 0, .cap = size, .data = data};  \
}                                                               \
type* type##Array_append(type##Array *array, type item) {       \
    if (array->len == array->cap) return NULL;                  \
    array->data[array->len] = item;                             \
    type *ptr = &array->data[array->len];                       \
    array->len++;                                               \
    return ptr;                                                 \
}                                                               \
                                                                \
void type##Array_destroy(type##Array array) {                   \
    free(array.data);                                           \
}

typedef struct Enemy {
    FValue speed;
    Vector3 position;
    Direction direction;
} Enemy;

Array(Enemy)

typedef struct LevelBlock {
    
} LevelBlock;

Array(LevelBlock)

typedef struct World {
    Player *player;
    EnemyArray enemies;
    LevelBlockArray blocks;
} World;

World World_create(Player *player) {
    World world;
    world.player = player;
    world.enemies = EnemyArray_create(1024);
    world.blocks = LevelBlockArray_create(1024);
    return world;
}

void World_update(World *world) {
    player_update(world->player);
}

void World_draw(World *world) {
    player_draw(world->player);

    DrawGrid(10, 1);
    draw_gizmo(10);
    debug_direction();
}

void World_draw2D(World *world) {
}

void World_destroy(World *world) {
    EnemyArray_destroy(world->enemies);
    LevelBlockArray_destroy(world->blocks);
}

int main() {
    /* printf("%f\n", fwarp(359.999, 360)); */
    /* return 0; */


    InitWindow(1200, 800, "template");
    SetTargetFPS(60);

    load_waepon_models();

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, -10.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 65.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;

    Player player = {
        .camera = &camera, 
        /* .direction = (Vector3){.x = 0, .y = -1, .z = 1},  */

        .position = (Vector3){ 0.0f, 10.0f, -10.0f }, 
        .speed = {.max = 1, .val = 0.1},
        .weapon = NEW_RAILGUN,
    };

    World world = World_create(&player);
    /* EnemyArray_append(world.enemies, (Enemy){ */
    /*     .position */
    /* }) */

    DisableCursor();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        
        World_draw(&world);
        World_update(&world);

        EndMode3D();

        World_draw2D(&world);

        DrawFPS(10, 10);
        EndDrawing();
    }

    return 0;
}
