#include "raylib.h"
#include <math.h>

#define VIRTUAL_W 800
#define VIRTUAL_H 600

#define ENEMY_COUNT 5
#define PLAT_COUNT 14
#define GRAVITY 850.0f

// Estados de animación
typedef enum {
    ANIM_IDLE = 0,
    ANIM_WALK = 1,
    ANIM_JUMP = 2
} AnimationState;

typedef struct Player {
    Rectangle rect;
    Vector2 velocity;
    bool onGround;
    Texture2D spriteSheet; 
    Rectangle frameRec;
    int currentFrame;
    int framesCounter;
    AnimationState animState;
    bool facingRight;
    bool isMoving;
} Player;

typedef struct Enemy {
    Rectangle rect;
} Enemy;

// --- DIBUJO DE ESCENARIO ---

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

    DrawRectangle(600, 70, 160, 120, (Color){180,220,255,255});
    DrawRectangleLines(600, 70, 160, 120, (Color){120,160,200,255});
}

//PLATAFORMAS
void DrawPlatform(Rectangle p) {
    DrawRectangleRec(p, (Color){160,130,90,255});
}

// ENEMIGO 
void DrawEnemy(Enemy enemy) {
    DrawRectangleRec(enemy.rect, (Color){20, 180, 60, 255});
    DrawCircle(enemy.rect.x + 8,  enemy.rect.y + 6, 3, DARKGREEN);
    DrawCircle(enemy.rect.x + 20, enemy.rect.y + 6, 3, DARKGREEN);
}

// LÓGICA DE ANIMACIÓN

//  ANIMACIÓN DEL JUGADOR
void UpdatePlayerAnimation(Player *player) {
    
    int framesPerAnimation[] = {3, 4, 1}; 
    float animationSpeed[] = {8.0f, 12.0f, 1.0f}; 
    
    player->framesCounter++;
    
    
    AnimationState newState = player->animState;
    
    if (!player->onGround) {
        newState = ANIM_JUMP; 
    } else if (player->isMoving) {
        newState = ANIM_WALK; 
    } else {
        newState = ANIM_IDLE; 
    }
    
    
    if (newState != player->animState) {
        player->animState = newState;
        player->currentFrame = 0;
        player->framesCounter = 0;
    }
    
    
    int speed = (int)(60.0f / animationSpeed[player->animState]);
    
    if (player->framesCounter >= speed) {
        player->currentFrame++;
        player->framesCounter = 0;
        
       
        if (player->currentFrame >= framesPerAnimation[player->animState]) {
            player->currentFrame = 0;
        }
    }
}

// DIBUJAR JUGADOR
void DrawPlayer(Player player) {
    
    float frameWidth = 32.0f;
    float frameHeight = 32.0f;
    

    Rectangle sourceRec = {
        player.currentFrame * frameWidth,  
        player.animState * frameHeight,        
        frameWidth,                           
        frameHeight                            
    };
    
    
    if (!player.facingRight) {
         sourceRec.width = -frameWidth; 
    } else {
         sourceRec.width = frameWidth;
    }
    
    // Dibujar el sprite
    DrawTextureRec(player.spriteSheet, 
                  sourceRec, 
                  (Vector2){player.rect.x, player.rect.y}, 
                  WHITE);
}

//PROGRAMA PRINCIPAL 

