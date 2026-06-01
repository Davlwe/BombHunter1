#pragma once
#include "raylib.h"
#include <vector>

class Map;

class Bomb {
public:
    Bomb(Vector2 pos);

    void Update();
    void Draw();
    void Detonate();                     // player pressed Q — start 0.5s delay

    bool HasExploded() const;
    bool IsExploding() const;
    bool HasExplosionBeenProcessed() const;
    void MarkExplosionProcessed();

    Vector2 GetGridPos() const;
    std::vector<Vector2> GetExplosionCells(const Map& map) const;

private:
    Vector2 position;
    float detonateTimer;
    float explosionTimer;
    bool detonating;
    bool exploded;
    bool done;
    bool explosionProcessed;

    static const float DETONATE_DELAY;
    static const float EXPLOSION_DURATION;
};
