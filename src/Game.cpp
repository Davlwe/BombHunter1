#include "Game.h"

#include "Player.h"
#include "Monster.h"
#include "Bomb.h"
#include "Level.h"
#include "Map.h"

#include "raylib.h"
#include <algorithm>

// =========================
// Button helper — simple rect-based clickable region
// =========================

struct Button {
    Rectangle rect;
    const char* text;
    int fontSize;
    bool hovered;

    Button(float x, float y, float w, float h, const char* t, int fs = 24)
        : rect{x, y, w, h}, text(t), fontSize(fs), hovered(false) {}

    void Update()
    {
        hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    bool IsClicked() const
    {
        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void Draw(Color idle, Color hover, Color textColor) const
    {
        Color bg = hovered ? hover : idle;
        DrawRectangleRounded(rect, 0.25f, 8, bg);
        DrawRectangleRoundedLines(rect, 0.25f, 8, Fade(BLACK, 0.15f));

        int tw = MeasureText(text, fontSize);
        DrawText(text,
                 (int)(rect.x + (rect.width  - tw) / 2),
                 (int)(rect.y + (rect.height - fontSize) / 2),
                 fontSize, textColor);
    }
};

// =========================
// Constructor / Destructor
// =========================

Game::Game()
{
    state           = MAIN_MENU;
    currentLevel    = 1;
    lives           = 3;
    player          = nullptr;
    activeBomb      = nullptr;
    level           = nullptr;
    levelClearTimer = 0.0f;
}

Game::~Game()
{
    delete player;
    delete level;
    for (auto m : monsters) delete m;
    delete activeBomb;
}

// =========================
// Init
// =========================

void Game::Init()
{
    state = MAIN_MENU;
}

// =========================
// Main Update Loop
// =========================

void Game::Update()
{
    switch (state)
    {
        case MAIN_MENU:
            UpdateMainMenu();
            break;

        case HOW_TO_PLAY:
            UpdateHowToPlay();
            break;

        case PLAYING:
            UpdatePlaying();
            break;

        case LEVEL_CLEAR:
            UpdateLevelClear();
            break;

        case GAME_OVER:
            UpdateGameOver();
            break;

        case WIN:
            if (IsKeyPressed(KEY_ENTER))
                ResetGame();
            break;
    }
}

// =========================
// MAIN MENU STATE
// =========================

void Game::UpdateMainMenu()
{
    const float W = 1280.0f;
    const float BTN_W = 300.0f;
    const float BTN_H = 60.0f;
    const float BTN_X = (W - BTN_W) / 2.0f;

    Button startBtn(BTN_X, 340, BTN_W, BTN_H, "Start Game");
    Button howToBtn(BTN_X, 420, BTN_W, BTN_H, "How To Play");

    startBtn.Update();
    howToBtn.Update();

    if (startBtn.IsClicked())
    {
        ResetGame();          // sets lives=3, currentLevel=1, state=PLAYING
    }

    if (howToBtn.IsClicked())
    {
        state = HOW_TO_PLAY;
    }
}

// =========================
// HOW TO PLAY STATE
// =========================

void Game::UpdateHowToPlay()
{
    // Close button — top-right corner
    Button closeBtn(1280.0f - 60.0f, 20.0f, 40.0f, 40.0f, "X", 28);
    closeBtn.Update();

    if (closeBtn.IsClicked())
    {
        state = MAIN_MENU;
    }
}

// =========================
// PLAYING STATE
// =========================

static bool IsSameCell(Vector2 a, Vector2 b)
{
    return (int)a.x == (int)b.x && (int)a.y == (int)b.y;
}

void Game::UpdatePlaying()
{
    const Map& map = level->GetMap();

    // ---- Player ----
    player->Update(map);

    // ---- Place bomb (E) ----
    if (IsKeyPressed(KEY_E) && activeBomb == nullptr)
    {
        activeBomb = new Bomb(player->GetGridPos());
    }

    // ---- Detonate bomb (Q) ----
    if (IsKeyPressed(KEY_Q) && activeBomb != nullptr)
    {
        activeBomb->Detonate();
    }

    // ---- Update bomb ----
    if (activeBomb != nullptr)
    {
        activeBomb->Update();

        // Process explosion once
        if (activeBomb->IsExploding() && !activeBomb->HasExplosionBeenProcessed())
        {
            activeBomb->MarkExplosionProcessed();

            std::vector<Vector2> cells = activeBomb->GetExplosionCells(map);

            // Kill monsters in explosion
            for (auto m : monsters)
            {
                if (m->IsDead()) continue;
                for (auto& cell : cells)
                {
                    if (IsSameCell(cell, m->GetGridPos()))
                    {
                        m->Kill();
                        break;
                    }
                }
            }

            // Hurt player in explosion — restart level
            for (auto& cell : cells)
            {
                if (IsSameCell(cell, player->GetGridPos()))
                {
                    lives--;
                    LoadLevel(currentLevel);
                    return;
                }
            }
        }

        // Remove finished bomb
        if (activeBomb->HasExploded())
        {
            delete activeBomb;
            activeBomb = nullptr;
        }
    }

    // ---- Update monsters ----
    Vector2 playerPos = player->GetGridPos();
    for (auto m : monsters)
    {
        if (!m->IsDead())
            m->Update(map, playerPos);
    }

    // ---- Monster touches player — restart level ----
    for (auto m : monsters)
    {
        if (m->IsDead()) continue;
        if (IsSameCell(m->GetGridPos(), player->GetGridPos()))
        {
            lives--;
            LoadLevel(currentLevel);
            return;
        }
    }

    // ---- Remove dead monsters ----
    monsters.erase(
        std::remove_if(monsters.begin(), monsters.end(),
                       [](Monster* m) { return m->IsDead(); }),
        monsters.end());

    // ---- Game over check ----
    if (lives <= 0)
    {
        state = GAME_OVER;
        return;
    }

    // ---- Win condition ----
    if (monsters.empty())
    {
        state = LEVEL_CLEAR;
        levelClearTimer = 0.0f;
    }
}

// =========================
// LEVEL CLEAR STATE
// =========================

void Game::UpdateLevelClear()
{
    levelClearTimer += GetFrameTime();

    if (levelClearTimer > 2.0f)
    {
        currentLevel++;

        if (currentLevel > 5)
            state = WIN;
        else
        {
            LoadLevel(currentLevel);
            state = PLAYING;
        }
    }
}

// =========================
// GAME OVER STATE
// =========================

void Game::UpdateGameOver()
{
    if (IsKeyPressed(KEY_ENTER))
        ResetGame();
}

// =========================
// DRAW
// =========================

void Game::Draw()
{
    BeginDrawing();
    ClearBackground({ 20, 28, 40, 255 });   // dark navy — consistent across states

    switch (state)
    {
        case MAIN_MENU:
            DrawMainMenu();
            break;

        case HOW_TO_PLAY:
            DrawHowToPlay();
            break;

        case PLAYING:
            DrawPlaying();
            DrawUI();
            break;

        case LEVEL_CLEAR:
            DrawPlaying();
            // Dimmed overlay
            DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.35f));
            // Banner
            DrawRectangle(0, 250, 1280, 80, Fade({ 40, 160, 60, 255 }, 0.85f));
            DrawText("LEVEL CLEAR!",
                     (int)(640 - MeasureText("LEVEL CLEAR!", 36) / 2), 268, 36, WHITE);
            break;

        case GAME_OVER:
            // Dim background
            DrawRectangle(0, 0, 1280, 720, { 20, 5, 5, 255 });
            DrawText("GAME OVER",
                     (int)(640 - MeasureText("GAME OVER", 52) / 2), 240, 52, RED);
            DrawText("Press ENTER to restart",
                     (int)(640 - MeasureText("Press ENTER to restart", 22) / 2), 340, 22,
                     { 200, 200, 210, 255 });
            // Decorative line
            DrawLine(440, 310, 840, 310, Fade(RED, 0.5f));
            break;

        case WIN:
            // Celebration background
            DrawRectangle(0, 0, 1280, 720, { 10, 20, 35, 255 });
            const char* winText = "YOU WIN!";
            int winW = MeasureText(winText, 56);
            DrawText(winText, (int)(640 - winW / 2) + 3, 217, 56, Fade(BLACK, 0.4f));
            DrawText(winText, (int)(640 - winW / 2), 214, 56, GOLD);

            DrawText("Press ENTER to restart",
                     (int)(640 - MeasureText("Press ENTER to restart", 22) / 2), 320, 22,
                     { 180, 200, 230, 255 });

            // Decorative sparkles
            float t = (float)GetTime();
            for (int i = 0; i < 20; i++)
            {
                float angle = t * 0.8f + i * 0.628f;
                float rad   = 180.0f + sinf(t * 1.5f + i) * 60.0f;
                int sx = 640 + (int)(cosf(angle) * rad);
                int sy = 280 + (int)(sinf(angle) * rad * 0.6f);
                Color spark = (i % 3 == 0) ? GOLD : YELLOW;
                DrawCircle(sx, sy, 2.0f + sinf(t * 3.0f + i) * 1.5f,
                           Fade(spark, 0.4f + 0.3f * sinf(t * 2.0f + i)));
            }
            break;
    }

