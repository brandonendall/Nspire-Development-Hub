#include "art.h"
#include <stddef.h>

enum {
    PALETTE_COLORS = 256,
    PALETTE_BYTES = PALETTE_COLORS * 2,
    WALL_BYTES = PALETTE_BYTES + EH_SCREEN_WIDTH * EH_SCREEN_HEIGHT,
    ICON_BYTES = PALETTE_BYTES + EH_ICON_SIZE * EH_ICON_SIZE * 2,
    HEADER_W = 156,
    HEADER_H = 29,
    PREVIEW_W = 112,
    PREVIEW_H = 84
};
extern const uint8_t eh_art_data[];

const char *const eh_wallpaper_names[EH_WALLPAPER_COUNT] = {
    "Cyber Wave", "Neon City", "Cyber Lab", "Reactor", "Tech Corridor", "Solid White", "Solid Black"
};
const char *const eh_icon_names[EH_ICON_COUNT] = {
    "Core Cube", "Nexus Ring", "Shield Core", "Tech Gear", "Hex Fusion",
    "Orbital E", "Layer Stack", "Crystal Core", "Chip Matrix", "Dragon Core"
};

static unsigned checked(unsigned value, unsigned count) {
    return value < count ? value : 0;
}

/* Every theme maps the source chroma. Neutral reflections retain their range.
 * Keep Blue saturated and give the neutral themes distinct value ranges. */
static uint16_t tint(EhAppearance a, EhPalette p, uint16_t color) {
    if (a.theme==EH_WHITE){
        unsigned gray=eh_luma(color);
        unsigned light=gray+(255-gray)*42/100;
        return EH_RGB(light,light,light);
    }
    uint16_t accent=a.theme==EH_BLUE?EH_RGB(27,179,255):p.accent;
    if(a.theme==EH_BLACK)accent=EH_RGB(255,255,255);
    uint16_t tinted=eh_tint(color,accent);
    if (a.theme == EH_BLACK)
        tinted = (uint16_t)(((tinted & 0xf7deu) >> 1) + ((tinted & 0xe79cu) >> 2));
    return tinted;
}

static void load_palette(uint16_t table[PALETTE_COLORS], const uint8_t *asset,
                         EhAppearance a) {
    EhPalette p = eh_palette(a);
    for (unsigned i = 0; i < PALETTE_COLORS; ++i) {
        uint16_t source = (uint16_t)(asset[2 * i] | asset[2 * i + 1] << 8);
        table[i] = tint(a, p, source);
    }
}

static const uint8_t *wallpaper(EhAppearance a, uint16_t **colors) {
    static uint16_t palette[PALETTE_COLORS];
    static unsigned last_wall = EH_WALLPAPER_COUNT, last_theme = EH_THEME_COUNT;
    unsigned wall = checked(a.wallpaper, EH_ART_WALLPAPERS);
    a.theme = checked(a.theme, EH_THEME_COUNT);
    const uint8_t *asset = eh_art_data + wall * WALL_BYTES;
    if (wall != last_wall || a.theme != last_theme) {
        load_palette(palette, asset, a);
        last_wall = wall;
        last_theme = a.theme;
    }
    *colors = palette;
    return asset + PALETTE_BYTES;
}

void eh_wallpaper_paint(uint16_t *frame, EhAppearance a) {
    if (!frame) return;
    if(a.wallpaper==EH_SOLID_WHITE||a.wallpaper==EH_SOLID_BLACK){
        eh_box(frame,0,0,EH_SCREEN_WIDTH,EH_SCREEN_HEIGHT,a.wallpaper==EH_SOLID_WHITE?UINT16_MAX:0);
        return;
    }
    uint16_t *colors;
    const uint8_t *pixels = wallpaper(a, &colors);
    for (unsigned i = 0; i < EH_SCREEN_WIDTH * EH_SCREEN_HEIGHT; ++i)
        frame[i] = colors[pixels[i]];
}

