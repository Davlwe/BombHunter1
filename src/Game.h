#pragma once

#include "raylib.h"
#include <vector>

// Forward declarations
class Player;
class Monster;
class Bomb;
class Level;

enum GameState {
    MAIN_MENU,
    HOW_TO_PLAY,
    PLAYING,
    LEVEL_CLEAR,
    GAME_OVER,
    WIN
};

class Game {
public:
    Game();
    ~Game();

    void Init();
    void Update();
    void Draw();

    void ResetGame();
    void LoadLevel(int levelIndex);

private:
    void UpdateMainMenu();
    void UpdateHowToPlay();
    void UpdatePlaying();
    void UpdateLevelClear();
    void UpdateGameOver();

    void DrawMainMenu();
    void DrawHowToPlay();
    void DrawPlaying();
    void DrawUI();

private:
    GameState state;

    int currentLevel;
    int lives;

    Player* player;
    std::vector<Monster*> monsters;
    Bomb* activeBomb;

    Level* level;

    float levelClearTimer;
};