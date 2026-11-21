#ifndef ENDRAHOME_ART_H
#define ENDRAHOME_ART_H

#include "display.h"

#define EH_ICON_SIZE 48

extern const char *const eh_wallpaper_names[EH_WALLPAPER_COUNT];
extern const char *const eh_icon_names[EH_ICON_COUNT];

void eh_wallpaper_paint(uint16_t *frame, EhAppearance appearance);
void eh_wallpaper_preview(uint16_t *frame, EhAppearance appearance, int x, int y);
void eh_icon_paint(uint16_t *frame, EhAppearance appearance, int x, int y);
void eh_header_paint(uint16_t *frame, EhAppearance appearance);

#endif
