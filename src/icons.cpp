#include "icons.h"

#ifdef BOARD_TDISPLAY_S3_AMOLED

#include "lucide_icons.h"

static inline uint16_t rgb888_to_565(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint16_t)(r >> 3) << 11) | ((uint16_t)(g >> 2) << 5) | (b >> 3);
}

void drawIcon24(TFT_eSprite &dst, int x, int y, const uint16_t *src48, uint16_t transparent_color) {
    if (!src48) return;
    const int stride = dst.width();
    uint16_t *buf = (uint16_t *)dst.getPointer();
    if (!buf) return;

    for (int dy = 0; dy < 24; dy++) {
        for (int dx = 0; dx < 24; dx++) {
            uint16_t c = src48[(dy * 2) * 48 + (dx * 2)];
            if (c == transparent_color) continue;
            // Sprite has setSwapBytes(true) so the AMOLED expects big-endian.
            // Direct buffer writes bypass that; swap manually.
            uint16_t swapped = (uint16_t)((c >> 8) | (c << 8));
            buf[(y + dy) * stride + (x + dx)] = swapped;
        }
    }
}

void drawBatteryIcon24(TFT_eSprite &dst, int x, int y, const uint8_t *src48_rgb888) {
    if (!src48_rgb888) return;
    const int stride = dst.width();
    uint16_t *buf = (uint16_t *)dst.getPointer();
    if (!buf) return;

    for (int dy = 0; dy < 24; dy++) {
        for (int dx = 0; dx < 24; dx++) {
            int offset = ((dy * 2) * 48 + (dx * 2)) * 3;
            uint8_t r = src48_rgb888[offset + 0];
            uint8_t g = src48_rgb888[offset + 1];
            uint8_t b = src48_rgb888[offset + 2];
            // Skip the icon's white source background.
            if (r == 0xFF && g == 0xFF && b == 0xFF) continue;
            uint16_t c = rgb888_to_565(r, g, b);
            uint16_t swapped = (uint16_t)((c >> 8) | (c << 8));
            buf[(y + dy) * stride + (x + dx)] = swapped;
        }
    }
}

const uint8_t *batteryIcon(int pct, bool charging) {
    if (charging)    return icon_battery_charging_data;
    if (pct >= 70)   return icon_battery_full_data;
    if (pct >= 40)   return icon_battery_medium_data;
    if (pct >= 15)   return icon_battery_low_data;
    return icon_battery_data;
}

#endif
