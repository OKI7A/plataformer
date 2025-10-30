#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Juego de la Vida - Raylib base");
    SetTargetFPS(10);

    const int cols = 80;
    const int rows = 45;
    const int cellSize = 10;

    bool grid[45][80] = { false };
    bool next[45][80] = { false };

    bool running = true;

    srand(time(NULL));

    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
            grid[y][x] = GetRandomValue(0, 4) == 0;

    while (!WindowShouldClose())
    {
        if (running)
        {
            for (int y = 0; y < rows; y++)
            {
                for (int x = 0; x < cols; x++)
                {
                    int alive = 0;
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        for (int dx = -1; dx <= 1; dx++)
                        {
                            if (dx == 0 && dy == 0) continue;
                            int nx = x + dx;
                            int ny = y + dy;
                            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
                                if (grid[ny][nx]) alive++;
                        }
                    }
                    if (grid[y][x] && (alive == 2 || alive == 3))
                        next[y][x] = true;
                    else if (!grid[y][x] && alive == 3)
                        next[y][x] = true;
                    else
                        next[y][x] = false;
                }
            }

            for (int y = 0; y < rows; y++)
                for (int x = 0; x < cols; x++)
                    grid[y][x] = next[y][x];
        }

        BeginDrawing();
        ClearBackground(WHITE);

        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                if (grid[y][x])
                    DrawRectangle(x * cellSize, y * cellSize, cellSize - 1, cellSize - 1, BLACK);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
