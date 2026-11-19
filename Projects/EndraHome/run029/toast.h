#ifndef ENDRAHOME_TOAST_H
#define ENDRAHOME_TOAST_H
#include <stdint.h>
#define EH_TICKS_PER_SECOND 32768u
typedef struct { char text[64]; uint32_t start; int pending; } EhToast;
void eh_toast_set(EhToast *toast, const char *message);
int eh_toast_tick(EhToast *toast, uint32_t now);
#endif
