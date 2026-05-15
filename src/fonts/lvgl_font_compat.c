// Empty stub bodies for the LVGL function-pointer slots referenced by the
// imported font .c files. The slots are never invoked — draw_text.cpp walks
// the lv_font_fmt_txt_dsc_t struct directly.
#include "lvgl_font_compat.h"

void lv_font_get_glyph_dsc_fmt_txt(void) {}
void lv_font_get_bitmap_fmt_txt(void) {}
