#include "Level.h"
#include <fstream>
#include <sstream>
#include <string>

Level::Level(int index)
{
    playerSpawn = {1, 1};
    LoadFromFile(index);
}

void Level::LoadFromFile(int index)
{
    // Find levels relative to the executable, not the current working directory.
    // This makes the game work wherever it's launched from (double-click, terminal, etc.).
    std::string basePath = GetApplicationDirectory();
#ifdef PLATFORM_WEB
    // On web, GetApplicationDirectory() returns "" (no OS-specific path).
    // Level data is embedded at /levels/ via --embed-file, so use absolute path.
    if (basePath.empty() || basePath == "/")
        basePath = "/";
#endif
    std::string filename = basePath + "levels/level" + std::to_string(index) + ".txt";
    std::ifstream file(filename);

    int data[Map::HEIGHT][Map::WIDTH];

    // Start with all empty
    for (int y = 0; y < Map::HEIGHT; y++)
        for (int x = 0; x < Map::WIDTH; x++)
            data[y][x] = 0;

    if (file.is_open())
    {
        std::string line;
        for (int y = 0; y < Map::HEIGHT && std::getline(file, line); y++)
        {
            std::istringstream iss(line);
            for (int x = 0; x < Map::WIDTH; x++)
            {
                int val = 0;
                iss >> val;
                data[y][x] = val;

                if (val == 2)
                    playerSpawn = {(float)x, (float)y};
                else if (val == 3)
                    monsterSpawns.push_back({(float)x, (float)y});
            }
        }
        file.close();
    }
    else
    {
        // Fallback: default level with border walls
        for (int x = 0; x < Map::WIDTH; x++)
        {
            data[0][x] = 1;
            data[Map::HEIGHT - 1][x] = 1;
        }
        for (int y = 0; y < Map::HEIGHT; y++)
        {
            data[y][0] = 1;
            data[y][Map::WIDTH - 1] = 1;
        }
        data[2][2] = 2;
        data[5][5] = 3;
        monsterSpawns.push_back({5, 5});
    }

    map.LoadFromArray(data);
}

void Level::Draw() const
{
    map.Draw();
}

Vector2 Level::GetPlayerSpawn() const
{
    return playerSpawn;
}

std::vector<Vector2> Level::GetMonsterSpawns() const
{
    return monsterSpawns;
}

const Map& Level::GetMap() const
{
    return map;
}

bool Level::IsWalkable(int x, int y) const
{
    return map.IsWalkable(x, y);
}
