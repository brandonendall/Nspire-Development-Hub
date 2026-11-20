#ifndef ENDRAHOME_DISPLAY_H
#define ENDRAHOME_DISPLAY_H

#include <stdint.h>

#define EH_SCREEN_WIDTH 320
#define EH_SCREEN_HEIGHT 240
#define EH_GLYPH_WIDTH 8
#define EH_COLOR_COUNT 20
#define EH_RGB(r,g,b) ((uint16_t)((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3)))

typedef struct { unsigned theme, folder, font; } EhAppearance;
typedef struct {
    uint16_t background, panel, raised, selected, accent, folder, ink;
} EhPalette;

extern const char *const eh_theme_names[EH_COLOR_COUNT];
extern const char *const eh_folder_names[EH_COLOR_COUNT];
extern const char *const eh_font_names[EH_COLOR_COUNT];

EhPalette eh_palette(EhAppearance appearance);
void eh_box(uint16_t *frame, int x, int y, int w, int h, uint16_t color);
void eh_text(uint16_t *frame, int x, int y, const char *text, uint16_t ink, int limit, int scale);
void eh_small_text(uint16_t *frame,int x,int y,const char *s,uint16_t ink,int scale,int max_chars);
void eh_fit_text(uint16_t *frame, int x, int y, const char *text, uint16_t ink, int columns);
void eh_folder_icon(uint16_t *frame, int x, int y, int scale, const EhPalette *colors);
void eh_app_icon(uint16_t *frame, int x, int y, int scale);
void eh_brand_badge(uint16_t *frame, int x, int y);
void eh_theme_badge(uint16_t *frame, int x, int y, const EhPalette *p);
void eh_theme_icon(uint16_t *frame, int x, int y, int kind, const EhPalette *p);

#endif
