#include "display.h"
#include "font8x8_basic.h"
#include <string.h>

const char *const eh_theme_names[] = {"Ocean","Blue","Charcoal","Slate","Red","Green","Purple","Yellow","Orange","Gold"};
const char *const eh_folder_names[] = {"Yellow","Blue","Green","Purple","Red","White","Orange","Black","Cyan","Gold"};
const char *const eh_font_names[] = {"White","Blue","Green","Yellow","Red","Purple","Orange","Black","Cyan","Gold"};

static unsigned checked(unsigned n) { return n < EH_COLOR_COUNT ? n : 0; }
static uint16_t blend(uint16_t a, uint16_t b, unsigned percent) {
    unsigned inv = 100 - percent;
    unsigned r = ((a >> 11) * inv + (b >> 11) * percent) / 100;
    unsigned g = (((a >> 5) & 63) * inv + ((b >> 5) & 63) * percent) / 100;
    unsigned blue = ((a & 31) * inv + (b & 31) * percent) / 100;
    return (uint16_t)((r << 11) | (g << 5) | blue);
}
EhPalette eh_palette(EhAppearance a) {
    static const uint16_t accents[] = {EH_RGB(61,205,214),EH_RGB(93,165,252),EH_RGB(163,179,189),EH_RGB(163,185,212),EH_RGB(235,72,75),EH_RGB(65,200,115),EH_RGB(182,146,247),EH_RGB(255,216,55),EH_RGB(255,139,56),EH_RGB(231,184,65)};
    static const uint16_t folders[] = {EH_RGB(245,189,86),EH_RGB(110,196,250),EH_RGB(112,226,174),EH_RGB(200,165,251),EH_RGB(245,100,100),EH_RGB(247,249,252),EH_RGB(255,154,64),EH_RGB(30,35,42),EH_RGB(61,205,214),EH_RGB(231,184,65)};
    static const uint16_t fonts[] = {EH_RGB(247,249,252),EH_RGB(110,196,250),EH_RGB(112,226,174),EH_RGB(255,236,91),EH_RGB(255,125,130),EH_RGB(200,165,251),EH_RGB(255,174,98),EH_RGB(30,35,42),EH_RGB(103,222,255),EH_RGB(255,215,92)};
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

/* Adapted from the EndraHome universal E prototype at 72fca481.
 * One scalable mark serves desktop shortcuts, executable rows and the header. */
void eh_app_icon(uint16_t *f, int x, int y, int scale) {
    const int size = 16 * scale;
    const int edge = scale;
    const int stroke = scale + 1;
    const int inset = size / 4;
    const int width = size / 2;
    const uint16_t navy = EH_RGB(8,19,38), shade = EH_RGB(15,39,65);
    const uint16_t blue = EH_RGB(48,153,215), cyan = EH_RGB(103,222,255);
    eh_box(f,x,y,size,size,blue);
    eh_box(f,x+edge,y+edge,size-2*edge,size-2*edge,navy);
    eh_box(f,x+2*edge,y+2*edge,size-4*edge,size-4*edge,shade);
    int left = x + inset, top = y + inset;
    eh_box(f,left,top,stroke,width,cyan);
    eh_box(f,left,top,width,stroke,cyan);
    eh_box(f,left,top+(width-stroke)/2,width-stroke,stroke,cyan);
    eh_box(f,left,top+width-stroke,width,stroke,cyan);
}

void eh_brand_badge(uint16_t *frame, int x, int y) {
    const uint16_t top = EH_RGB(100,218,255), right = EH_RGB(27,84,150);
    for (int i=0; i<32; ++i) {
        int rise = i * 5 / 31;
        eh_box(frame,x+i,y+5-rise,1,rise+1,top);
    }
    for (int i=0; i<5; ++i)
        eh_box(frame,x+32+i,y+5-i,1,32,right);
    eh_app_icon(frame,x,y+6,2);
}

/* Run030: shaded theme-aware header cube and inexpensive distinct tile symbols. */
void eh_theme_badge(uint16_t *f,int x,int y,const EhPalette *p) {
    uint16_t bright=blend(p->accent,0xffff,55),dark=blend(p->accent,0,58);
    eh_box(f,x+5,y,26,3,bright);eh_box(f,x+2,y+3,32,29,dark);
    eh_box(f,x+2,y+3,29,26,p->accent);eh_box(f,x+5,y+6,23,20,p->raised);
    eh_box(f,x+10,y+9,4,15,bright);
    eh_box(f,x+10,y+9,14,3,bright);
    eh_box(f,x+10,y+15,12,3,bright);
    eh_box(f,x+10,y+21,14,3,bright);
    eh_box(f,x+31,y+4,3,27,dark);
}
void eh_theme_icon(uint16_t *f,int x,int y,int kind,const EhPalette *p) {
    uint16_t hi=blend(p->accent,0xffff,50),lo=blend(p->accent,0,50);
    eh_box(f,x,y,32,32,lo);eh_box(f,x+2,y+2,28,28,p->raised);
    eh_box(f,x+2,y+2,28,2,hi);eh_box(f,x+2,y+28,28,2,lo);
    switch(kind%7) {
    case 0: /* game controller */
        eh_box(f,x+5,y+11,22,12,p->accent);eh_box(f,x+8,y+15,9,3,hi);
        eh_box(f,x+11,y+12,3,9,hi);eh_box(f,x+21,y+14,3,3,hi);break;
    case 1: /* archive */
        eh_box(f,x+7,y+8,18,17,p->accent);eh_box(f,x+9,y+10,14,3,hi);
        eh_box(f,x+15,y+14,3,9,lo);break;
    case 2: /* terminal */
        eh_box(f,x+5,y+8,22,17,p->accent);eh_box(f,x+7,y+10,18,13,p->raised);
        eh_box(f,x+9,y+13,4,2,hi);eh_box(f,x+13,y+15,4,2,hi);
        eh_box(f,x+18,y+19,5,2,hi);break;
    case 3: /* folder */
        eh_folder_icon(f,x+5,y+5,1,p);break;
    case 4: /* image */
        eh_box(f,x+6,y+7,20,18,p->accent);eh_box(f,x+8,y+9,16,14,p->raised);
        eh_box(f,x+18,y+11,4,4,hi);eh_box(f,x+10,y+19,12,3,hi);break;
    case 5: /* tools */
        eh_box(f,x+14,y+7,4,18,p->accent);eh_box(f,x+10,y+7,12,4,hi);
        eh_box(f,x+11,y+23,10,3,hi);break;
    default: /* favorite */
        eh_box(f,x+14,y+7,4,18,hi);eh_box(f,x+8,y+13,16,5,hi);
        eh_box(f,x+11,y+18,10,4,p->accent);break;
    }
}
