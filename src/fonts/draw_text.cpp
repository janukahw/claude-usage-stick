#include "draw_text.h"

#ifdef BOARD_TDISPLAY_S3_AMOLED

static inline const lv_font_fmt_txt_dsc_t *txt_dsc(const lv_font_t *font) {
    return (const lv_font_fmt_txt_dsc_t *)font->dsc;
}

static inline bool glyph_lookup(const lv_font_t *font, uint8_t c,
                                const lv_font_fmt_txt_glyph_dsc_t *&out_g) {
    const lv_font_fmt_txt_dsc_t *dsc = txt_dsc(font);
    if (!dsc || dsc->cmap_num == 0) return false;
    const lv_font_fmt_txt_cmap_t &cmap = dsc->cmaps[0];
    if (c < cmap.range_start) return false;
    uint32_t local = c - cmap.range_start;
    if (local >= cmap.range_length) return false;
    out_g = &dsc->glyph_dsc[local + cmap.glyph_id_start];
    return true;
}

int stringWidth(const char *text, const lv_font_t *font) {
    if (!text || !font) return 0;
    int w = 0;
    for (const char *p = text; *p; ++p) {
        const lv_font_fmt_txt_glyph_dsc_t *g;
        if (!glyph_lookup(font, (uint8_t)*p, g)) continue;
        // adv_w is scaled by 16 (LVGL convention)
        w += (g->adv_w + 8) >> 4;
    }
    return w;
}

int drawString(TFT_eSprite &dst, int x, int y, const char *text,
               const lv_font_t *font, uint16_t color) {
    if (!text || !font) return x;
    const lv_font_fmt_txt_dsc_t *dsc = txt_dsc(font);
    if (!dsc) return x;

    const int stride = dst.width();
    const int dh = dst.height();
    uint16_t *buf = (uint16_t *)dst.getPointer();
    if (!buf) return x;

    const uint8_t fr = (color >> 11) & 0x1F;
    const uint8_t fg = (color >> 5)  & 0x3F;
    const uint8_t fb =  color        & 0x1F;

    const int line_height = font->line_height;
    const int base_line   = font->base_line;

    int cursor = x;
    for (const char *p = text; *p; ++p) {
        uint8_t c = (uint8_t)*p;
        const lv_font_fmt_txt_glyph_dsc_t *g;
        if (!glyph_lookup(font, c, g)) continue;

        if (g->box_w == 0 || g->box_h == 0) {
            cursor += (g->adv_w + 8) >> 4;
            continue;
        }

        const uint8_t *bmp = &dsc->glyph_bitmap[g->bitmap_index];

        // Top-left pixel of the glyph in sprite coords.
        const int gx0 = cursor + g->ofs_x;
        const int gy0 = y + line_height - base_line - g->box_h - g->ofs_y;

        for (int py = 0; py < g->box_h; py++) {
            int yy = gy0 + py;
            if (yy < 0 || yy >= dh) continue;
            for (int px = 0; px < g->box_w; px++) {
                int xx = gx0 + px;
                if (xx < 0 || xx >= stride) continue;

                int pixel_idx = py * g->box_w + px;
                int byte_idx  = pixel_idx >> 1;
                uint8_t alpha = (pixel_idx & 1) ? (bmp[byte_idx] & 0x0F) : (bmp[byte_idx] >> 4);
                if (alpha == 0) continue;

                uint16_t *slot = &buf[yy * stride + xx];
                if (alpha == 15) {
                    uint16_t out = color;
                    *slot = (uint16_t)((out >> 8) | (out << 8));
                    continue;
                }

                // Alpha-blend with existing sprite pixel (which is stored byte-swapped).
                uint16_t exist_be = *slot;
                uint16_t exist = (uint16_t)((exist_be >> 8) | (exist_be << 8));
                uint8_t br = (exist >> 11) & 0x1F;
                uint8_t bg = (exist >> 5)  & 0x3F;
                uint8_t bb =  exist        & 0x1F;
                uint8_t inv = 15 - alpha;
                uint8_t nr = (fr * alpha + br * inv + 7) / 15;
                uint8_t ng = (fg * alpha + bg * inv + 7) / 15;
                uint8_t nb = (fb * alpha + bb * inv + 7) / 15;
                uint16_t out = (uint16_t)((nr << 11) | (ng << 5) | nb);
                *slot = (uint16_t)((out >> 8) | (out << 8));
            }
        }

        cursor += (g->adv_w + 8) >> 4;
    }
    return cursor;
}

#endif
