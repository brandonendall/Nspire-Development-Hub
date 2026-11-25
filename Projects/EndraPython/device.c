#include "device.h"
#include "app.h"
#include "runtime.h"
#include "py/runtime.h"
#include <stdlib.h>
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

#define GINT_TRANSPARENT (-1)
#define GINT_MAX_VERTICES 128

typedef struct { int x, y; } DrawPoint;

static int min_int(int a, int b) { return a < b ? a : b; }
static int max_int(int a, int b) { return a > b ? a : b; }

static int gint_color(mp_obj_t value) {
    mp_int_t color = mp_obj_get_int(value);
    if (color < GINT_TRANSPARENT || color > UINT16_MAX)
        mp_raise_ValueError(MP_ERROR_TEXT("expected RGB565 or C_NONE"));
    return color;
}

static DrawPoint draw_point(const mp_obj_t *args) {
    DrawPoint point = {coordinate(args[0]), coordinate(args[1])};
    return point;
}

static void draw_span(int left, int right, int y, int color) {
    if (color != GINT_TRANSPARENT && left <= right && y >= 0 && y < EP_HEIGHT)
        rectangle(left, y, right - left + 1, 1, color);
}

static void draw_line(DrawPoint a, DrawPoint b, int color) {
    if (color == GINT_TRANSPARENT) return;
    int steep = abs(b.y-a.y) > abs(b.x-a.x);
    int u0 = steep ? a.y : a.x, u1 = steep ? b.y : b.x;
    int v0 = steep ? a.x : a.y, v1 = steep ? b.x : b.y;
    if (u0 > u1) {
        int temp = u0; u0 = u1; u1 = temp;
        temp = v0; v0 = v1; v1 = temp;
    }
    if (u0 == u1) {
        draw_span(a.x, a.x, a.y, color);
        return;
    }
    int end = min_int(u1, (steep ? EP_HEIGHT : EP_WIDTH)-1);
    int delta = v1-v0, length = u1-u0;
    int round = delta < 0 ? -length/2 : length/2;
    // Clip the major axis before stepping. Distance off-screen cannot lengthen the loop.
    for (int u = max_int(0,u0); u <= end; u++) {
        int v = v0 + ((int64_t)(u-u0)*delta + round)/length;
        if (steep) draw_span(v, v, u, color);
        else draw_span(u, u, v, color);
    }
}

static void draw_polygon(const DrawPoint *points, size_t count, int fill, int border) {
    if (fill != GINT_TRANSPARENT) {
        int top = points[0].y, bottom = top;
        for (size_t i = 1; i < count; i++) {
            top = min_int(top,points[i].y);
            bottom = max_int(bottom,points[i].y);
        }
        for (int y = max_int(0,top); y < min_int(EP_HEIGHT,bottom); y++) {
            int64_t crossings[GINT_MAX_VERTICES];
            size_t used = 0;
            for (size_t i = 0, prev = count-1; i < count; prev = i++) {
                DrawPoint a = points[prev], b = points[i];
                if (a.y > b.y) { DrawPoint swap = a; a = b; b = swap; }
                if (y < a.y || y >= b.y) continue;
                int64_t x = (int64_t)a.x*65536 + (int64_t)(b.x-a.x)*(y-a.y)*65536/(b.y-a.y);
                size_t j = used++;
                while (j && crossings[j-1] > x) {
                    crossings[j] = crossings[j-1];
                    j--;
                }
                crossings[j] = x;
            }
            for (size_t i = 0; i+1 < used; i += 2) {
                int64_t left = crossings[i], right = crossings[i+1];
                int first = (left + (left > 0 ? 65535 : 0))/65536;
                int last = (right - (right < 0 ? 65535 : 0))/65536;
                draw_span(first,last,y,fill);
            }
        }
    }
    int edge = border == GINT_TRANSPARENT ? fill : border;
    for (size_t i = 0, prev = count-1; i < count; prev = i++)
        draw_line(points[prev],points[i],edge);
}

static uint64_t square(int value) {
    return (int64_t)value*value;
}

static int ellipse_extent(DrawPoint a, DrawPoint b, int y) {
    if (y < a.y || y > b.y) return -1;
    if (a.y == b.y) return a.x;
    int sum_x = a.x+b.x, sum_y = a.y+b.y;
    uint64_t w2 = square(b.x-a.x), h2 = square(b.y-a.y);
    uint64_t limit = w2*(h2 - square(2*y-sum_y));
    // Start with the center pixel(s), including rows at the ends of an even-width box.
    int lo = a.x, hi = a.x+(b.x-a.x)/2;
    while (lo < hi) {
        int x = lo+(hi-lo)/2;
        if (square(2*x-sum_x)*h2 <= limit) hi = x;
        else lo = x+1;
    }
    return lo;
}

static void draw_ellipse(DrawPoint a, DrawPoint b, int fill, int border) {
    int sum_x = a.x+b.x;
    for (int y = max_int(a.y,0); y <= min_int(b.y,EP_HEIGHT-1); y++) {
        int left = ellipse_extent(a,b,y), right = sum_x-left;
        if (border == GINT_TRANSPARENT) {
            draw_span(left,right,y,fill);
            continue;
        }
        int prev = ellipse_extent(a,b,y-1), next = ellipse_extent(a,b,y+1);
        if (y == a.y || y == b.y) {
            draw_span(left,right,y,border);
            continue;
        }
        int inner = max_int(left+1,max_int(prev,next));
        if (inner > sum_x-inner) draw_span(left,right,y,border);
        else {
            draw_span(left,inner-1,y,border);
            draw_span(inner,sum_x-inner,y,fill);
            draw_span(sum_x-inner+1,right,y,border);
        }
    }
}

