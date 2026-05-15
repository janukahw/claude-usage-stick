// Icon helpers — render Lucide icons from the lucide_icons.h arrays to the
// AMOLED framebuffer sprite. Handles byte-swap (sprite has setSwapBytes(true))
// and nearest-neighbor 2:1 downsample for 48→24 px since 48x48 icons are too
// large for the 240-pixel-wide right panel.
//
// Two source formats coexist in lucide_icons.h:
//  - RGB565 packed uint16_t arrays (bluetooth, trash) — drawIcon24
//  - RGB888 unpacked uint8_t arrays (battery family)  — drawBatteryIcon24
#pragma once
#include <stdint.h>

#ifdef BOARD_TDISPLAY_S3_AMOLED
  #include <TFT_eSPI.h>

  // RGB565 source. Pixels matching transparent_color (typically 0x18E3 for
  // Clawdmeter-converted icons) are skipped.
  void drawIcon24(TFT_eSprite &dst, int x, int y, const uint16_t *src48, uint16_t transparent_color = 0x18E3);

  // RGB888 source (3 bytes per pixel). Pure white pixels (255,255,255) are
  // treated as the icons' source background and skipped.
  void drawBatteryIcon24(TFT_eSprite &dst, int x, int y, const uint8_t *src48_rgb888);

  // Pick the right battery-state icon (charging > full > medium > low > empty).
  const uint8_t *batteryIcon(int pct, bool charging);
#endif
