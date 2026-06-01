#pragma once
#include "raylib.h"

class Map;

// Purely cosmetic — does not affect behaviour
enum class MonsterType { Normal, Fast, Smart };

class Monster {
public:
    Monster(Vector2 pos);

    void Update(const Map& map, Vector2 playerPos);
    void Draw();
    void Kill();

    Vector2 GetGridPos() const;
    bool IsDead() const;

private:
    Vector2 position;       // logical grid position (used for collisions)
    Vector2 visualPos;      // smoothly interpolated position (used for drawing)
    MonsterType type;
    float moveTimer;
    bool dead;

    static constexpr float MOVE_DELAY     = 0.65f;   // time between monster moves
    static constexpr float MOVE_SMOOTHING = 14.0f;   // visual interpolation speed
};
