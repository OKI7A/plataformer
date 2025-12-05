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
    Rectangle collisionRect;  
} Enemy;

// FONDO

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

// PLATAFORMAS

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

// dibujar enemigo
void DrawEnemy(Enemy enemy) {
    // Cuerpo del enemigo
    DrawRectangleRec(enemy.rect, (Color){20, 180, 60, 255});
    
    // Ojos
    DrawCircle(enemy.rect.x + 8, enemy.rect.y + 6, 3, DARKGREEN);
    DrawCircle(enemy.rect.x + 20, enemy.rect.y + 6, 3, DARKGREEN);
    
    
    // DrawRectangleLinesEx(enemy.collisionRect, 1, RED);
}

// MAIN

int main() {

    // FULLSCREEN
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Fred the Bread");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);

    //GAME OVER STATE
    bool gameOver = false;

    // JUGADOR
    Player player = {
        .rect = (Rectangle){100, 300, 32, 48},
        .velocity = {0, 0},
        .onGround = false
    };

    const float MOVE_SPEED = 210;
    const float JUMP_FORCE = 310;

    const float JUMP_CUTOFF = 200;
    const float FALL_GRAVITY = 900;

    // PLATAFORMAS
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
        {240, 255, 50, 17},
        {100, 215, 120, 13},
        {19, 150, 120, 17}
    };

    // ENEMIGOS
    Enemy enemies[ENEMY_COUNT] = {
        {   // Enemigo 1
            .rect = (Rectangle){260, 490, 28, 16},  
            .collisionRect = (Rectangle){262, 492, 24, 12}  
        },
        {   // Enemigo 2
            .rect = (Rectangle){450, 460, 28, 16},
            .collisionRect = (Rectangle){452, 462, 24, 12}
        },
        {   // Enemigo 3
            .rect = (Rectangle){330, 280, 28, 16},
            .collisionRect = (Rectangle){332, 282, 24, 12}
        },
        {   // Enemigo 4
            .rect = (Rectangle){110, 185, 28, 16},
            .collisionRect = (Rectangle){112, 187, 24, 12}
        },
        {   // Enemigo 5
            .rect = (Rectangle){560, 350, 28, 16},
            .collisionRect = (Rectangle){562, 352, 24, 12}
        }
    };

    // GAME LOOP
    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        
        if (IsKeyPressed(KEY_ESCAPE)) break;

        // GAME OVER
        if (gameOver) {

            if (IsKeyPressed(KEY_R)) {
                // Reiniciar jugador
                player.rect.x = 100;
                player.rect.y = 300;
                player.velocity = (Vector2){0,0};
                gameOver = false;
            }

            // DIBUJAR LA PANTALLA DE GAME OVER
            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("GAME OVER", GetScreenWidth()/2 - 180, GetScreenHeight()/2 - 40, 60, RED);
            DrawText("Presiona R para reiniciar", GetScreenWidth()/2 - 160, GetScreenHeight()/2 + 40, 30, WHITE);
            DrawText("ESC para salir", GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 20, LIGHTGRAY);

            EndDrawing();
            continue;
        }

        // MOVIMIENTO NORMAL

        float dx = 0;
        if (IsKeyDown(KEY_A)) dx -= 1;
        if (IsKeyDown(KEY_D)) dx += 1;
        player.velocity.x = dx * MOVE_SPEED;

        if (IsKeyPressed(KEY_SPACE) && player.onGround) {
            player.velocity.y = -JUMP_FORCE;
            player.onGround = false;
        }

        // GRAVEDAD
        player.velocity.y += GRAVITY * dt;

        // SALTO VARIABLE
        if (!IsKeyDown(KEY_SPACE) && player.velocity.y < 0) {
            player.velocity.y += FALL_GRAVITY * dt;
        }

        // MOVER X
        player.rect.x += player.velocity.x * dt;

        for (int i = 0; i < PLAT_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, platforms[i])) {

                if (player.velocity.x > 0)
                    player.rect.x = platforms[i].x - player.rect.width;

                else if (player.velocity.x < 0)
                    player.rect.x = platforms[i].x + platforms[i].width;
            }
        }

        // MOVER Y
        player.rect.y += player.velocity.y * dt;
        player.onGround = false;

        for (int i = 0; i < PLAT_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, platforms[i])) {

                if (player.velocity.y > 0) {
                    player.rect.y = platforms[i].y - player.rect.height;
                    player.velocity.y = 0;
                    player.onGround = true;
                }
                else if (player.velocity.y < 0) {
                    player.rect.y = platforms[i].y + platforms[i].height;
                    player.velocity.y = 0;
                }
            }
        }

        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, enemies[i].collisionRect)) {
                gameOver = true;
            }
        }

        
        BeginTextureMode(target);
        ClearBackground(BLACK);
        
        DrawKitchenBackground();
        
        for (int i = 0; i < PLAT_COUNT; i++)
            DrawPlatform(platforms[i]);
        
        DrawRectangleRec(player.rect, BLUE);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            DrawEnemy(enemies[i]);
        }
        
        EndTextureMode();

       
        BeginDrawing();
        ClearBackground(BLACK);
        
        
        DrawTexturePro(
            target.texture,
            (Rectangle){0, 0, VIRTUAL_W, -VIRTUAL_H},  
            (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
        
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}