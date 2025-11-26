#include "raylib.h"
#include <math.h>

#define PLAT_COUNT 5
#define SCREEN_W 800
#define SCREEN_H 600

// FALTA PERSONAJE POR DEFINIR, SALE 


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

    // Cámara  
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.rect.x + player.rect.width/2, player.rect.y + player.rect.height/2 };
    camera.offset = (Vector2){ SCREEN_W/2.0f, SCREEN_H/2.0f };
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        
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

        // --- COLISIONES ---
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
       BeginMode2D(camera);

    // === FONDO ===

    // Pared con ligero color crema
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){245, 240, 230, 255});

    // Azulejos inferiores
    int tileSize = 48;
    for (int x = 0; x < SCREEN_W; x += tileSize) {
        for (int y = SCREEN_H/2; y < SCREEN_H; y += tileSize) {
            Color tileColor = ((x/tileSize + y/tileSize) % 2 == 0) ?
                (Color){220, 220, 220, 255} :
                (Color){205, 205, 205, 255};
            DrawRectangle(x, y, tileSize, tileSize, tileColor);
            DrawRectangleLines(x, y, tileSize, tileSize, (Color){180, 180, 180, 255});
        }
    }

    // Refrigerador
    DrawRectangle(40, SCREEN_H - 260, 120, 240, (Color){210, 210, 220, 255});
    DrawRectangle(40, SCREEN_H - 260, 120, 6, (Color){150, 150, 160, 255}); // borde
    DrawRectangle(50, SCREEN_H - 150, 18, 50, GRAY); // manija inferior
    DrawRectangle(50, SCREEN_H - 240, 18, 40, GRAY); // manija superior

    // Estante de pared
    DrawRectangle(250, 100, 300, 12, (Color){120, 100, 80, 255});
    // Objetos sobre estante
    DrawRectangle(260, 70, 30, 30, RED);   // caja
    DrawRectangle(310, 80, 20, 20, BLUE);  // taza
    DrawRectangle(350, 75, 40, 25, ORANGE);// frasco

    // Ventana
    DrawRectangle(580, 80, 160, 120, (Color){180, 220, 255, 255});
    DrawRectangleLines(580, 80, 160, 120, (Color){150, 180, 220, 255});
    // Marco
    DrawLine(660, 80, 660, 200, (Color){150, 180, 220, 255});
    DrawLine(580, 140, 740, 140, (Color){150, 180, 220, 255});


    // === PLATAFORMAS ===
    for (int i = 0; i < PLAT_COUNT; i++) {
        Rectangle p = platforms[i];

        // --- superficie general ---
        DrawRectangleRec(p, (Color){180, 150, 110, 255});

        // borde oscuro arriba
        DrawRectangle(p.x, p.y, p.width, 4, (Color){100, 70, 40, 255});

        // Dependiendo de su altura, cambiar el tipo de mueble:
        if (p.y > SCREEN_H - 40) {
            // plataforma piso
            continue;
        }
        else if (p.height <= 18) {
            // repisa delgada
            DrawLine(p.x, p.y + p.height, p.x + p.width, p.y + p.height, (Color){80, 60, 40, 255});
        }
        else if (p.height > 18 && p.height <= 20) {
            // mesa
            DrawRectangle(p.x + 10, p.y + p.height, 6, 30, (Color){120, 90, 60, 255});
            DrawRectangle(p.x + p.width - 16, p.y + p.height, 6, 30, (Color){120, 90, 60, 255});
        }
        else {
            // estufa
            DrawCircle(p.x + 20, p.y + 8, 6, DARKGRAY);
            DrawCircle(p.x + 50, p.y + 8, 6, DARKGRAY);
            DrawCircle(p.x + 80, p.y + 8, 6, DARKGRAY);
        }
    }

    // === JUGADOR (por ahora cuadro azul) ===
    DrawRectangleRec(player.rect, BLUE);

EndMode2D();


EndDrawing();

    }

    CloseWindow();
    return 0;
}