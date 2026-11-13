#ifndef NX_PLATFORM_H
#define NX_PLATFORM_H
#include <stdint.h>
enum { K_UP=256,K_DOWN,K_LEFT,K_RIGHT,K_ENTER,K_ESC,K_TAB,K_DELETE,K_MENU };
int platform_init(void);
void platform_close(void);
void platform_present(uint16_t *pixels);
int platform_key(void);
int platform_cancel(void);
int platform_launch(const char *path);
void platform_info(char *out,unsigned length);
#endif
