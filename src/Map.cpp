#include "Map.h"
#include <cmath>

Map::Map()
{
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            tiles[y][x] = 0;
}

void Map::LoadFromArray(const int data[HEIGHT][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            tiles[y][x] = data[y][x];
}

bool Map::IsWalkable(int x, int y) const
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return false;
    return tiles[y][x] != 1;   // 1 = wall
}

int Map::GetTile(int x, int y) const
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return -1;
    return tiles[y][x];
}

void Map::Draw() const
{
    // Colour palette
    Color grassA     = { 34,  139, 34,  255 };   // forest green
    Color grassB     = { 45,  155, 42,  255 };   // lighter variant
    Color wallBase   = { 105, 105, 115, 255 };   // stone grey
    Color wallTop    = { 140, 140, 150, 255 };   // lit edge
    Color wallShadow = { 70,  70,  80,  255 };   // shadow edge
    Color gridColor  = { 0,   0,   0,   30  };   // very subtle grid

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            int px = OFFSET_X + x * TILE_SIZE;
            int py = OFFSET_Y + y * TILE_SIZE;

            if (tiles[y][x] == 1)
            {
                // --- Stone wall with beveled 3D edges ---
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, wallBase);

                // Top-left lit edge
                DrawRectangle(px, py, TILE_SIZE, 3, wallTop);
                DrawRectangle(px, py, 3, TILE_SIZE, wallTop);

                // Bottom-right shadow edge
                DrawRectangle(px, py + TILE_SIZE - 3, TILE_SIZE, 3, wallShadow);
                DrawRectangle(px + TILE_SIZE - 3, py, 3, TILE_SIZE, wallShadow);

                // Mortar dots in two corners
                DrawCircle(px + 3, py + 3, 2, wallTop);
                DrawCircle(px + TILE_SIZE - 3, py + TILE_SIZE - 3, 2, wallShadow);
            }
            else
            {
                // --- Grass floor with checkerboard variation ---
                Color grass = ((x + y) % 2 == 0) ? grassA : grassB;
                DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, grass);

                // Occasional random-seeming grass tuft (deterministic)
                int seed = (x * 7 + y * 13) % 11;
                if (seed == 0)
                {
                    DrawCircle(px + 8,  py + 8,  2, Fade(grassB, 0.5f));
                    DrawCircle(px + 22, py + 20, 2, Fade(grassA, 0.4f));
                }
                else if (seed == 2)
                {
                    DrawCircle(px + 16, py + 10, 2, Fade(grassB, 0.4f));
                    DrawCircle(px + 6,  py + 22, 1, Fade(grassA, 0.5f));
                }
            }

            // Grid line (bottom and right only, to avoid double-drawing)
            DrawLine(px, py + TILE_SIZE, px + TILE_SIZE, py + TILE_SIZE, gridColor);
            DrawLine(px + TILE_SIZE, py, px + TILE_SIZE, py + TILE_SIZE, gridColor);
        }
    }
}
