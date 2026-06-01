#include "raylib.h"
#include "src/Game.h"

// =========================
// Web (Emscripten) support
// =========================
#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>

static Game game;

static void UpdateDrawFrame(void)
{
    game.Update();
    game.Draw();
}

int main()
{
    const int screenW = 1280;
    const int screenH = 720;

    InitWindow(screenW, screenH, "Bomb Hunter");
    game.Init();

    // Hand control to the browser's requestAnimationFrame loop.
    // fps=0  →  use the browser's native refresh rate (typically 60).
    // simulate_infinite_loop=1  →  emscripten never returns from this call.
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

    // Unreachable on web — emscripten_set_main_loop never returns.
    CloseWindow();
    return 0;
}

#else // ─── Native desktop ────────────────────────────────────────────

int main()
{
    InitWindow(1280, 720, "Bomb Hunter");
    SetTargetFPS(60);

    Game game;
    game.Init();

    while (!WindowShouldClose())
    {
        game.Update();
        game.Draw();
    }

    CloseWindow();
    return 0;
}

#endif // PLATFORM_WEB
