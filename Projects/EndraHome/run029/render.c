#include "render.h"
#include "caption.h"
#include "desktop.h"
#include "display.h"
#include "asset031.h"
#include <string.h>
static uint16_t tint(uint16_t a,uint16_t b,int k){int v=255-k;return (uint16_t)((((((a>>11)*v+(b>>11)*k)/255)&31)<<11)|((((((a>>5)&63)*v+((b>>5)&63)*k)/255)&63)<<5)|((((a&31)*v+(b&31)*k)/255)&31));}
static void tile_draw(uint16_t *f,const EhTile *tile,const EhRegistry *apps,const EhPalette *p,int selected){
    int x=tile->center-35,y=tile->top;
    eh_box(f,x,y,70,65,selected?p->accent:p->panel);
    eh_box(f,x+1,y+1,68,63,selected?p->selected:p->background);
    eh_app_icon(f,tile->center-16,y+2,2);
    const char *label=tile->slot==EH_FILES_SLOT?"Files":(!apps->slots[tile->slot].path[0]?"Add App":apps->slots[tile->slot].label);
    char first[EH7_LABEL_CAP],second[EH7_LABEL_CAP];
    /* Full nine-character Favorites fits without a dangling final S. */
    int len=(int)strlen(label);
    if(len<=9) {
        eh_text(f,tile->center-len*4,y+39,label,p->ink,9,1);
    } else {
        eh_caption(label,8,first,second);
        eh_text(f,tile->center-(int)strlen(first)*4,y+37,first,p->ink,8,1);
        eh_text(f,tile->center-(int)strlen(second)*4,y+48,second,p->ink,8,1);
    }
}
void eh7_render(uint16_t *f,const EhUI7 *ui,const EhRegistry *apps){
    if(!f||!ui||!apps)return;
    EhPalette p=eh_palette(apps->appearance);
    /* Pixel-native 320x240 RGB565 background: static 240 scanline gradient, no animation or image decoding. */
    for(int y=0;y<240;++y){
        uint16_t col=tint(p.background,p.raised,y*82/239);
        eh_box(f,0,y,320,1,col);
    }
    /* Subtle horizontal ambient accent, precomputed only when the screen is drawn. */
    for(int y=53;y<210;++y){
        int strength=(y-53)*16/157;
        eh_box(f,0,y,320,2,tint(p.accent,p.background,180+strength));
    }
    eh_box(f,0,0,320,51,p.panel);
    eh_box(f,0,49,320,2,p.accent);
    eh031_cube(f,10,5);
    eh_text(f,57,10,"EndraHome",p.ink,9,1);
    eh_text(f,57,26,"Launcher",p.ink,8,1);
    eh_text(f,258,17,"R031",p.accent,4,1);
    EhTile tiles[EH7_SLOT_COUNT];
    int count=eh_desktop_layout(apps,tiles);
    for(int i=0;i<count;++i)tile_draw(f,&tiles[i],apps,&p,tiles[i].slot==ui->selected);
    eh_box(f,0,215,320,25,p.panel);
    eh_box(f,0,215,320,1,p.accent);
    eh_text(f,14,224,"Enter: Open",p.ink,11,1);
    eh_text(f,192,224,"Menu: Apps",p.ink,10,1);
}
