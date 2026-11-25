#include "device.h"
#include "app.h"
#include "runtime.h"
#include "py/runtime.h"
#include <string.h>

static uint16_t canvas[EP_WIDTH * EP_HEIGHT];
static bool canvas_ready;
static uint32_t last_tick;
static uint64_t elapsed_ticks;
#define TICKS_PERIOD (1u << 30)
#define TICKS_MASK (TICKS_PERIOD - 1)

void device_reset(void) {
    canvas_ready = false;
    elapsed_ticks = 0;
    last_tick = device_ticks();
}

static void init_canvas(void) {
    if (!canvas_ready) {
        memset(canvas, 0xff, sizeof canvas);
        canvas_ready = true;
    }
}

static int coordinate(mp_obj_t value) {
    mp_int_t n = mp_obj_get_int(value);
    if (n < -32768 || n > 32767) mp_raise_ValueError(MP_ERROR_TEXT("coordinate out of range"));
    return (int)n;
}

static unsigned channel(mp_obj_t value) {
    mp_int_t n = mp_obj_get_int(value);
    if (n < 0 || n > 255) mp_raise_ValueError(MP_ERROR_TEXT("RGB channel must be 0 to 255"));
    return (unsigned)n;
}

static mp_obj_t color_obj(mp_obj_t r, mp_obj_t g, mp_obj_t b) {
    mp_obj_t items[] = {
        MP_OBJ_NEW_SMALL_INT(channel(r)),
        MP_OBJ_NEW_SMALL_INT(channel(g)),
        MP_OBJ_NEW_SMALL_INT(channel(b)),
    };
    return mp_obj_new_tuple(3, items);
}
static MP_DEFINE_CONST_FUN_OBJ_3(rgb_obj, color_obj);

static uint16_t color565(mp_obj_t value) {
    mp_obj_t *rgb;
    mp_obj_get_array_fixed_n(value, 3, &rgb);
    return (channel(rgb[0]) >> 3) << 11 | (channel(rgb[1]) >> 2) << 5 | (channel(rgb[2]) >> 3);
}

static void rectangle(int x, int y, int w, int h, uint16_t color) {
    init_canvas();
    for (int row = y < 0 ? 0 : y; row < y + h && row < EP_HEIGHT; row++)
        for (int col = x < 0 ? 0 : x; col < x + w && col < EP_WIDTH; col++)
            canvas[row * EP_WIDTH + col] = color;
}

