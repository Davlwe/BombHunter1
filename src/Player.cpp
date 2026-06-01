#include "Player.h"
#include "Map.h"
#include <cmath>

Player::Player(Vector2 pos)
{
    position    = pos;
    visualPos   = { (float)pos.x, (float)pos.y };
    moveCooldown = 0.0f;
}

void Player::Update(const Map& map)
{
    // ---- Tick cooldown ----
    float dt = GetFrameTime();
    if (moveCooldown > 0.0f)
        moveCooldown -= dt;

    // ---- Handle input (IsKeyDown for smooth held-key repeat) ----
    int dx = 0, dy = 0;

    if (moveCooldown <= 0.0f)
    {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            dy = -1;
        }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            dy = 1;
        }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            dx = -1;
        }
        else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            dx = 1;
        }

        if (dx != 0 || dy != 0)
        {
            int newX = (int)position.x + dx;
            int newY = (int)position.y + dy;

            if (map.IsWalkable(newX, newY))
            {
                position.x = (float)newX;
                position.y = (float)newY;
                moveCooldown = MOVE_COOLDOWN;
            }
        }
    }

    // ---- Smooth visual interpolation toward logical position ----
    float t = 1.0f - expf(-MOVE_SMOOTHING * dt);
    visualPos.x += (position.x - visualPos.x) * t;
    visualPos.y += (position.y - visualPos.y) * t;
}

void Player::Draw()
{
    const int S  = Map::TILE_SIZE;
    const int px = Map::OFFSET_X + (int)(visualPos.x * S);
    const int py = Map::OFFSET_Y + (int)(visualPos.y * S);
    const int M  = 3;  // margin from tile edge

    // --- Shadow underneath ---
    DrawRectangle(px + M + 1, py + S - M - 4, S - 2*M - 2, 4,
                  Fade(BLACK, 0.2f));

    // --- Body: rounded rect ---
    Rectangle body = {
        (float)(px + M),
        (float)(py + M),
        (float)(S - 2*M),
        (float)(S - 2*M)
    };
    Color bodyColor = { 30, 144, 255, 255 };   // dodger blue
    DrawRectangleRounded(body, 0.35f, 8, bodyColor);

    // --- Top highlight for depth ---
    Rectangle highlight = {
        (float)(px + M + 2),
        (float)(py + M + 2),
        (float)(S - 2*M - 4),
        (float)((S - 2*M) / 2 - 2)
    };
    DrawRectangleRounded(highlight, 0.3f, 6, Fade(WHITE, 0.25f));

    // --- Bright centre dot (eye / emblem) ---
    DrawCircle(px + S/2, py + S/2 - 2, 4, Fade(WHITE, 0.8f));
    DrawCircle(px + S/2, py + S/2 - 2, 2, Fade(BLACK, 0.5f));
}

void Player::Respawn(Vector2 pos)
{
    position  = pos;
    visualPos = { (float)pos.x, (float)pos.y };
    moveCooldown = 0.0f;
}

Vector2 Player::GetGridPos() const
{
    return position;
}
