#include "display.h"
#include "font8x8_basic.h"
#include <string.h>

const char *const eh_theme_names[EH_THEME_COUNT] = {
    "Blue","Red","Green","Yellow","Orange","Purple","Black","White"
};
const char *const eh_folder_names[] = {"Yellow","Blue","Green","Purple","Red","White","Orange","Black","Cyan","Gold","Gray","Silver","Dark Red","Lime","Pink","Teal","Navy","Brown","Lavender","Charcoal"};
const char *const eh_font_names[] = {"White","Blue","Green","Yellow","Red","Purple","Orange","Black","Cyan","Gold","Gray","Silver","Dark Red","Lime","Pink","Teal","Navy","Brown","Lavender","Charcoal"};

static unsigned checked(unsigned n) { return n < EH_COLOR_COUNT ? n : 0; }
static uint16_t blend(uint16_t a, uint16_t b, unsigned percent) {
    unsigned inv = 100 - percent;
    unsigned r = ((a >> 11) * inv + (b >> 11) * percent) / 100;
    unsigned g = (((a >> 5) & 63) * inv + ((b >> 5) & 63) * percent) / 100;
    unsigned blue = ((a & 31) * inv + (b & 31) * percent) / 100;
    return (uint16_t)((r << 11) | (g << 5) | blue);
}
EhPalette eh_palette(EhAppearance a) {
    static const uint16_t accents[EH_THEME_COUNT] = {
      EH_RGB(61,205,214), EH_RGB(255,70,73), EH_RGB(52,225,92),
      EH_RGB(255,252,60), EH_RGB(255,123,25), EH_RGB(187,75,255),
      EH_RGB(138,145,153), EH_RGB(56,62,71)
    };
    static const uint16_t folders[] = {EH_RGB(245,189,86),EH_RGB(110,196,250),EH_RGB(112,226,174),EH_RGB(200,165,251),EH_RGB(245,100,100),EH_RGB(247,249,252),EH_RGB(255,154,64),EH_RGB(30,35,42),EH_RGB(61,205,214),EH_RGB(231,184,65),EH_RGB(125,130,140),EH_RGB(192,197,204),EH_RGB(145,30,45),EH_RGB(155,240,45),EH_RGB(255,130,180),EH_RGB(30,140,145),EH_RGB(20,55,110),EH_RGB(140,92,55),EH_RGB(190,160,240),EH_RGB(50,55,65)};
    static const uint16_t fonts[] = {EH_RGB(247,249,252),EH_RGB(110,196,250),EH_RGB(112,226,174),EH_RGB(255,236,91),EH_RGB(255,125,130),EH_RGB(200,165,251),EH_RGB(255,174,98),EH_RGB(30,35,42),EH_RGB(103,222,255),EH_RGB(255,215,92),EH_RGB(125,130,140),EH_RGB(192,197,204),EH_RGB(145,30,45),EH_RGB(155,240,45),EH_RGB(255,130,180),EH_RGB(30,140,145),EH_RGB(20,55,110),EH_RGB(140,92,55),EH_RGB(190,160,240),EH_RGB(50,55,65)};
    EhPalette p;
    p.theme = a.theme<EH_THEME_COUNT?a.theme:EH_BLUE;
    p.accent = accents[p.theme];
    p.folder = folders[checked(a.folder)];
    p.ink = fonts[checked(a.font)];
    p.background = blend(EH_RGB(9,13,20), p.accent, 4);
    p.panel = blend(EH_RGB(14,20,28), p.accent, 9);
    p.raised = blend(EH_RGB(18,25,34), p.accent, 15);
    p.selected = blend(EH_RGB(14,22,32), p.accent, 26);
    if(p.theme==EH_WHITE){
        p.background=EH_RGB(224,227,231);
        p.panel=EH_RGB(240,242,245);
        p.raised=EH_RGB(250,251,253);
        p.selected=EH_RGB(202,212,223);
    }
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

static int contrast_edge(const uint16_t *f,int x,int y,uint16_t c,uint16_t *edge){
    if((unsigned)x>=EH_SCREEN_WIDTH||(unsigned)y>=EH_SCREEN_HEIGHT)return 0;
    int fg=(int)eh_luma(c), bg=(int)eh_luma(f[y*EH_SCREEN_WIDTH+x]);
    *edge=fg>=128?EH_RGB(4,8,15):EH_RGB(244,248,252);
    return fg-bg<66 && bg-fg<66;
}

static void text_backing(uint16_t *f,int x,int y,const char *s,int count,
                         int pitch,int width,int height,uint16_t ink){
    int length=0;
    while(length<count&&s[length])++length;
    if(!length)return;
    int span=(length-1)*pitch+width;
    uint16_t edge;
    if(contrast_edge(f,x+span/4,y+height/2,ink,&edge) ||
       contrast_edge(f,x+span*3/4,y+height/2,ink,&edge))
        eh_box(f,x-1,y-1,span+2,height+2,edge);
}

static void glyph(uint16_t *f,int x,int y,const uint8_t *rows,unsigned w,unsigned h,
                  int scale,uint16_t ink,int reverse){
    for(unsigned row=0;row<h;++row)
        for(unsigned col=0;col<w;++col)
            if(rows[row]&(1u<<(reverse?w-1-col:col)))
                eh_box(f,x+(int)col*scale,y+(int)row*scale,scale,scale,ink);
}

void eh_text(uint16_t *f, int x, int y, const char *s, uint16_t ink, int limit, int scale) {
    text_backing(f,x,y,s,limit,8*scale,7*scale,8*scale,ink);
    for (int i = 0; i < limit && s[i]; ++i) {
        unsigned ch = (unsigned char)s[i];
        if (ch < 32 || ch >= 127) ch = '?';
        glyph(f,x+i*8*scale,y,(const uint8_t *)font8x8_basic[ch],8,8,scale,ink,0);
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
    uint16_t edge;
    if(contrast_edge(f,x+8*s,y+9*s,p->folder,&edge)){
        eh_box(f,x+s-1,y+2*s-1,7*s+2,4*s+2,edge);
        eh_box(f,x-1,y+5*s-1,16*s+2,11*s,edge);
    }
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

/* 5x7 native pixel font adapted from EndraZIP's compact on-screen glyphs.
 * All text is rasterized to the caller's 320x240 frame, never to the LCD
 * directly. This provides a lighter header than the chunky 8x8 type. */
typedef struct { char ch; uint8_t bits[7]; } EhMiniGlyph;
static const EhMiniGlyph mini34[]={
 {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}},
 {'C',{14,17,16,16,16,17,14}}, {'D',{30,17,17,17,17,17,30}},
 {'E',{31,16,16,30,16,16,31}}, {'F',{31,16,16,30,16,16,16}},
 {'G',{15,16,16,23,17,17,15}}, {'H',{17,17,17,31,17,17,17}},
 {'I',{31,4,4,4,4,4,31}}, {'J',{1,1,1,1,17,17,14}},
 {'K',{17,18,20,24,20,18,17}}, {'L',{16,16,16,16,16,16,31}},
 {'M',{17,27,21,21,17,17,17}}, {'N',{17,25,21,19,17,17,17}},
 {'O',{14,17,17,17,17,17,14}}, {'P',{30,17,17,30,16,16,16}},
 {'Q',{14,17,17,17,21,18,13}}, {'R',{30,17,17,30,20,18,17}},
 {'S',{15,16,16,14,1,1,30}}, {'T',{31,4,4,4,4,4,4}},
 {'U',{17,17,17,17,17,17,14}}, {'V',{17,17,17,17,17,10,4}},
 {'W',{17,17,17,21,21,21,10}}, {'X',{17,17,10,4,10,17,17}},
 {'Y',{17,17,10,4,4,4,4}}, {'Z',{31,1,2,4,8,16,31}},
 {'0',{14,17,19,21,25,17,14}}, {'1',{4,12,4,4,4,4,14}},
 {'2',{14,17,1,2,4,8,31}}, {'3',{30,1,1,14,1,1,30}},
 {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,30,1,1,17,14}},
 {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}},
 {'8',{14,17,17,14,17,17,14}}, {'9',{14,17,17,15,1,1,14}},
 {'.',{0,0,0,0,0,12,12}}, {'-',{0,0,0,31,0,0,0}},
 {'_',{0,0,0,0,0,0,31}}, {'/',{1,1,2,4,8,16,16}},
 {':',{0,12,12,0,12,12,0}}, {'>',{16,8,4,2,4,8,16}},
 {'<',{1,2,4,8,4,2,1}}, {'+',{0,4,4,31,4,4,0}},
 {' ',{0,0,0,0,0,0,0}}, {'?',{14,17,1,2,4,0,4}},
 {'&',{12,18,20,8,21,18,13}}, {'=',{0,31,0,31,0,0,0}},
};
void eh_small_text_plain(uint16_t *f,int x,int y,const char *s,uint16_t color,int scale,int max_chars){
 if(!f||!s||scale<1||scale>3||max_chars<1)return;
 for(int i=0;i<max_chars&&s[i];i++){
  unsigned char ch=(unsigned char)s[i];
  if(ch>='a'&&ch<='z')ch=(unsigned char)(ch-'a'+'A');
  const uint8_t *rows=NULL;
  for(size_t g=0;g<sizeof(mini34)/sizeof(mini34[0]);g++)
   if(mini34[g].ch==(char)ch){rows=mini34[g].bits;break;}
  if(!rows)rows=mini34[sizeof(mini34)/sizeof(mini34[0])-4].bits;
  glyph(f,x+i*6*scale,y,rows,5,7,scale,color,1);
 }
}