static mp_obj_t gint_line(size_t n, const mp_obj_t *args) {
    (void)n;
    draw_line(draw_point(args),draw_point(args+2),gint_color(args[4]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gint_line_obj, 5, 5, gint_line);

static mp_obj_t gint_triangle(size_t n, const mp_obj_t *args) {
    (void)n;
    DrawPoint points[] = {draw_point(args),draw_point(args+2),draw_point(args+4)};
    draw_polygon(points,3,gint_color(args[6]),GINT_TRANSPARENT);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gint_triangle_obj, 7, 7, gint_triangle);

static mp_obj_t gint_poly(mp_obj_t vertices, mp_obj_t fill, mp_obj_t border) {
    size_t n;
    mp_obj_t *items;
    mp_obj_get_array(vertices,&n,&items);
    if (n < 6 || n > 2*GINT_MAX_VERTICES || n%2)
        mp_raise_ValueError(MP_ERROR_TEXT("expected 3 to 128 x,y pairs"));
    DrawPoint points[GINT_MAX_VERTICES];
    for (size_t i = 0; i < n/2; i++) points[i] = draw_point(items+2*i);
    draw_polygon(points,n/2,gint_color(fill),gint_color(border));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(gint_poly_obj, gint_poly);

static mp_obj_t gint_ellipse(size_t n, const mp_obj_t *args) {
    (void)n;
    DrawPoint a = draw_point(args), b = draw_point(args+2);
    DrawPoint top_left = {min_int(a.x,b.x),min_int(a.y,b.y)};
    DrawPoint bottom_right = {max_int(a.x,b.x),max_int(a.y,b.y)};
    draw_ellipse(top_left,bottom_right,gint_color(args[4]),gint_color(args[5]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gint_ellipse_obj, 6, 6, gint_ellipse);

static mp_obj_t gint_circle(size_t n, const mp_obj_t *args) {
    (void)n;
    DrawPoint center = draw_point(args);
    int r = coordinate(args[2]);
    if (r < 0) mp_raise_ValueError(MP_ERROR_TEXT("radius must be nonnegative"));
    DrawPoint a = {center.x-r,center.y-r}, b = {center.x+r,center.y+r};
    draw_ellipse(a,b,gint_color(args[3]),gint_color(args[4]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gint_circle_obj, 5, 5, gint_circle);

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

static int text_face(mp_obj_t value){
    mp_int_t face=mp_obj_get_int(value);
    if(face<0 || face>2)mp_raise_ValueError(MP_ERROR_TEXT("invalid font face"));
    return (int)face;
}

static const char *font_text(mp_obj_t value){
    size_t length;
    const char *text=mp_obj_str_get_data(value,&length);
    if(length>256 || memchr(text,0,length) || memchr(text,'\n',length) || memchr(text,'\r',length))
        mp_raise_ValueError(MP_ERROR_TEXT("font text must be a short single line"));
    return text;
}

static mp_obj_t text_size(mp_obj_t value,mp_obj_t face){
    int w,h;
    device_measure_text(font_text(value),text_face(face),&w,&h);
    mp_obj_t size[]={MP_OBJ_NEW_SMALL_INT(w),MP_OBJ_NEW_SMALL_INT(h)};
    return mp_obj_new_tuple(2,size);
}
static MP_DEFINE_CONST_FUN_OBJ_2(text_size_obj,text_size);

static mp_obj_t rasterize_text(size_t n,const mp_obj_t *args){
    (void)n;
    mp_buffer_info_t buffer;
    mp_get_buffer_raise(args[0],&buffer,MP_BUFFER_WRITE);
    int w=coordinate(args[1]),h=coordinate(args[2]);
    const char *text=font_text(args[3]);
    int face=text_face(args[4]);
    mp_int_t color=mp_obj_get_int(args[5]);
    if(w<1 || h<1 || (size_t)w*h>buffer.len/2 || color<0 || color>65535)
        mp_raise_ValueError(MP_ERROR_TEXT("invalid text surface"));
    device_rasterize_text(buffer.buf,w,h,text,face,(uint16_t)color,
        mp_obj_is_true(args[6]),mp_obj_is_true(args[7]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rasterize_text_obj,8,8,rasterize_text);

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
    NAME(display), FUNC(_blit565, blit565_obj),
    FUNC(_gint_line,gint_line_obj), FUNC(_gint_triangle,gint_triangle_obj), FUNC(_gint_poly,gint_poly_obj),
    FUNC(_gint_ellipse,gint_ellipse_obj), FUNC(_gint_circle,gint_circle_obj),
    FUNC(_text_size,text_size_obj), FUNC(_rasterize_text,rasterize_text_obj), FUNC(color, rgb_obj), FUNC(set_pixel, set_pixel_obj), FUNC(get_pixel, get_pixel_obj),
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
