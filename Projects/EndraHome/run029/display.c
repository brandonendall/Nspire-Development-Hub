#include "display.h"
#include "font8x8_basic.h"
#include <string.h>

const char *const eh_theme_names[] = {"Ocean", "Blue", "Charcoal", "Slate", "Ruby", "Forest", "Violet"};
const char *const eh_folder_names[] = {"Amber", "Sky", "Mint", "Lilac", "Coral", "Silver", "Lime"};
const char *const eh_font_names[] = {"White", "Ice", "Mint", "Lemon", "Pink", "Lilac", "Peach"};

static unsigned checked(unsigned n) { return n < EH_COLOR_COUNT ? n : 0; }
static uint16_t blend(uint16_t a, uint16_t b, unsigned percent) {
    unsigned inv = 100 - percent;
    unsigned r = ((a >> 11) * inv + (b >> 11) * percent) / 100;
    unsigned g = (((a >> 5) & 63) * inv + ((b >> 5) & 63) * percent) / 100;
    unsigned blue = ((a & 31) * inv + (b & 31) * percent) / 100;
    return (uint16_t)((r << 11) | (g << 5) | blue);
}
EhPalette eh_palette(EhAppearance a) {
    static const uint16_t accents[] = {
        EH_RGB(61,205,214), EH_RGB(93,165,252), EH_RGB(163,179,189), EH_RGB(163,185,212),
        EH_RGB(245,119,125), EH_RGB(86,214,148), EH_RGB(182,146,247)
    };
    static const uint16_t folders[] = {
        EH_RGB(245,189,86), EH_RGB(110,196,250), EH_RGB(112,226,174), EH_RGB(200,165,251),
        EH_RGB(253,151,124), EH_RGB(206,218,226), EH_RGB(191,219,110)
    };
    static const uint16_t fonts[] = {
        EH_RGB(247,249,252), EH_RGB(177,231,255), EH_RGB(175,252,212), EH_RGB(255,236,151),
        EH_RGB(255,193,218), EH_RGB(226,204,255), EH_RGB(255,210,172)
    };
    EhPalette p;
    p.accent = accents[checked(a.theme)];
    p.folder = folders[checked(a.folder)];
    p.ink = fonts[checked(a.font)];
    p.background = blend(EH_RGB(9,13,20), p.accent, 4);
    p.panel = blend(EH_RGB(14,20,28), p.accent, 9);
    p.raised = blend(EH_RGB(18,25,34), p.accent, 15);
    p.selected = blend(EH_RGB(14,22,32), p.accent, 26);
    return p;
}

void eh_box(uint16_t *f, int x, int y, int w, int h, uint16_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (w > EH_SCREEN_WIDTH - x) w = EH_SCREEN_WIDTH - x;
    if (h > EH_SCREEN_HEIGHT - y) h = EH_SCREEN_HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    for (int row = y; row < y + h; ++row)
        for (int col = x; col < x + w; ++col)
            f[row * EH_SCREEN_WIDTH + col] = color;
}

void eh_text(uint16_t *f, int x, int y, const char *s, uint16_t ink, int limit, int scale) {
    for (int i = 0; i < limit && s[i]; ++i) {
        unsigned ch = (unsigned char)s[i];
        if (ch < 32 || ch >= 127) ch = '?';
        for (int row = 0; row < 8; ++row) {
            unsigned bits = (unsigned char)font8x8_basic[ch][row];
            for (int col = 0; col < 8; ++col)
                if (bits & (1u << col))
                    eh_box(f, x + (i * 8 + col) * scale, y + row * scale, scale, scale, ink);
        }
    }
}

void eh_fit_text(uint16_t *f, int x, int y, const char *s, uint16_t ink, int cols) {
    if (cols <= 0) return;
    if ((int)strlen(s) <= cols || cols < 4) {
        eh_text(f, x, y, s, ink, cols, 1);
        return;
    }
    eh_text(f, x, y, s, ink, cols - 3, 1);
    eh_text(f, x + 8 * (cols - 3), y, "...", ink, 3, 1);
}

void eh_folder_icon(uint16_t *f, int x, int y, int s, const EhPalette *p) {
    eh_box(f,x+s,y+2*s,7*s,4*s,p->folder);
    eh_box(f,x,y+5*s,16*s,10*s,blend(p->folder,0,25));
    eh_box(f,x,y+7*s,16*s,7*s,p->folder);
    eh_box(f,x+s,y+7*s,14*s,s,blend(p->folder,0xffff,45));
}

void eh_app_icon(uint16_t *f, int x, int y, int s, const EhPalette *p) {
    eh_box(f,x,y+s,16*s,14*s,p->accent);
    eh_box(f,x+s,y+4*s,14*s,10*s,p->panel);
    eh_box(f,x+2*s,y+2*s,s,s,p->panel);
    eh_box(f,x+4*s,y+2*s,s,s,p->panel);
    for (int col=0; col<4; ++col)
        eh_box(f,x+(6+col)*s,y+(6+col/2)*s,s,(6-col)*s,p->accent);
}