    EndDrawing();
}

// =========================
// DRAW: MAIN MENU
// =========================

void Game::DrawMainMenu()
{
    const float W = 1280.0f;
    const float H = 720.0f;
    const float BTN_W = 320.0f;
    const float BTN_H = 64.0f;
    const float BTN_X = (W - BTN_W) / 2.0f;

    // --- Decorative background ---
    // Top gradient band
    DrawRectangle(0, 0, (int)W, (int)H, { 15, 25, 50, 255 });

    // Subtle grid pattern overlay
    for (int y = 0; y < (int)H; y += 48)
        DrawLine(0, y, (int)W, y, Fade(WHITE, 0.03f));
    for (int x = 0; x < (int)W; x += 48)
        DrawLine(x, 0, x, (int)H, Fade(WHITE, 0.03f));

    // Accent circles for visual interest
    DrawCircle(100, 620, 180, Fade({ 50, 120, 220, 255 }, 0.08f));
    DrawCircle(1180, 100, 140, Fade({ 220, 80, 40, 255 }, 0.07f));
    DrawCircle(640, 400, 250, Fade({ 80, 200, 120, 255 }, 0.05f));

    // --- Title with shadow ---
    const char* title = "BOMB HUNTER";
    int titleSize = 64;
    int tw = MeasureText(title, titleSize);
    DrawText(title, (int)((W - tw) / 2) + 3, 123, titleSize, Fade(BLACK, 0.3f));   // shadow
    DrawText(title, (int)((W - tw) / 2),     120, titleSize, { 255, 210, 50, 255 }); // gold

    // --- Subtitle ---
    const char* sub = "Clear every level to win!";
    int subSize = 22;
    int sw = MeasureText(sub, subSize);
    DrawText(sub, (int)((W - sw) / 2), 200, subSize, { 180, 200, 230, 255 });

    // --- Buttons ---
    Button startBtn(BTN_X, 320, BTN_W, BTN_H, "Start Game");
    Button howToBtn(BTN_X, 410, BTN_W, BTN_H, "How To Play");
    startBtn.Update();
    howToBtn.Update();

    // Start Game: green, with scale bump on hover
    if (startBtn.hovered)
    {
        Rectangle r = startBtn.rect;
        DrawRectangleRounded({r.x - 4, r.y - 2, r.width + 8, r.height + 4}, 0.3f, 8,
                             Fade({ 50, 180, 80, 255 }, 0.35f));  // glow
    }
    startBtn.Draw({ 40, 160, 60, 255 }, { 55, 200, 80, 255 }, WHITE);

    // How To Play: blue-grey
    if (howToBtn.hovered)
    {
        Rectangle r = howToBtn.rect;
        DrawRectangleRounded({r.x - 4, r.y - 2, r.width + 8, r.height + 4}, 0.3f, 8,
                             Fade({ 60, 120, 200, 255 }, 0.3f));
    }
    howToBtn.Draw({ 50, 60, 90, 255 }, { 70, 90, 140, 255 }, WHITE);

    // --- Footer ---
    const char* footer = "Arrow Keys: Move  |  E: Place Bomb  |  Q: Detonate";
    int fw = MeasureText(footer, 16);
    DrawText(footer, (int)((W - fw) / 2), 590, 16, { 130, 145, 170, 255 });

    // Version / credit line
    const char* credit = "A puzzle game built with raylib";
    int cw = MeasureText(credit, 14);
    DrawText(credit, (int)((W - cw) / 2), 670, 14, { 90, 100, 120, 255 });
}

