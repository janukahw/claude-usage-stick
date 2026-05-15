// Ported verbatim from github.com/HermannBjorgvin/Clawdmeter — see CREDITS.md.
#include "usage_rate.h"
#include <Arduino.h>

// Thresholds in %/min. A 5-hour (300 min) session ÷ 100% = 0.33 %/min to fill
// exactly at the same pace as the session itself resets — the "heavy" tier
// starts right there (filling in 4–5 hours).
#define RATE_THRESH_NORMAL  0.10f
#define RATE_THRESH_ACTIVE  0.20f
#define RATE_THRESH_HEAVY   0.33f

// Min span between oldest and newest sample before we trust the computed rate.
// At default 60s polling that's ~4 min of accumulated history before we leave
// the idle group on cold boot.
#define MIN_WINDOW_MS       240000UL

#define RING_SIZE 6

struct Sample { uint32_t ms; float pct; };

static Sample ring[RING_SIZE];
static uint8_t count = 0;
static uint8_t head  = 0;

static inline uint8_t oldest_idx(void) {
    return (head + RING_SIZE - count) % RING_SIZE;
}

static void usage_rate_reset(void) {
    count = 0;
    head  = 0;
}

void usage_rate_sample(float session_pct) {
    uint32_t now = millis();

    if (count > 0) {
        uint8_t latest = (head + RING_SIZE - 1) % RING_SIZE;
        // Session reset: pct dropped substantially. Restart tracking.
        if (session_pct + 5.0f < ring[latest].pct) {
            usage_rate_reset();
        }
    }

    ring[head] = { now, session_pct };
    head = (head + 1) % RING_SIZE;
    if (count < RING_SIZE) count++;
}

int usage_rate_group(void) {
    if (count < 2) return 0;

    uint8_t o = oldest_idx();
    uint8_t l = (head + RING_SIZE - 1) % RING_SIZE;
    uint32_t dt = ring[l].ms - ring[o].ms;
    if (dt < MIN_WINDOW_MS) return 0;

    float dp = ring[l].pct - ring[o].pct;
    if (dp < 0.0f) dp = 0.0f;
    float rate = dp * 60000.0f / (float)dt;

    if (rate < RATE_THRESH_NORMAL) return 0;
    if (rate < RATE_THRESH_ACTIVE) return 1;
    if (rate < RATE_THRESH_HEAVY)  return 2;
    return 3;
}
