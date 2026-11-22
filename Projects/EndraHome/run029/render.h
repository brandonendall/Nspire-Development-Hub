#ifndef EH010_RENDER_H
#define EH010_RENDER_H
/* Include local registry first: imported Run007 renderer header otherwise
 * selects obsolete Apps007 filename on the Help screen.
 */
#include "registry.h"
#include "../run005/logic7.h"
#include <stdint.h>
/* Safe framebuffer-only wallpaper and translucent overlay helpers. */
void eh_glass_panel(uint16_t *frame,int x,int y,int w,int h,unsigned opacity);
void eh_desktop_wallpaper(uint16_t *frame,EhAppearance appearance);
void eh_icon_caption(uint16_t *frame,int x,int y,const char *text,const EhPalette *p);
void eh_theme_glass(uint16_t *frame,int x,int y,int w,int h,unsigned alpha,const EhPalette *p);
void eh_desktop_clock(uint16_t *frame,int second,int hide_seconds);
void eh_clock_seconds_label(char label[12],int second);
void eh7_render(uint16_t *frame,const EhUI7 *ui,const EhRegistry *apps);
#endif
