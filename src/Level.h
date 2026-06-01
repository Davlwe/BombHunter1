#pragma once
#include "raylib.h"
#include <vector>
#include "Map.h"

class Level {
public:
    Level(int index);

    void Draw() const;
    Vector2 GetPlayerSpawn() const;
    std::vector<Vector2> GetMonsterSpawns() const;
    const Map& GetMap() const;
    bool IsWalkable(int x, int y) const;

private:
    void LoadFromFile(int index);

    Map map;
    Vector2 playerSpawn;
    std::vector<Vector2> monsterSpawns;
};
