#include "render.h"
#include "caption.h"
#include "desktop.h"
#include "display.h"
#include <string.h>

/* Run029 visual refresh: one shared badge for every desktop shortcut. */
static void eh_e_badge(uint16_t *f,int x,int y,int size,const EhPalette *p,int selected) {
    uint16_t navy=EH_RGB(7,19,39), blue=EH_RGB(16,47,83), glow=EH_RGB(104,223,255);
    eh_box(f,x,y,size,size,selected?glow:p->accent);
    eh_box(f,x+2,y+2,size-4,size-4,navy);
    eh_box(f,x+4,y+4,size-8,size-8,blue);
    int stroke=size>=30?4:2;
    int left=x+size/4, top=y+size/4, width=size/2, height=size/2;
    eh_box(f,left,top,stroke,height,glow);
    eh_box(f,left,top,width,stroke,glow);
    eh_box(f,left,top+height/2-stroke/2,width-stroke,stroke,glow);
    eh_box(f,left,top+height-stroke,width,stroke,glow);
}


static void draw_tile(uint16_t *f, const EhTile *tile, const EhRegistry *apps, const EhPalette *p, int selected) {
    int x = tile->center - 34, y = tile->top;
    if (selected) {
        eh_box(f,x,y,68,63,p->accent);
        eh_box(f,x+1,y+1,66,61,p->selected);
    }
    eh_e_badge(f,tile->center-16,y+1,32,p,selected);
    /* Eight glyphs per line fit the 68-pixel card even in the four-column row. */
    const char *label = tile->slot == EH_FILES_SLOT ? "Files" : apps->slots[tile->slot].label;
    char first[EH7_LABEL_CAP], second[EH7_LABEL_CAP];
    eh_caption(label,8,first,second);
    eh_text(f,tile->center-(int)strlen(first)*4,y+36,first,p->ink,8,1);
    eh_text(f,tile->center-(int)strlen(second)*4,y+47,second,p->ink,8,1);
}

void eh7_render(uint16_t *f, const EhUI7 *ui, const EhRegistry *apps) {
    if (!f || !ui || !apps) return;
    EhPalette p = eh_palette(apps->appearance);
    eh_box(f,0,0,320,240,p.background);
    eh_box(f,0,0,320,52,p.panel);
    /* Matching E logo beside EndraHome, using the same tiny badge routine. */
    eh_e_badge(f,14,10,34,&p,0);
    eh_text(f,55,12,"EndraHome",p.ink,9,2);
    eh_text(f,56,32,"Launcher",p.ink,8,1);
    eh_text(f,240,18,"Run 029",p.ink,7,1);
    eh_box(f,0,51,320,1,p.accent);
    EhTile tiles[EH7_SLOT_COUNT];
    int count = eh_desktop_layout(apps,tiles);
    for (int i=0; i<count; ++i) draw_tile(f,&tiles[i],apps,&p,tiles[i].slot==ui->selected);
    eh_box(f,0,215,320,25,p.panel);
    eh_text(f,14,224,"Enter  Open",p.ink,11,1);
    eh_text(f,171,224,"Menu  Manage apps",p.ink,17,1);
}
