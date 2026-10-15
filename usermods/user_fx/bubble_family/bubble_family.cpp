
#include "wled.h"
#include "bubble_family.h"

Bubble bubbles[MAX_BUBBLES];
bool bubblesInit = false;

void resetBubble(Bubble &b, float cx, float cy, uint8_t gen)
{
    b.x = cx;
    b.y = cy;
    b.radius = 0.5f;
    b.maxRadius = 4.0f + gen * 2.0f;
    b.age = 0;
    b.maxAge = 40 + gen * 10;
    b.generation = gen;
    b.alive = true;

    float angle = (float)hw_random16() / 65535.0f * 6.2831853f;
    float speed = 0.3f + 0.2f * gen;
    b.vx = cosf(angle) * speed;
    b.vy = sinf(angle) * speed;
}

static void spawnChildren(const Bubble &parent)
{
    if (parent.generation >= 3)
        return;

    uint8_t children = 3 + parent.generation;
    for (uint8_t i = 0; i < children; i++)
    {
        for (uint8_t j = 0; j < MAX_BUBBLES; j++)
        {
            if (!bubbles[j].alive)
            {
                resetBubble(bubbles[j], parent.x, parent.y, parent.generation + 1);
                break;
            }
        }
    }
}

static void initBubbles()
{
    for (uint8_t i = 0; i < MAX_BUBBLES; i++)
    {
        bubbles[i].alive = false;
    }
    resetBubble(bubbles[0], 31.5f, 31.5f, 0);
    bubblesInit = true;
}

void bubbleBurstEffect(void)
{
    if (!bubblesInit)
        initBubbles();

    // Fade background
    for (uint16_t i = 0; i < SEGMENT.length(); i++)
    {
        CRGB c = CRGB(SEGMENT.getPixelColor(i));
        c.fadeToBlackBy(40);
        SEGMENT.setPixelColor(i, c.r, c.g, c.b);
    }

    // Update bubbles
    for (uint8_t i = 0; i < MAX_BUBBLES; i++)
    {
        Bubble &b = bubbles[i];
        if (!b.alive)
            continue;

        b.age++;
        b.radius += 0.15f;
        b.x += b.vx;
        b.y += b.vy;

        bool shouldBurst = (b.age > b.maxAge) || (b.radius > b.maxRadius);

        if (shouldBurst)
        {
            spawnChildren(b);
            b.alive = false;
            continue;
        }

        // Draw bubble
        for (uint8_t y = 0; y < 64; y++)
        {
            for (uint8_t x = 0; x < 64; x++)
            {
                float dx = x + 0.5f - b.x;
                float dy = y + 0.5f - b.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < b.radius)
                {
                    float edge = (b.radius - dist) / b.radius;
                    uint8_t bri = (uint8_t)(edge * 255);
                    uint32_t col = SEGMENT.color_from_palette(b.age * 3, false, bri, 0);
                    SEGMENT.setPixelColorXY(x, y, col);
                }
            }
        }
    }

    // Restart when all bubbles are gone
    bool anyAlive = false;
    for (uint8_t i = 0; i < MAX_BUBBLES; i++)
    {
        if (bubbles[i].alive)
        {
            anyAlive = true;
            break;
        }
    }
    if (!anyAlive)
        bubblesInit = false;

}