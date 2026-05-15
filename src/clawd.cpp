// Adapted from github.com/HermannBjorgvin/Clawdmeter — see CREDITS.md.
#include "clawd.h"

#ifdef BOARD_TDISPLAY_S3_AMOLED

#include "clawd_animations.h"
#include "usage_rate.h"
#include <Arduino.h>
#include <string.h>

#define GRID 20

// While Clawd is showing, auto-cycle to the next animation in the current
// rate-driven group every this many ms.
#define CLAWD_ROTATE_INTERVAL_MS 20000

#define GROUP_COUNT 4
#define GROUP_MAX   4
static int8_t  group_lists[GROUP_COUNT][GROUP_MAX];
static uint8_t group_size[GROUP_COUNT] = {0};
static uint8_t group_rotation[GROUP_COUNT] = {0};

static const char* GROUP_NAMES[GROUP_COUNT][GROUP_MAX] = {
    { "expression sleep", "idle breathe", "idle blink", "expression wink" },
    { "idle look around", "work think", "work coding", NULL },
    { "dance sway", "expression surprise", "dance bounce", NULL },
    { "dance bounce dj", "dance sway dj", "dance djmix", NULL },
};

static uint16_t cur_anim = 0;
static uint16_t cur_frame = 0;
static uint32_t frame_started_ms = 0;
static uint32_t last_pick_ms = 0;

static void resolve_group_lists() {
    for (int g = 0; g < GROUP_COUNT; g++) {
        group_size[g] = 0;
        for (int s = 0; s < GROUP_MAX; s++) {
            group_lists[g][s] = -1;
            const char* want = GROUP_NAMES[g][s];
            if (!want) continue;
            for (int i = 0; i < SPLASH_ANIM_COUNT; i++) {
                if (strcmp(splash_anims[i].name, want) == 0) {
                    group_lists[g][group_size[g]++] = (int8_t)i;
                    break;
                }
            }
        }
    }
}

static void pick_for_current_rate() {
    int g = usage_rate_group();
    if (g < 0 || g >= GROUP_COUNT) g = 0;
    if (group_size[g] == 0) return;

    uint8_t slot = group_rotation[g] % group_size[g];
    group_rotation[g]++;
    int8_t idx = group_lists[g][slot];
    if (idx < 0) return;

    cur_anim = (uint16_t)idx;
    cur_frame = 0;
    frame_started_ms = millis();
    last_pick_ms = frame_started_ms;
}

void clawd_init() {
    resolve_group_lists();
    pick_for_current_rate();
}

bool clawd_tick() {
    if (SPLASH_ANIM_COUNT == 0) return false;

    bool changed = false;
    if (millis() - last_pick_ms >= CLAWD_ROTATE_INTERVAL_MS) {
        pick_for_current_rate();
        changed = true;
    }

    const splash_anim_def_t *a = &splash_anims[cur_anim];
    if (a->frame_count == 0) return changed;

    uint16_t hold = a->holds[cur_frame];
    if (millis() - frame_started_ms >= hold) {
        cur_frame = (cur_frame + 1) % a->frame_count;
        frame_started_ms = millis();
        changed = true;
    }
    return changed;
}

void clawd_draw(TFT_eSprite &dst, int x, int y, int cell) {
    if (SPLASH_ANIM_COUNT == 0) return;
    const splash_anim_def_t *a = &splash_anims[cur_anim];
    if (a->frame_count == 0) return;

    const uint8_t  *cells = a->frames[cur_frame];
    const uint16_t *palette = a->palette;
    const int span = GRID * cell;
    const int stride = dst.width();
    uint16_t *buf = (uint16_t *)dst.getPointer();
    if (!buf) return;

    // Clear the Clawd zone first so previous frame doesn't ghost through where
    // the new frame has palette index 0 (transparent in source art).
    for (int dy = 0; dy < span; dy++) {
        uint16_t *row = &buf[(y + dy) * stride + x];
        for (int dx = 0; dx < span; dx++) row[dx] = 0x0000;
    }

    for (int gy = 0; gy < GRID; gy++) {
        for (int gx = 0; gx < GRID; gx++) {
            uint8_t code = cells[gy * GRID + gx];
            if (code == 0) continue;       // 0 = transparent — leave background
            uint16_t color = (code < SPLASH_PALETTE_SIZE) ? palette[code] : 0x0000;
            // Honor sprite swap-bytes — palette colors are stored as native
            // RGB565, but the AMOLED expects big-endian, and we have
            // setSwapBytes(true) set on the sprite. Direct buffer writes
            // bypass that, so swap manually.
            color = (uint16_t)((color >> 8) | (color << 8));
            int bx = x + gx * cell;
            int by = y + gy * cell;
            for (int dy = 0; dy < cell; dy++) {
                uint16_t *row = &buf[(by + dy) * stride + bx];
                for (int dx = 0; dx < cell; dx++) row[dx] = color;
            }
        }
    }
}

#endif
