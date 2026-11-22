#ifndef EH_GBA_UI_PLATFORM_H
#define EH_GBA_UI_PLATFORM_H
#include <stdint.h>
typedef enum { EH_KEY_NONE, EH_KEY_UP, EH_KEY_DOWN, EH_KEY_LEFT, EH_KEY_RIGHT, EH_KEY_ENTER, EH_KEY_ESC, EH_KEY_MENU } EhKey;
#define EH_W 320
#define EH_H 240
int eh_platform_start(void);
void eh_platform_stop(void);
uint16_t *eh_framebuffer(void);
void eh_platform_flip(void);
EhKey eh_platform_readkey(void);
void eh_platform_sleep(void);
/* Finger-slide delta from the most recent key scan; contact is returned. */
int eh_platform_touch_pan(int *dx,int *dy);
#endif
