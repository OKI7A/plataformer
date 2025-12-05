#include "raylib.h"
#include <math.h>

#define VIRTUAL_W 800
#define VIRTUAL_H 600

#define ENEMY_COUNT 5
#define PLAT_COUNT 12
#define GRAVITY 1000.0f

typedef struct Player {
    Rectangle rect;
    Vector2 velocity;
    bool onGround;
} Player;

typedef struct Enemy {
    Rectangle rect;
} Enemy;

// ---------------- FONDO ----------------

void DrawKitchenBackground() {
    DrawRectangle(0, 0, VIRTUAL_W, VIRTUAL_H, (Color){245, 240, 230, 255});

    int tileSize = 70;
    for (int x = 0; x < VIRTUAL_W; x += tileSize) {
        for (int y = VIRTUAL_H/2; y < VIRTUAL_H; y += tileSize) {
            Color tileColor = ((x/tileSize + y/tileSize) % 2 == 0)
                ? (Color){225,225,225,255}
                : (Color){205,205,205,255};

            DrawRectangle(x, y, tileSize, tileSize, tileColor);
            DrawRectangleLines(x, y, tileSize, tileSize, (Color){160,160,160,255});
        }
    }

    DrawRectangle(250, 60, 300, 40, (Color){130,110,85,255});
    DrawRectangleLines(250, 60, 300, 40, (Color){90,70,50,255});

    DrawRectangle(600, 70, 160, 120, (Color){180,220,255,255});
    DrawRectangleLines(600, 70, 160, 120, (Color){120,160,200,255});
}

// ---------------- PLATAFORMAS ----------------

void DrawPlatform(Rectangle p) {
    if (p.y > VIRTUAL_H - 30) {
        DrawRectangleRec(p, (Color){150,140,120,255});
        return;
    }

    if (p.height <= 14) {
        DrawRectangleRec(p, (Color){160,130,90,255});
        return;
    }

    if (p.height <= 20) {
        DrawRectangleRec(p, (Color){180,150,110,255});
        return;
    }

    DrawRectangleRec(p, GRAY);
}

// ---------------- MAIN ----------------

int main() {

    // ✅ FULLSCREEN REAL SIN BARRAS
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Fred the Bread");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);

    // ------------- JUGADOR -------------

    Player player = {
        .rect = (Rectangle){100, 300, 32, 48},
        .velocity = {0, 0},
        .onGround = false
    };

    const float MOVE_SPEED = 210;
    const float JUMP_FORCE = 310;

    // ------------- PLATAFORMAS -------------

    Rectangle platforms[PLAT_COUNT] = {
        {0, VIRTUAL_H - 20, VIRTUAL_W, 20},
        {80, 540, 100, 20},
        {220, 510, 150, 18},
        {420, 480, 80, 16},
        {530, 450, 140, 16},
        {700, 410, 70, 15},
        {560, 365, 90, 17},
        {490, 320, 50, 17},
        {320, 300, 120, 17},
        {240, 250, 50, 13},
        {100, 205, 120, 13},
        {500, 210, 120, 17}
    };

    // ------------- ENEMIGOS ESTÁTICOS -------------

    Enemy enemies[ENEMY_COUNT] = {
        {{260, 490, 28, 16}},
        {{450, 460, 28, 16}},
        {{330, 280, 28, 16}},
        {{110, 185, 28, 16}},
        {{560, 350, 28, 16}}
    };

    // ================= GAME LOOP =================

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // -------- MOVIMIENTO --------
        float dx = 0;
        if (IsKeyDown(KEY_A)) dx -= 1;
        if (IsKeyDown(KEY_D)) dx += 1;
        player.velocity.x = dx * MOVE_SPEED;

        // -------- SALTO --------
        if (IsKeyPressed(KEY_SPACE) && player.onGround) {
            player.velocity.y = -JUMP_FORCE;
            player.onGround = false;
        }

        // -------- GRAVEDAD --------
        player.velocity.y += GRAVITY * dt;

        // -------- MOVER --------
        player.rect.x += player.velocity.x * dt;
        player.rect.y += player.velocity.y * dt;

        // -------- COLISIÓN CON PLATAFORMAS --------
        player.onGround = false;
        for (int i = 0; i < PLAT_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, platforms[i]) &&
                player.velocity.y >= 0) {

                player.rect.y = platforms[i].y - player.rect.height;
                player.velocity.y = 0;
                player.onGround = true;
            }
        }

        // -------- COLISIÓN CON ENEMIGOS --------

        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, enemies[i].rect)) {
                player.rect.x = 100;
                player.rect.y = 300;
                player.velocity = (Vector2){0, 0};
            }
        }

        // -------- DIBUJO EN TEXTURA --------
        BeginTextureMode(target);
        ClearBackground(BLACK);

        DrawKitchenBackground();

        for (int i = 0; i < PLAT_COUNT; i++)
            DrawPlatform(platforms[i]);

        DrawRectangleRec(player.rect, BLUE);

        for (int i = 0; i < ENEMY_COUNT; i++) {
            DrawRectangleRec(enemies[i].rect, (Color){20, 180, 60, 255});
            DrawCircle(enemies[i].rect.x + 6, enemies[i].rect.y + 6, 3, DARKGREEN);
        }

        EndTextureMode();

        
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
            target.texture,
            (Rectangle){0, 0, VIRTUAL_W, -VIRTUAL_H},
            (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
            (Vector2){0, 0},
            0,
            WHITE
        );

        EndDrawing();
    }

    //faltan imagenes

    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}