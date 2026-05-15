// Bitmap-font text rendering for the AMOLED sprite.
//
// Uses the LVGL-format font data shipped in font_styrene_*.c / font_tiempos_*.c
// — see lvgl_font_compat.h for the layout. Renders 4bpp alpha glyphs with
// per-pixel alpha-blending onto the sprite's framebuffer, honoring the
// sprite's setSwapBytes(true) setting via manual byte-swap.
#pragma once
#include "lvgl_font_compat.h"

#ifdef BOARD_TDISPLAY_S3_AMOLED
  #include <TFT_eSPI.h>

  // Render `text` with top-left at (x, y). `color` is RGB565 native (not
  // byte-swapped). Returns the cursor x advance after rendering.
  int drawString(TFT_eSprite &dst, int x, int y, const char *text,
                 const lv_font_t *font, uint16_t color);

  // Measure the pixel width of `text` in the given font.
  int stringWidth(const char *text, const lv_font_t *font);

  // Fonts available — declared here, defined in their respective .c files.
  extern "C" const lv_font_t font_styrene_20;
  extern "C" const lv_font_t font_styrene_48;
  extern "C" const lv_font_t font_tiempos_34;
#endif
