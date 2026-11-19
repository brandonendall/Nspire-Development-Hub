#include "render.h"
#include "display.h"
#include "manager.h"
#include "overlay.h"
#include "files.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint16_t guarded[320*240+2], *frame=guarded+1;
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
    guarded[0]=0x1881;guarded[320*240+1]=0x8118;
    EhUniversalUI ui;eh7_ui_init(&ui,&r);
    eh7_render(frame,&ui.page,&r);
    assert(color_count(p.ink)>50 && color_count(p.folder)>20);
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
        assert(color_count(p.ink)>60);
        if(!screen)assert(color_count(p.folder)>10);
    }
    eh_toast_set(&ui.notice,"Appearance saved");eh7_overlay(frame,&ui);
    eh_text(frame,317,237,"wide",p.ink,4,2);
    assert(guarded[0]==0x1881 && guarded[320*240+1]==0x8118);
    puts("PASS tinted text, icon colors, settings preview and framebuffer bounds");
    return 0;
}