void eh_wallpaper_preview(uint16_t *frame, EhAppearance a, int x, int y) {
    if (!frame || x < 0 || y < 0 || x > EH_SCREEN_WIDTH - PREVIEW_W ||
        y > EH_SCREEN_HEIGHT - PREVIEW_H) return;
    if(a.wallpaper==EH_SOLID_WHITE||a.wallpaper==EH_SOLID_BLACK){
        eh_box(frame,x,y,PREVIEW_W,PREVIEW_H,a.wallpaper==EH_SOLID_WHITE?UINT16_MAX:0);
        return;
    }
    uint16_t *colors;
    const uint8_t *pixels = wallpaper(a, &colors);
    for (int row = 0; row < PREVIEW_H; ++row) {
        const uint8_t *source = pixels + (row * EH_SCREEN_HEIGHT / PREVIEW_H) * EH_SCREEN_WIDTH;
        uint16_t *dest = frame + (y + row) * EH_SCREEN_WIDTH + x;
        for (int col = 0; col < PREVIEW_W; ++col)
            dest[col] = colors[source[col * EH_SCREEN_WIDTH / PREVIEW_W]];
    }
}

static uint16_t over(uint16_t back, uint16_t front, unsigned alpha) {
    unsigned inv = 255 - alpha;
    unsigned r = ((back >> 11) * inv + (front >> 11) * alpha + 127) / 255;
    unsigned g = (((back >> 5) & 63) * inv + ((front >> 5) & 63) * alpha + 127) / 255;
    unsigned b = ((back & 31) * inv + (front & 31) * alpha + 127) / 255;
    return (uint16_t)((r << 11) | (g << 5) | b);
}

static void draw_masked(uint16_t *frame, const uint8_t *asset, const uint16_t *colors,
                        int x, int y, int w, int h) {
    if (!frame) return;
    const uint8_t *pixels = asset + PALETTE_BYTES, *opacity = pixels + w * h;
    for (int row = 0; row < h; ++row) {
        if ((unsigned)(y + row) >= EH_SCREEN_HEIGHT) continue;
        for (int col = 0; col < w; ++col) {
            if ((unsigned)(x + col) >= EH_SCREEN_WIDTH) continue;
            int i = row * w + col;
            unsigned alpha = opacity[i];
            if (!alpha) continue;
            uint16_t *dest = frame + (y + row) * EH_SCREEN_WIDTH + x + col;
            uint16_t front = colors[pixels[i]];
            *dest = alpha == 255 ? front : over(*dest, front, alpha);
        }
    }
}

void eh_icon_paint(uint16_t *frame, EhAppearance a, int x, int y) {
    static uint16_t palette[PALETTE_COLORS];
    static unsigned last_icon = EH_ICON_COUNT, last_theme = EH_THEME_COUNT;
    unsigned icon = checked(a.icon, EH_ICON_COUNT);
    if(a.icon_color && a.icon_color<=EH_THEME_COUNT)a.theme=a.icon_color-1;
    a.theme = checked(a.theme, EH_THEME_COUNT);
    const uint8_t *asset = eh_art_data + EH_ART_WALLPAPERS * WALL_BYTES + icon * ICON_BYTES;
    if (icon != last_icon || a.theme != last_theme) {
        load_palette(palette, asset, a);
        last_icon = icon;
        last_theme = a.theme;
    }
    draw_masked(frame, asset, palette, x, y, EH_ICON_SIZE, EH_ICON_SIZE);
}

void eh_header_paint(uint16_t *frame, EhAppearance a) {
    static uint16_t palette[PALETTE_COLORS];
    static unsigned last_theme = EH_THEME_COUNT;
    a.theme = checked(a.theme, EH_THEME_COUNT);
    const uint8_t *asset = eh_art_data + EH_ART_WALLPAPERS * WALL_BYTES +
                           EH_ICON_COUNT * ICON_BYTES;
    if (a.theme != last_theme) {
        load_palette(palette, asset, a);
        last_theme = a.theme;
    }
    draw_masked(frame, asset, palette, 8, 5, HEADER_W, HEADER_H);
}
