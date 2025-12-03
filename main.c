#include "raylib.h"
#include <math.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define PLAT_COUNT 12   // ahora hay más plataformas
#define GRAVITY 900.0f

typedef struct Player {
    Rectangle rect;
    Vector2 velocity;
    bool onGround;
} Player;

// -------------------------
// DIBUJAR FONDO DEL CUARTO
// -------------------------
void DrawKitchenBackground() {
    // Pared color crema
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){245, 240, 230, 255});

    // Azulejos en mitad inferior
    int tileSize = 48;
    for (int x = 0; x < SCREEN_W; x += tileSize) {
        for (int y = SCREEN_H/2; y < SCREEN_H; y += tileSize) {
            Color tileColor = ((x/tileSize + y/tileSize) % 2 == 0)
                ? (Color){225,225,225,255}
                : (Color){205,205,205,255};

            DrawRectangle(x, y, tileSize, tileSize, tileColor);
            DrawRectangleLines(x, y, tileSize, tileSize, (Color){160,160,160,255});
        }
    }

    // Refrigerador
    DrawRectangle(40, SCREEN_H - 280, 140, 260, (Color){210, 210, 225, 255});
    DrawRectangle(50, SCREEN_H - 200, 22, 60, GRAY);
    DrawRectangle(50, SCREEN_H - 260, 22, 40, GRAY);

    // Alacena superior
    DrawRectangle(250, 60, 300, 40, (Color){130,110,85,255});
    DrawRectangleLines(250, 60, 300, 40, (Color){90,70,50,255});

    // Objetos arriba
    DrawRectangle(270, 40, 30, 20, RED);
    DrawRectangle(330, 45, 20, 15, BLUE);
    DrawRectangle(370, 40, 40, 25, ORANGE);

    // Ventana
    DrawRectangle(600, 70, 160, 120, (Color){180,220,255,255});
    DrawRectangleLines(600, 70, 160, 120, (Color){120,160,200,255});
    DrawLine(680, 70, 680, 190, (Color){120,160,200,255});
    DrawLine(600, 130, 760, 130, (Color){120,160,200,255});
}

// -------------------------
// DIBUJO DE PLATAFORMAS
// -------------------------
void DrawPlatform(Rectangle p) {

    // Suelo especial grande
    if (p.y > SCREEN_H - 30) {
        DrawRectangleRec(p, (Color){150,140,120,255});
        return;
    }

    // repisa delgada
    if (p.height <= 14) {
        DrawRectangleRec(p, (Color){160,130,90,255});
        DrawLine(p.x, p.y + p.height, p.x+p.width, p.y+p.height, (Color){60,40,20,255});
        return;
    }

    // mesa
    if (p.height > 14 && p.height <= 20) {
        DrawRectangleRec(p, (Color){180,150,110,255});
        DrawRectangle(p.x + 8, p.y + p.height, 8, 28, (Color){120, 90, 60, 255});
        DrawRectangle(p.x + p.width - 16, p.y + p.height, 8, 28, (Color){120, 90, 60, 255});
        return;
    }

    // estufa
    DrawRectangleRec(p, (Color){100,100,100,255});
    DrawCircle(p.x + 20, p.y + 8, 7, DARKGRAY);
    DrawCircle(p.x + 50, p.y + 8, 7, DARKGRAY);
    DrawCircle(p.x + 80, p.y + 8, 7, DARKGRAY);
}

// -------------------------------------------------------

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "Fred the Bread - Sandbox Room");
    SetTargetFPS(60);

    // ---------------------------------------------------
    // JUGADOR
    // ---------------------------------------------------
    Player player = {
        .rect = (Rectangle){100, 300, 32, 48},
        .velocity = {0,0},
        .onGround = false
    };

    const float MOVE_SPEED = 210;
    const float JUMP_FORCE = 420;

    // ---------------------------------------------------
    // PLATAFORMAS (CUARTO COMPLETO)
    // ---------------------------------------------------
    Rectangle platforms[PLAT_COUNT] = {
        // Piso
        {0, SCREEN_H - 20, SCREEN_W, 20},

        // Repisas y mesas
        {200, 480, 150, 14},
        {420, 460, 130, 20},
        {100, 420, 160, 16},
        {580, 420, 140, 16},

        // Estufa grande
        {300, 510, 120, 28},

        // Altas
        {250, 340, 120, 14},
        {450, 315, 140, 20},
        {80, 290, 100, 14},
        {600, 270, 160, 20},

        // Muy altas
        {350, 200, 120, 14},
        {180, 160, 140, 14},
    };

    // ---------------------------------------------------
    // BUCLE DEL JUEGO
    // ---------------------------------------------------
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // MOVIMIENTO HORIZONTAL
        float dx = 0;
        if (IsKeyDown(KEY_A)) dx -= 1;
        if (IsKeyDown(KEY_D)) dx += 1;
        player.velocity.x = dx * MOVE_SPEED;

        // SALTO
        if (IsKeyPressed(KEY_SPACE) && player.onGround) {
            player.velocity.y = -JUMP_FORCE;
            player.onGround = false;
        }

        // GRAVEDAD
        player.velocity.y += GRAVITY * dt;

        // MOVER
        player.rect.x += player.velocity.x * dt;
        player.rect.y += player.velocity.y * dt;

        // COLISIONES
        player.onGround = false;
        for (int i = 0; i < PLAT_COUNT; i++) {

            if (CheckCollisionRecs(player.rect, platforms[i])) {

                Rectangle p = platforms[i];
                float overlapX = (player.rect.x + player.rect.width) - p.x;
                float overlapX2 = (p.x + p.width) - player.rect.x;

                float overlapY = (player.rect.y + player.rect.height) - p.y;
                float overlapY2 = (p.y + p.height) - player.rect.y;

                if (overlapX > 0 && overlapX2 > 0 && overlapY > 0 && overlapY2 > 0) {

                    if (overlapY < overlapX) {
                        if (player.rect.y < p.y) {
                            player.rect.y -= overlapY;
                            player.velocity.y = 0;
                            player.onGround = true;
                        } else {
                            player.rect.y += overlapY2;
                            player.velocity.y = 0;
                        }
                    } else {
                        if (player.rect.x < p.x)
                            player.rect.x -= overlapX;
                        else
                            player.rect.x += overlapX2;

                        player.velocity.x = 0;
                    }
                }
            }
        }

        // ---------------------------------------------------
        // DIBUJO
        // ---------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        DrawKitchenBackground();

        for (int i = 0; i < PLAT_COUNT; i++)
            DrawPlatform(platforms[i]);

        DrawRectangleRec(player.rect, BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
