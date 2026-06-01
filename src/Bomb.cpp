#include "Bomb.h"
#include "Map.h"
#include <cmath>

const float Bomb::DETONATE_DELAY     = 0.5f;
const float Bomb::EXPLOSION_DURATION = 0.4f;

Bomb::Bomb(Vector2 pos)
{
    position           = pos;
    detonateTimer      = 0.0f;
    explosionTimer     = 0.0f;
    detonating         = false;
    exploded           = false;
    done               = false;
    explosionProcessed = false;
}

void Bomb::Update()
{
    if (done) return;

    if (detonating)
    {
        detonateTimer += GetFrameTime();
        if (detonateTimer >= DETONATE_DELAY)
        {
            detonating = false;
            exploded   = true;
        }
    }
    else if (exploded)
    {
        explosionTimer += GetFrameTime();
        if (explosionTimer >= EXPLOSION_DURATION)
            done = true;
    }
}

void Bomb::Detonate()
{
    if (!detonating && !exploded)
        detonating = true;
}

void Bomb::Draw()
{
    const float S  = (float)Map::TILE_SIZE;
    const int   cx = Map::OFFSET_X + (int)(position.x * S);
    const int   cy = Map::OFFSET_Y + (int)(position.y * S);
    const int   C  = (int)S / 2;   // centre offset

    if (!exploded)
    {
        // --- Pulse animation ---
        float speed = detonating ? 22.0f : 10.0f;
        float time  = detonating ? detonateTimer : 0.0f;
        float pulse = 1.0f + 0.15f * sinf(time * speed);
        float sz    = S * pulse;
        int   bx    = (int)(cx + (S - sz) / 2);
        int   by    = (int)(cy + (S - sz) / 2);

        // Shadow
        DrawEllipse(cx + C, cy + (int)S - 4, (int)S / 3, 3,
                    Fade(BLACK, 0.25f));

        // Bomb body (black rounded rect)
        DrawRectangleRounded(
            {(float)bx, (float)by, sz, sz}, 0.2f, 6, Fade(BLACK, 0.9f));

        // Fuse spark on top
        float sparkAngle = time * 15.0f;
        int sparkX = cx + C + (int)(cosf(sparkAngle) * 6.0f);
        int sparkY = by - 2 + (int)(sinf(sparkAngle * 0.7f) * 4.0f);
        DrawCircle(sparkX, sparkY, 3, YELLOW);
        DrawCircle(sparkX, sparkY, 1, WHITE);

        // Bright centre dot
        DrawCircle(cx + C, cy + C, 5, YELLOW);
        DrawCircle(cx + C, cy + C, 2, Fade(WHITE, 0.7f));

        // Warning ring when detonating
        if (detonating)
        {
            float warnAlpha = 0.5f + 0.5f * sinf(time * 30.0f);
            DrawRing({(float)(cx + C), (float)(cy + C)},
                     sz/2 + 4, sz/2 + 7, 0.0f, 360.0f, 16,
                     Fade(RED, warnAlpha));
        }
    }
    else
    {
        // --- Explosion with alpha fade ---
        float progress = explosionTimer / EXPLOSION_DURATION;  // 0 → 1
        float alpha    = 1.0f - progress;

        // Flash: bright white centre that fades quickly
        if (progress < 0.3f)
        {
            float flashA = (1.0f - progress / 0.3f) * 0.6f;
            DrawRectangle(cx - (int)S, cy - (int)S, (int)S * 3, (int)S * 3,
                          Fade(WHITE, flashA));
        }

        Color fireOuter = { 255, 80,  0,   (unsigned char)(220 * alpha) };
        Color fireInner = { 255, 180, 30,  (unsigned char)(230 * alpha) };
        Color centerCol = { 255, 220, 100, (unsigned char)(240 * alpha) };

        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                int tx = (int)position.x + dx;
                int ty = (int)position.y + dy;
                int tpx = Map::OFFSET_X + tx * (int)S;
                int tpy = Map::OFFSET_Y + ty * (int)S;

                Color col;
                if (dx == 0 && dy == 0)
                    col = centerCol;
                else if (abs(dx) + abs(dy) == 1)
                    col = fireInner;
                else
                    col = fireOuter;

                DrawRectangle(tpx, tpy, (int)S, (int)S, col);

                // Small ember particles
                int seed = (tx * 3 + ty * 7) % 5;
                if (seed == 0)
                    DrawCircle(tpx + 8 + seed*3, tpy + 8 + seed*2, 2,
                               Fade(YELLOW, alpha * 0.6f));
            }
        }
    }
}

bool Bomb::HasExploded() const               { return done; }
bool Bomb::IsExploding() const               { return exploded && !done; }
bool Bomb::HasExplosionBeenProcessed() const { return explosionProcessed; }
void Bomb::MarkExplosionProcessed()          { explosionProcessed = true; }

Vector2 Bomb::GetGridPos() const { return position; }

std::vector<Vector2> Bomb::GetExplosionCells(const Map& map) const
{
    std::vector<Vector2> cells;
    int bx = (int)position.x;
    int by = (int)position.y;

    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int tx = bx + dx;
            int ty = by + dy;

            // Walls block — only walkable cells take damage
            if (tx >= 0 && tx < Map::WIDTH && ty >= 0 && ty < Map::HEIGHT
                && map.IsWalkable(tx, ty))
            {
                cells.push_back({(float)tx, (float)ty});
            }
        }
    }
    return cells;
}