int main(void) {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Fred the Bread");
    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_W, VIRTUAL_H);
    
    
    Texture2D playerSpriteSheet = LoadTexture("bread_right.png");
    Texture2D goalTex = LoadTexture("bolsa.png");
    
 
    if (playerSpriteSheet.id == 0) {
        TraceLog(LOG_FATAL, "ERROR: No se encontró bread_right.png. Asegúrate de que el archivo existe.");
        CloseWindow();
        return 1;
    }

    bool gameOver = false;
    bool victory = false;

    //INICIALIZAR JUGADOR 
    Player player = {
        .rect = (Rectangle){20, 450, 32, 32},   
        .velocity = {0, 0},
        .onGround = false,
        
        .spriteSheet = playerSpriteSheet, 
        .frameRec = (Rectangle){0, 0, 32, 32},
        .currentFrame = 0,
        .framesCounter = 0,
        .animState = ANIM_IDLE,
        .facingRight = true,
        .isMoving = false
    };

    const float MOVE_SPEED = 210;
    const float JUMP_FORCE = 350;
    

    Rectangle platforms[PLAT_COUNT] = {
        {0, 580, 800, 20},
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
        {19, 170, 70, 17},
        {135, 120, 120, 17},
        {290, 70, 300, 40}
    };

    Enemy enemies[ENEMY_COUNT] = {
        { .rect = {260, 495, 28, 16} },
        { .rect = {450, 465, 28, 16} },
        { .rect = {370, 285, 28, 16} },
        { .rect = {580, 350, 28, 16} },
        { .rect = {175, 105, 28, 16} }
    };

    Rectangle goal = {480, 35, 80, 45};
    Rectangle goalSource = { 0, 0, (float)goalTex.width, (float)goalTex.height };

    // GAME LOOP 
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_ESCAPE)) break;

        // PANTALLAS FINALES
        if (gameOver || victory) {
            BeginDrawing();
            ClearBackground(BLACK);

            if (victory) {
                DrawText("¡VICTORIA!", GetScreenWidth()/2 - 180, GetScreenHeight()/2 - 60, 60, GREEN);
                DrawText("VAMOOOS! Salvaste a FRED", GetScreenWidth()/2 - 230, GetScreenHeight()/2, 30, WHITE);
            } else {
                DrawText("GAME OVER", GetScreenWidth()/2 - 180, GetScreenHeight()/2 - 60, 60, RED);
            }

            DrawText("Presiona R para reiniciar", GetScreenWidth()/2 - 200, GetScreenHeight()/2 + 50, 25, YELLOW);
            EndDrawing();

            if (IsKeyPressed(KEY_R)) {
                player.rect = (Rectangle){20, 450, 32, 32};
                player.velocity = (Vector2){0,0};
                player.onGround = false;
                player.currentFrame = 0;
                player.animState = ANIM_IDLE;
                player.facingRight = true;
                player.isMoving = false;
                gameOver = false;
                victory = false;
            }
            continue;
        }

        // MOVIMIENTO 
        player.isMoving = false;
        
        float dx = 0;
        if (IsKeyDown(KEY_A)) {
            dx -= 1;
            player.facingRight = false;
            player.isMoving = true;
        }
        if (IsKeyDown(KEY_D)) {
            dx += 1;
            player.facingRight = true;
            player.isMoving = true;
        }
        player.velocity.x = dx * MOVE_SPEED;

        if (IsKeyPressed(KEY_SPACE) && player.onGround) {
            player.velocity.y = -JUMP_FORCE;
            player.onGround = false;
        }

        player.velocity.y += GRAVITY * dt;

        player.rect.x += player.velocity.x * dt;
        player.rect.y += player.velocity.y * dt;
        player.onGround = false;

        for (int i = 0; i < PLAT_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, platforms[i])) {
                if (player.velocity.y > 0) {
                    player.rect.y = platforms[i].y - player.rect.height;
                    player.velocity.y = 0;
                    player.onGround = true;
                }
            }
        }

        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (CheckCollisionRecs(player.rect, enemies[i].rect)) {
                gameOver = true;
            }
        }

        if (CheckCollisionRecs(player.rect, goal)) {
            victory = true;
        }

        UpdatePlayerAnimation(&player);

        //DIBUJO 
        BeginTextureMode(target);
        ClearBackground(BLACK);

        DrawKitchenBackground();

        for (int i = 0; i < PLAT_COUNT; i++)
            DrawPlatform(platforms[i]);

        DrawTexturePro(goalTex, goalSource, goal, (Vector2){0,0}, 0.0f, WHITE);

    
        DrawPlayer(player);

        for (int i = 0; i < ENEMY_COUNT; i++)
            DrawEnemy(enemies[i]);

        EndTextureMode();

        float scale = fmin(
            (float)GetScreenWidth() / VIRTUAL_W,
            (float)GetScreenHeight() / VIRTUAL_H
        );

        float offsetX = (GetScreenWidth() - VIRTUAL_W * scale) / 2;
        float offsetY = (GetScreenHeight() - VIRTUAL_H * scale) / 2;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
            target.texture,
            (Rectangle){0, 0, VIRTUAL_W, -VIRTUAL_H},
            (Rectangle){offsetX, offsetY, VIRTUAL_W * scale, VIRTUAL_H * scale},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
        
        // // DEBUG EXTENDIDO
        // DrawText(TextFormat("Direccion: %s", player.facingRight ? "DERECHA" : "IZQUIERDA"), 
        //         10, 10, 20, player.facingRight ? GREEN : RED);
        // DrawText(TextFormat("Frame: %d", player.currentFrame), 
        //         10, 40, 20, DARKGRAY);
        
        // const char* stateNames[] = {"IDLE", "WALK", "JUMP"};
        // DrawText(TextFormat("Estado: %s", stateNames[player.animState]), 
        //         10, 70, 20, DARKGRAY);
        
       
        
        EndDrawing();
    }

    //  LIMPIEZA 
    UnloadTexture(playerSpriteSheet);
    UnloadTexture(goalTex);
    UnloadRenderTexture(target);
    CloseWindow();
    
    return 0;
}