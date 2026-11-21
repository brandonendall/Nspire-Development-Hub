#include "render.h"
#include "display.h"
#include "manager.h"
#include "overlay.h"
#include "files.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint16_t guarded[320*240+2], *frame=guarded+1;
static uint16_t raw_wallpaper[320*240];
static int color_count(uint16_t color) {
    int n=0;
    for(int i=0;i<320*240;i++) n+=frame[i]==color;
    return n;
}
int main(void) {
    EhRegistry r;eh7_registry_init(&r);
    r.appearance=(EhAppearance){4,2,1};
    EhPalette p=eh_palette(r.appearance);
    assert(p.folder!=p.ink && p.accent!=p.ink);
    /* Run030 theme-dependent icon and shaded badge. */
    eh_theme_icon(frame,0,0,0,&p);
    eh_theme_badge(frame,40,0,&p);
    const uint16_t mark=p.accent;
    assert(frame[0]!=0);
    guarded[0]=0x1881;guarded[320*240+1]=0x8118;
    EhUniversalUI ui;eh7_ui_init(&ui,&r);
    eh_wallpaper_paint(raw_wallpaper);
    eh7_render(frame,&ui.page,&r);
    /* Hero launcher: original artwork is untouched outside the icon and
       name, unlike the opaque 66x63 card used by Run035. */
    assert(frame[124*320+121]==raw_wallpaper[124*320+121]);
    assert(frame[124*320+160]!=raw_wallpaper[124*320+160]);
    /* Run035: original artwork header remains, fake Run031/96% readout is scrubbed. */
    assert(frame[15*320+265] == frame[15*320+211]);
    assert(color_count(EH_RGB(53,219,252))>20);
    (void)mark;
    eh7_ui_handle(&ui,EH_KEY_MENU);eh15_manager_render(frame,&ui);
    assert(color_count(p.ink)>100);
    ui.manager_item=4;eh7_ui_handle(&ui,EH_KEY_ENTER);
    eh7_ui_handle(&ui,EH_KEY_DOWN);eh7_ui_handle(&ui,EH_KEY_DOWN);
    eh7_ui_handle(&ui,EH_KEY_RIGHT);
    EhPalette preview=eh_palette(ui.preview);eh15_manager_render(frame,&ui);
    assert(color_count(preview.ink)>100 && !color_count(p.ink));
    Eh13PickEntry entry={0};strcpy(entry.name,"Long folder title that needs ellipsis");entry.directory=1;
    Eh17Files files={0};files.active=1;files.nav.active=1;files.nav.show_all=1;
    files.nav.count=1;files.nav.items=&entry;strcpy(files.nav.dir,"/documents");
    for(int screen=0;screen<=4;screen++){
        if(screen==3)continue; /* Image pixels have their own decoder and rendering tests. */
        files.screen=screen;eh17_files_render(frame,&files,&r);
        assert(color_count(p.ink)>60 || color_count(EH_RGB(245,251,255))>60);
        if(!screen)assert(color_count(p.folder)>10);
    }
    eh_toast_set(&ui.notice,"Appearance saved");eh7_overlay(frame,&ui);
    eh_text(frame,317,237,"wide",p.ink,4,2);
    eh_small_text(frame,316,232,"ENDRA",EH_RGB(53,219,252),1,6);
    assert(guarded[0]==0x1881 && guarded[320*240+1]==0x8118);
    puts("PASS tinted text, icon colors, settings preview and framebuffer bounds");
    return 0;
}