void eh_small_text(uint16_t *f,int x,int y,const char *s,uint16_t color,int scale,int max_chars){
 if(!f||!s||scale<1||scale>3||max_chars<1)return;
 text_backing(f,x,y,s,max_chars,6*scale,5*scale,7*scale,color);
 eh_small_text_plain(f,x,y,s,color,scale,max_chars);
}

/* Weighted luminance expanded from the RGB565 channels. */
unsigned eh_luma(uint16_t c){
    return ((c>>11)*14+((c>>5)&63)*11+(c&31)*4)/5;
}

uint16_t eh_tint(uint16_t c,uint16_t accent){
    unsigned r=c>>11,g=((c>>5)&63)>>1,b=c&31;
    unsigned hi=r>g?r:g;if(b>hi)hi=b;
    unsigned lo=r<g?r:g;if(b<lo)lo=b;
    unsigned ar=accent>>11,ag=((accent>>5)&63)>>1,ab=accent&31;
    unsigned peak=ar>ag?ar:ag;if(ab>peak)peak=ab;
    if(!peak){ar=ag=ab=31;peak=31;}
    unsigned d=hi-lo,rr=lo+d*ar/peak,gg=lo+d*ag/peak,bb=lo+d*ab/peak;
    return (uint16_t)((rr<<11)|((gg*2+(gg>>4))<<5)|bb);
}

uint16_t eh_skin(const EhPalette *p,uint16_t c){
    if(p->theme==EH_BLUE)return c;
    if(p->theme==EH_WHITE){
        unsigned level=252-eh_luma(c);
        return EH_RGB(level,level,level);
    }
    return eh_tint(c,p->accent);
}
