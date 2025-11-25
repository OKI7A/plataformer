#include "raylib.h"
#include <math.h>

#define PLAT_COUNT 5
#define SCREEN_W 800
#define SCREEN_H 600

// FALTA PERSONAJE POR DEFINIR SALE 


typedef struct Player {
    Rectangle rect;
    Vector2 velocity;
    bool onGround;
} Player;

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Ian el pan");
    SetTargetFPS(60);

    // Jugador
    Player player = { { 100.0f, 100.0f, 32.0f, 48.0f }, {0,0}, false };
    const float MOVE_SPEED = 220.0f;    
    const float GRAVITY = 900.0f;       
    const float JUMP_FORCE = 420.0f;    

    // Plataformas 
    static Rectangle platforms[PLAT_COUNT] = {
    { 0, SCREEN_H - 20, SCREEN_W, 20 },
    { 200, 450, 120, 16 },
    { 380, 350, 160, 16 },
    { 80, 300, 100, 16 },
    { 600, 250, 140, 16 }
};

    // Cámara simple 
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.rect.x + player.rect.width/2, player.rect.y + player.rect.height/2 };
    camera.offset = (Vector2){ SCREEN_W/2.0f, SCREEN_H/2.0f };
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // --- INPUT HORIZONTAL ---
        float dx = 0;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) dx -= 1;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) dx += 1;
        player.velocity.x = dx * MOVE_SPEED;

        // --- JUMP ---
        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && player.onGround) {
            player.velocity.y = -JUMP_FORCE;
            player.onGround = false;
        }

        // --- GRAVITY ---
        player.velocity.y += GRAVITY * dt;

        // Aplicar movimiento
        player.rect.x += player.velocity.x * dt;
        player.rect.y += player.velocity.y * dt;

        // --- COLISIONES SIMPLES ---
        player.onGround = false;

        for (int i = 0; i < PLAT_COUNT; i++) {
            Rectangle plat = platforms[i];

            if (CheckCollisionRecs(player.rect, plat)) {
                Rectangle inter = { 0 };
                float ix = fmaxf(player.rect.x, plat.x);
                float iy = fmaxf(player.rect.y, plat.y);
                float ax = fminf(player.rect.x + player.rect.width, plat.x + plat.width);
                float ay = fminf(player.rect.y + player.rect.height, plat.y + plat.height);
                inter.x = ix;
                inter.y = iy;
                inter.width = ax - ix;
                inter.height = ay - iy;

                if (inter.width > 0 && inter.height > 0) {
                    if (inter.width < inter.height) {
                        if (player.rect.x < plat.x) {
                            player.rect.x -= inter.width;
                        } else {
                            player.rect.x += inter.width;
                        }
                        player.velocity.x = 0;
                    } else {

                        if (player.rect.y < plat.y) {
                            player.rect.y -= inter.height;
                            player.velocity.y = 0;
                            player.onGround = true;
                        } else {
                            player.rect.y += inter.height;
                            player.velocity.y = 0;
                        }
                    }
                }
            }
        }


        if (player.rect.x < -200) player.rect.x = -200;
        if (player.rect.x + player.rect.width > SCREEN_W + 200) player.rect.x = SCREEN_W + 200 - player.rect.width;

        // --- CAMARA ---
        camera.target = (Vector2){ player.rect.x + player.rect.width/2.0f, player.rect.y + player.rect.height/2.0f };

        // --- DIBUJO ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                // Dibujar plataformas
                for (int i = 0; i < PLAT_COUNT; i++) {
                    DrawRectangleRec(platforms[i], GRAY);
                }

                // Dibujar jugador
                Color playerColor = player.onGround ? BLUE : RED;
                DrawRectangleRec(player.rect, playerColor);

                
            EndMode2D();

            

        EndDrawing();
    }

    CloseWindow();
    return 0;
}