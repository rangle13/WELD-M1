struct Bubble
{
    float x, y;
    float vx, vy;
    float radius;
    float maxRadius;
    uint16_t age;
    uint16_t maxAge;
    uint8_t generation;
    bool alive;
};

#define MAX_BUBBLES 64

extern Bubble bubbles[MAX_BUBBLES];
extern bool bubblesInit;
void bubbleBurstEffect();