static mp_obj_t fill_rect(size_t n, const mp_obj_t *args) {
    (void)n;
    rectangle(coordinate(args[0]), coordinate(args[1]), coordinate(args[2]),
        coordinate(args[3]), color565(args[4]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fill_rect_obj, 5, 5, fill_rect);

static mp_obj_t set_pixel(size_t n, const mp_obj_t *args) {
    rectangle(coordinate(args[0]), coordinate(args[1]), 1, 1,
        n == 3 ? color565(args[2]) : 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(set_pixel_obj, 2, 3, set_pixel);

static mp_obj_t get_pixel(mp_obj_t xx, mp_obj_t yy) {
    int x = coordinate(xx), y = coordinate(yy);
    if (x < 0 || x >= EP_WIDTH || y < 0 || y >= EP_HEIGHT) return mp_const_none;
    init_canvas();
    unsigned c = canvas[y * EP_WIDTH + x];
    unsigned r = c >> 11, g = (c >> 5) & 63, b = c & 31;
    return color_obj(MP_OBJ_NEW_SMALL_INT((r << 3) | (r >> 2)),
        MP_OBJ_NEW_SMALL_INT((g << 2) | (g >> 4)), MP_OBJ_NEW_SMALL_INT((b << 3) | (b >> 2)));
}
static MP_DEFINE_CONST_FUN_OBJ_2(get_pixel_obj, get_pixel);

static mp_obj_t clear_screen(size_t n, const mp_obj_t *args) {
    rectangle(0, 0, EP_WIDTH, EP_HEIGHT, n ? color565(args[0]) : 0xffff);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clear_screen_obj, 0, 1, clear_screen);

static mp_obj_t draw_string(size_t n, const mp_obj_t *args) {
    int x = coordinate(args[0]), y = coordinate(args[1]), size = 1;
    const char *text = mp_obj_str_get_str(args[2]);
    uint16_t color = n >= 4 ? color565(args[3]) : 0;
    if (n == 5) {
        const char *font = mp_obj_str_get_str(args[4]);
        if (!strcmp(font, "small")) size = 0;
        else if (!strcmp(font, "large")) size = 2;
        else if (strcmp(font, "medium")) mp_raise_ValueError(MP_ERROR_TEXT("unknown font size"));
    }
    init_canvas();
    device_draw_text(canvas, x, y, size, color, text);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(draw_string_obj, 3, 5, draw_string);

/* Copy a validated big-endian RGB565 rectangle into the script canvas. */
static mp_obj_t blit565(size_t n, const mp_obj_t *args) {
    (void)n;
    mp_buffer_info_t source;
    mp_get_buffer_raise(args[0], &source, MP_BUFFER_READ);
    int stride = coordinate(args[1]);
    int sx = coordinate(args[2]), sy = coordinate(args[3]);
    int x = coordinate(args[4]), y = coordinate(args[5]);
    int w = coordinate(args[6]), h = coordinate(args[7]);
    if (stride <= 0 || sx < 0 || sy < 0 || x < 0 || y < 0 ||
        w <= 0 || h <= 0 || sx + w > stride ||
        x + w > EP_WIDTH || y + h > EP_HEIGHT ||
        (size_t)((sy + h - 1) * stride + sx + w) > source.len / 2)
        mp_raise_ValueError(MP_ERROR_TEXT("invalid blit bounds"));
    bool transparent = args[8] != mp_const_none;
    uint16_t key = transparent ? (uint16_t)mp_obj_get_int(args[8]) : 0;
    init_canvas();
    for (int row = 0; row < h; row++) {
        const uint8_t *src = (const uint8_t *)source.buf + 2 * ((size_t)(sy + row) * stride + sx);
        uint16_t *dst = canvas + (y + row) * EP_WIDTH + x;
        for (int col = 0; col < w; col++, src += 2) {
            uint16_t color = (uint16_t)src[0] << 8 | src[1];
            if (!transparent || color != key) dst[col] = color;
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(blit565_obj, 9, 9, blit565);

static mp_obj_t show_screen(void) {
    init_canvas();
    device_present(canvas);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(show_screen_obj, show_screen);

static mp_obj_t keydown(mp_obj_t value) {
    mp_int_t key = mp_obj_get_int(value);
    return mp_obj_new_bool(key >= UP && key <= MENU && device_key_held((int)key));
}
static MP_DEFINE_CONST_FUN_OBJ_1(keydown_obj, keydown);

static uint32_t current_ms(void) {
    uint32_t now = device_ticks();
    elapsed_ticks += (uint32_t)(now - last_tick);
    last_tick = now;
    return (uint32_t)(elapsed_ticks * 1000 / EP_TICKS_PER_SECOND) & TICKS_MASK;
}

static mp_obj_t ticks_ms(void) { return MP_OBJ_NEW_SMALL_INT(current_ms()); }
static MP_DEFINE_CONST_FUN_OBJ_0(ticks_ms_obj, ticks_ms);

static mp_obj_t ticks_diff(mp_obj_t a, mp_obj_t b) {
    uint32_t delta = (uint32_t)mp_obj_get_int(a) - (uint32_t)mp_obj_get_int(b);
    return MP_OBJ_NEW_SMALL_INT((int32_t)((delta + (TICKS_PERIOD >> 1)) & TICKS_MASK) - (int32_t)(TICKS_PERIOD >> 1));
}
static MP_DEFINE_CONST_FUN_OBJ_2(ticks_diff_obj, ticks_diff);

static void sleep_milliseconds(mp_int_t ms) {
    if (ms < 0 || ms >= (TICKS_PERIOD >> 1)) mp_raise_ValueError(MP_ERROR_TEXT("sleep out of range"));
    while (ms) {
        if (cancel_pressed()) mp_raise_type(&mp_type_KeyboardInterrupt);
        unsigned chunk = ms > 15 ? 15 : (unsigned)ms;
        if (!device_delay(chunk))
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("calculator timer stopped"));
        ms -= chunk;
    }
    current_ms();
}

static mp_obj_t sleep_ms(mp_obj_t value) {
    sleep_milliseconds(mp_obj_get_int(value));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sleep_ms_obj, sleep_ms);

static mp_obj_t sleep_seconds(mp_obj_t value) {
    mp_float_t seconds = mp_obj_get_float(value);
    if (!(seconds >= 0 && seconds < 536870)) mp_raise_ValueError(MP_ERROR_TEXT("sleep out of range"));
    sleep_milliseconds((mp_int_t)(seconds * 1000));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(sleep_obj, sleep_seconds);

#define NAME(q) {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_##q)}
#define FUNC(q, f) {MP_ROM_QSTR(MP_QSTR_##q), MP_ROM_PTR(&f)}
#define INTEGER(q, n) {MP_ROM_QSTR(MP_QSTR_##q), MP_ROM_INT(n)}
static const mp_rom_map_elem_t display_table[] = {
    NAME(display), FUNC(_blit565, blit565_obj), FUNC(color, rgb_obj), FUNC(set_pixel, set_pixel_obj), FUNC(get_pixel, get_pixel_obj),
    FUNC(fill_rect, fill_rect_obj), FUNC(clear_screen, clear_screen_obj),
    FUNC(draw_string, draw_string_obj), FUNC(show_screen, show_screen_obj),
    INTEGER(WIDTH, EP_WIDTH), INTEGER(HEIGHT, EP_HEIGHT),
};
static MP_DEFINE_CONST_DICT(display_dict, display_table);
const mp_obj_module_t ep_display_module = {.base={&mp_type_module}, .globals=(mp_obj_dict_t *)&display_dict};
MP_REGISTER_MODULE(MP_QSTR_display, ep_display_module);

static const mp_rom_map_elem_t keys_table[] = {
    NAME(keys), FUNC(keydown, keydown_obj), INTEGER(KEY_LEFT, LEFT), INTEGER(KEY_RIGHT, RIGHT),
    INTEGER(KEY_UP, UP), INTEGER(KEY_DOWN, DOWN), INTEGER(KEY_OK, ENTER),
    INTEGER(KEY_EXE, ENTER), INTEGER(KEY_MENU, MENU), INTEGER(KEY_ESC, ESC), INTEGER(KEY_EXIT, ESC),
};
static MP_DEFINE_CONST_DICT(keys_dict, keys_table);
const mp_obj_module_t ep_keys_module = {.base={&mp_type_module}, .globals=(mp_obj_dict_t *)&keys_dict};
MP_REGISTER_MODULE(MP_QSTR_keys, ep_keys_module);

static const mp_rom_map_elem_t time_table[] = {
    NAME(time), FUNC(ticks_ms, ticks_ms_obj), FUNC(ticks_diff, ticks_diff_obj),
    FUNC(sleep_ms, sleep_ms_obj), FUNC(sleep, sleep_obj),
};
static MP_DEFINE_CONST_DICT(time_dict, time_table);
const mp_obj_module_t ep_time_module = {.base={&mp_type_module}, .globals=(mp_obj_dict_t *)&time_dict};
MP_REGISTER_MODULE(MP_QSTR_time, ep_time_module);
