#pragma once
#include "raylib.h"

class Map {
public:
    static const int WIDTH     = 20;
    static const int HEIGHT    = 15;
    static const int TILE_SIZE = 32;

    // Centring offset for a 1280×720 window
    static const int BOARD_W = WIDTH  * TILE_SIZE;   // 640
    static const int BOARD_H = HEIGHT * TILE_SIZE;   // 480
    static const int OFFSET_X = (1280 - BOARD_W) / 2; // 320
    static const int OFFSET_Y = (720  - BOARD_H) / 2; // 120

    Map();

    void LoadFromArray(const int data[HEIGHT][WIDTH]);
    bool IsWalkable(int x, int y) const;
    int  GetTile(int x, int y) const;
    void Draw() const;

private:
    int tiles[HEIGHT][WIDTH];
};
