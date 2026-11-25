#pragma once
#include <stdint.h>

#define EP_WIDTH 320
#define EP_HEIGHT 240
#define EP_TICKS_PER_SECOND 32768u
#define EP_SCRIPT_KEY_CAP 64
#define EP_KEY_SHIFT 512
#define EP_KEY_CTRL 513

void device_reset(void);
void device_draw_text(uint16_t *pixels, int x, int y, int size, uint16_t color, const char *text);
void device_present(const uint16_t *pixels);
int device_key_held(int key);
unsigned device_script_keys(int keys[EP_SCRIPT_KEY_CAP]);
uint32_t device_ticks(void);
int device_delay(unsigned ms);
void device_measure_text(const char *text, int font, int *width, int *height);
void device_rasterize_text(uint8_t *pixels, int width, int height, const char *text,
                           int font, uint16_t color, int antialias, int transparent);