// =========================
// DRAW: HOW TO PLAY
// =========================

void Game::DrawHowToPlay()
{
    const float W = 1280.0f;
    const float H = 720.0f;
    const int   LEFT = 180;

    // --- Background ---
    DrawRectangle(0, 0, (int)W, (int)H, { 18, 22, 40, 255 });
    for (int y = 0; y < (int)H; y += 48)
        DrawLine(0, y, (int)W, y, Fade(WHITE, 0.02f));

    // --- Content card ---
    float cardX = 100, cardY = 30, cardW = W - 200, cardH = H - 60;
    DrawRectangleRounded({cardX, cardY, cardW, cardH}, 0.12f, 8,
                         { 28, 34, 55, 255 });
    DrawRectangleRoundedLines({cardX, cardY, cardW, cardH}, 0.12f, 8,
                              Fade(WHITE, 0.08f));

    // --- Title ---
    const char* title = "HOW TO PLAY";
    int titleSize = 38;
    int tw = MeasureText(title, titleSize);
    DrawText(title, (int)((W - tw) / 2), 60, titleSize, { 255, 210, 60, 255 });

    // Divider
    DrawLine(LEFT, 112, (int)W - LEFT, 112, Fade({ 255, 210, 60, 255 }, 0.35f));

    // Instructions
    struct Line { const char* label; const char* detail; };
    Line lines[] = {
        {"Goal",    "Eliminate all monsters using bombs."},
        {"Move",    "Arrow Keys"},
        {"Bomb",    "Press  E  to place a bomb on your tile."},
        {"Detonate","Press  Q  to detonate the placed bomb."},
        {"Limit",   "Only one bomb can exist at a time."},
        {"Blast",   "Bomb explosion affects a 3x3 area."},
        {"Danger",  "Avoid monsters and your own explosions!"},
        {"Lives",   "Player has 3 lives. Losing one restarts the level."},
        {"Clear",   "Defeat all monsters to clear a level and advance."},
    };
    const int LINE_COUNT = sizeof(lines) / sizeof(lines[0]);

    int y = 140;
    for (int i = 0; i < LINE_COUNT; i++)
    {
        // Label badge
        int lw = MeasureText(lines[i].label, 20);
        DrawRectangleRounded({(float)LEFT - 8, (float)y, (float)lw + 16, 26.0f},
                             0.3f, 6, { 50, 120, 220, 255 });
        DrawText(lines[i].label, LEFT, y + 2, 20, WHITE);

        // Detail text
        DrawText(lines[i].detail, LEFT + lw + 40, y + 2, 20, { 200, 210, 230, 255 });

        y += 42;
    }

    // Tip box
    y += 18;
    DrawRectangleRounded({(float)LEFT, (float)y, W - 2*LEFT, 56.0f}, 0.2f, 8,
                         { 40, 55, 90, 255 });
    const char* tip = "Tip: Lure monsters toward your bomb before detonating!";
    int tipW = MeasureText(tip, 18);
    DrawText(tip, (int)((W - tipW) / 2), y + 16, 18, { 200, 220, 255, 255 });

    // --- Close button ---
    Button closeBtn(W - 60.0f, 20.0f, 40.0f, 40.0f, "X", 26);
    closeBtn.Update();

    Color closeIdle  = { 180, 50, 50, 255 };
    Color closeHover = { 230, 80, 80, 255 };
    closeBtn.Draw(closeIdle, closeHover, WHITE);

    // Glow behind X
    DrawCircle((int)W - 40, 40, 26, Fade({ 255, 100, 100, 255 }, 0.12f));
}

