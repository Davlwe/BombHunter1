#include "Monster.h"
#include "Map.h"
#include <cmath>

Monster::Monster(Vector2 pos)
{
    position  = pos;
    visualPos = { (float)pos.x, (float)pos.y };
    moveTimer = 0.0f;
    dead      = false;

    // Deterministic cosmetic type from spawn position — no gameplay impact
    int hash = ((int)pos.x * 7 + (int)pos.y * 13) % 3;
    type = (hash == 0) ? MonsterType::Normal
         : (hash == 1) ? MonsterType::Fast
         :               MonsterType::Smart;
}

void Monster::Update(const Map& map, Vector2 playerPos)
{
    if (dead) return;

    float dt = GetFrameTime();
    moveTimer += dt;

    // ---- Move on timer tick ----
    if (moveTimer >= MOVE_DELAY)
    {
        moveTimer = 0.0f;

        float dx = playerPos.x - position.x;
        float dy = playerPos.y - position.y;

        int mx = 0, my = 0;

        // Prefer horizontal movement, fall back to vertical
        if (fabsf(dx) > fabsf(dy))
            mx = (dx > 0) ? 1 : -1;
        else if (dy != 0)
            my = (dy > 0) ? 1 : -1;

        int newX = (int)position.x + mx;
        int newY = (int)position.y + my;

        if (map.IsWalkable(newX, newY))
        {
            position.x = (float)newX;
            position.y = (float)newY;
        }
        else if (mx != 0)
        {
            // Horizontal blocked — try vertical
            int altY = (int)position.y + ((dy > 0) ? 1 : -1);
            if (map.IsWalkable((int)position.x, altY))
                position.y = (float)altY;
        }
        else if (my != 0)
        {
            // Vertical blocked — try horizontal
            int altX = (int)position.x + ((dx > 0) ? 1 : -1);
            if (map.IsWalkable(altX, (int)position.y))
                position.x = (float)altX;
        }
    }

    // ---- Smooth visual interpolation toward logical position ----
    float t = 1.0f - expf(-MOVE_SMOOTHING * dt);
    visualPos.x += (position.x - visualPos.x) * t;
    visualPos.y += (position.y - visualPos.y) * t;
}

void Monster::Draw()
{
    if (dead) return;

    const int S  = Map::TILE_SIZE;
    const int px = Map::OFFSET_X + (int)(visualPos.x * S);
    const int py = Map::OFFSET_Y + (int)(visualPos.y * S);
    const int M  = 3;

    // Select colour by cosmetic type
    Color bodyColor, eyeColor;
    if (type == MonsterType::Normal)
    {
        bodyColor = { 220, 40,  40,  255 };   // bright red
        eyeColor  = { 255, 220, 50,  255 };   // yellow eyes
    }
    else if (type == MonsterType::Fast)
    {
        bodyColor = { 255, 140, 0,   255 };   // bright orange
        eyeColor  = { 255, 255, 200, 255 };   // pale yellow eyes
    }
    else
    {
        bodyColor = { 150, 50,  220, 255 };   // bright purple
        eyeColor  = { 180, 255, 180, 255 };   // green eyes
    }

    // --- Shadow ---
    DrawRectangle(px + M + 1, py + S - M - 4, S - 2*M - 2, 4, Fade(BLACK, 0.2f));

    // --- Body: rounded rect ---
    Rectangle body = {
        (float)(px + M), (float)(py + M),
        (float)(S - 2*M), (float)(S - 2*M)
    };
    DrawRectangleRounded(body, 0.3f, 8, bodyColor);

    // --- Top highlight ---
    Rectangle hl = {
        (float)(px + M + 2), (float)(py + M + 2),
        (float)(S - 2*M - 4), (float)((S - 2*M) / 2 - 2)
    };
    DrawRectangleRounded(hl, 0.25f, 6, Fade(WHITE, 0.2f));

    // --- Angry eyes (two small circles) ---
    int eyeY = py + S/2 - 4;
    DrawCircle(px + 10, eyeY, 3, WHITE);
    DrawCircle(px + S - 10, eyeY, 3, WHITE);
    DrawCircle(px + 10, eyeY, 2, eyeColor);
    DrawCircle(px + S - 10, eyeY, 2, eyeColor);

    // --- Type-specific detail ---
    if (type == MonsterType::Fast)
    {
        // Speed lines on the sides
        DrawLine(px + 2, py + 8,  px + 2, py + S - 8, Fade(WHITE, 0.5f));
        DrawLine(px + S - 2, py + 8,  px + S - 2, py + S - 8, Fade(WHITE, 0.5f));
    }
    else if (type == MonsterType::Smart)
    {
        // Crown/circle on top
        DrawCircle(px + S/2, py + M + 2, 5, Fade(WHITE, 0.4f));
        DrawCircle(px + S/2, py + M + 2, 3, Fade(eyeColor, 0.6f));
    }
}

void Monster::Kill()
{
    dead = true;
}

Vector2 Monster::GetGridPos() const
{
    return position;
}

bool Monster::IsDead() const
{
    return dead;
}
