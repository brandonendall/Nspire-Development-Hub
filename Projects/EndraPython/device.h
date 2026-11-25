#pragma once
#include <stdint.h>

#define EP_WIDTH 320
#define EP_HEIGHT 240
#define EP_TICKS_PER_SECOND 32768u

void device_reset(void);
void device_draw_text(uint16_t *pixels, int x, int y, int size, uint16_t color, const char *text);
void device_present(const uint16_t *pixels);
int device_key_held(int key);
uint32_t device_ticks(void);
int device_delay(unsigned ms);