// =========================
// DRAW HELPERS (gameplay)
// =========================

void Game::DrawPlaying()
{
    level->Draw();
    player->Draw();

    for (auto m : monsters)
        m->Draw();

    if (activeBomb != nullptr)
        activeBomb->Draw();
}

void Game::DrawUI()
{
    const int PAD = 8;
    const int PANEL_W = 190;
    const int PANEL_H = 110;

    // Semi-transparent panel
    DrawRectangleRounded({(float)PAD, (float)PAD, (float)PANEL_W, (float)PANEL_H},
                         0.15f, 8, Fade(BLACK, 0.55f));
    DrawRectangleRoundedLines({(float)PAD, (float)PAD, (float)PANEL_W, (float)PANEL_H},
                              0.15f, 8, Fade(WHITE, 0.12f));

    // Level — cyan
    DrawText(TextFormat("Level: %d / 5", currentLevel),
             PAD + 12, PAD + 10, 20, { 80, 220, 255, 255 });

    // Lives — green→yellow→red based on count
    Color lifeCol;
    if      (lives >= 3) lifeCol = { 80,  220, 80,  255 };
    else if (lives == 2) lifeCol = { 240, 220, 50,  255 };
    else                 lifeCol = { 240, 60,  60,  255 };

    DrawText(TextFormat("Lives: %d", lives),
             PAD + 12, PAD + 38, 20, lifeCol);

    // Monsters — white, with icon
    DrawText(TextFormat("Monsters: %d", (int)monsters.size()),
             PAD + 12, PAD + 66, 20, { 220, 220, 230, 255 });

    // Small pulses on lives if low
    if (lives == 1)
    {
        float pulse = 0.7f + 0.3f * sinf((float)GetTime() * 5.0f);
        DrawText("!", PAD + PANEL_W - 24, PAD + 36, 22,
                 Fade(RED, pulse));
    }
}

// =========================
// GAME CONTROL
// =========================

void Game::ResetGame()
{
    currentLevel = 1;
    lives = 3;
    LoadLevel(currentLevel);
    state = PLAYING;
}

void Game::LoadLevel(int levelIndex)
{
    delete player;
    delete level;
    delete activeBomb;
    for (auto m : monsters) delete m;
    monsters.clear();

    player     = nullptr;
    level      = nullptr;
    activeBomb = nullptr;

    level = new Level(levelIndex);

    player = new Player(level->GetPlayerSpawn());

    for (auto& spawn : level->GetMonsterSpawns())
        monsters.push_back(new Monster(spawn));

    activeBomb = nullptr;
}
