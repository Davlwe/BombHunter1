#pragma once
#include "raylib.h"

class Map;

class Player {
public:
    Player(Vector2 pos);

    void Update(const Map& map);
    void Draw();
    void Respawn(Vector2 pos);

    Vector2 GetGridPos() const;

private:
    Vector2 position;       // logical grid position (used for collisions)
    Vector2 visualPos;      // smoothly interpolated position (used for drawing)
    float moveCooldown;     // time until next move is allowed when holding a key

    static constexpr float MOVE_COOLDOWN  = 0.10f;   // repeat rate when holding
    static constexpr float MOVE_SMOOTHING = 18.0f;   // visual interpolation speed
};
