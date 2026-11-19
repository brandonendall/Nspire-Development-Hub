#include "render.h"
#include "caption.h"
#include "desktop.h"
#include "display.h"
#include <string.h>

static void draw_tile(uint16_t *f, const EhTile *tile, const EhRegistry *apps, const EhPalette *p, int selected) {
    int x = tile->center - 34, y = tile->top;
    if (selected) {
        eh_box(f,x,y,68,63,p->accent);
        eh_box(f,x+1,y+1,66,61,p->selected);
    }
    if (tile->slot == EH_FILES_SLOT) eh_folder_icon(f,tile->center-16,y+1,2,p);
    else eh_app_icon(f,tile->center-16,y+1,2,p);
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
    /* The three faces retain the launcher cube at the compact header size. */
    for (int col = 0; col < 15; ++col) {
        int offset = col/2;
        eh_box(f,15+col,15-offset,1,2*offset+2,p.accent);
        eh_box(f,30+col,8+offset,1,16-2*offset,p.accent);
        eh_box(f,15+col,18+offset,1,13,p.selected);
        eh_box(f,30+col,25-offset,1,13,p.raised);
    }
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
