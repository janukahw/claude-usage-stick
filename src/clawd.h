// Clawd: animated pixel-art mascot for the dashboard. Picks animations based
// on the usage_rate group, ticks frames, and renders into a target sprite.
//
// Adapted from github.com/HermannBjorgvin/Clawdmeter (firmware/src/splash.cpp).
// Removed: LVGL dependency, dedicated canvas buffer. Renders directly to
// the framebuffer sprite via direct uint16_t writes.
#pragma once
#include <stdint.h>

#ifdef BOARD_TDISPLAY_S3_AMOLED
  #include <TFT_eSPI.h>

  void clawd_init();
  bool clawd_tick();                                       // advance frame / cycle animation; returns true if frame changed
  void clawd_draw(TFT_eSprite &dst, int x, int y, int cell);  // render current frame at scale
#else
  // Cross-platform no-op stubs so call sites stay free of #ifdef.
  inline void clawd_init() {}
  inline bool clawd_tick() { return false; }
#endif
