#include "render.h"
#include "display.h"
#include "art.h"
#include "desktop.h"
#include "manager.h"
#include "overlay.h"
#include "files.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static uint16_t guarded[320*240+2], *frame=guarded+1;
static uint16_t raw_wallpaper[320*240];
static uint32_t fingerprint(void) {
    uint32_t hash=2166136261u;
    for(int i=0;i<320*240;i++) hash=(hash^frame[i])*16777619u;
    return hash;
}
static int color_count(uint16_t color) {
    int n=0;
    for(int i=0;i<320*240;i++) n+=frame[i]==color;
    return n;
}
int main(void) {
    EhRegistry r;eh7_registry_init(&r);
    r.appearance=(EhAppearance){4,2,1,0,0};
    EhPalette p=eh_palette(r.appearance);
    assert(p.folder!=p.ink && p.accent!=p.ink);
    /* Run030 theme-dependent icon and shaded badge. */
    eh_theme_icon(frame,0,0,0,&p);
    eh_theme_badge(frame,40,0,&p);
    const uint16_t mark=p.accent;
    assert(frame[0]!=0);
    guarded[0]=0x1881;guarded[320*240+1]=0x8118;
    EhUniversalUI ui;eh7_ui_init(&ui,&r);
    eh_desktop_wallpaper(raw_wallpaper,r.appearance);
    eh7_render(frame,&ui.page,&r);
    assert(frame[84*320+121]==raw_wallpaper[84*320+121]);

    assert(color_count(p.ink)>5);
    EhTile tiles[EH7_SLOT_COUNT];
    assert(eh_desktop_layout(&r,tiles)==1 && tiles[0].slot==EH_FILES_SLOT);
    int center=tiles[0].center, top=tiles[0].top;
    for(int y=0;y<240;y++)for(int x=0;x<320;x++)
        if(x<center-EH_ICON_SIZE/2 || x>=center+EH_ICON_SIZE/2 || y<top || y>top+64)
            assert(frame[y*320+x]==raw_wallpaper[y*320+x]);
    eh_desktop_wallpaper(frame,(EhAppearance){.folder=2,.font=1});
    assert(frame[200*320+160]!=raw_wallpaper[200*320+160]);
    (void)mark;
    uint32_t wallpaper_hash[EH_WALLPAPER_COUNT], icon_hash[EH_ICON_COUNT];
    for(unsigned i=0;i<EH_WALLPAPER_COUNT;i++){
        EhAppearance a={.wallpaper=i};
        eh_wallpaper_paint(frame,a);
        wallpaper_hash[i]=fingerprint();
        for(unsigned j=0;j<i;j++)assert(wallpaper_hash[j]!=wallpaper_hash[i]);
        a.theme=5;eh_wallpaper_paint(frame,a);
        assert(fingerprint()!=wallpaper_hash[i]);
    }
    for(unsigned i=0;i<EH_ICON_COUNT;i++){
        EhAppearance a={.icon=i};
        eh_box(frame,0,0,320,240,0x1234);
        eh_icon_paint(frame,a,0,0);
        assert(frame[0]==0x1234 && frame[47*320+47]==0x1234);
        assert(color_count(0x1234)>320*240-EH_ICON_SIZE*EH_ICON_SIZE);
        icon_hash[i]=fingerprint();
        for(unsigned j=0;j<i;j++)assert(icon_hash[j]!=icon_hash[i]);
        a.theme=14;
        eh_box(frame,0,0,320,240,0x1234);
        eh_icon_paint(frame,a,0,0);
        assert(fingerprint()!=icon_hash[i]);
        eh_icon_paint(frame,a,-1,-1);
        eh_icon_paint(frame,a,319,239);
    }
    eh7_ui_handle(&ui,EH_KEY_MENU);eh15_manager_render(frame,&ui);
    assert(color_count(p.ink)>100);
    ui.manager_item=4;eh7_ui_handle(&ui,EH_KEY_ENTER);
    eh7_ui_handle(&ui,EH_KEY_DOWN);eh7_ui_handle(&ui,EH_KEY_DOWN);
    eh7_ui_handle(&ui,EH_KEY_RIGHT);
    EhPalette preview=eh_palette(ui.preview);eh15_manager_render(frame,&ui);
    assert(color_count(preview.ink)>100);
    Eh13PickEntry entry={0};strcpy(entry.name,"Long folder title that needs ellipsis");entry.directory=1;
    Eh17Files files={0};files.active=1;files.nav.active=1;files.nav.show_all=1;
    files.nav.count=1;files.nav.items=&entry;strcpy(files.nav.dir,"/documents");
    for(int screen=0;screen<=4;screen++){
        if(screen==3)continue; /* Image pixels have their own decoder and rendering tests. */
        files.screen=screen;eh17_files_render(frame,&files,&r);
        assert(color_count(p.ink)>30);
        if(!screen)assert(color_count(p.folder)>10);
    }
    files.screen=0;files.menu_open=1;
    r.appearance.theme=14;eh17_files_render(frame,&files,&r);
    uint16_t red=frame[24*320+57];
    r.appearance.theme=0;eh17_files_render(frame,&files,&r);
    assert(frame[24*320+57]!=red);
    r.appearance.theme=14;
    ui.page.page=EH_PAGE_DETAIL;ui.subview=0;ui.manager_item=6;
    eh7_ui_handle(&ui,EH_KEY_ENTER);assert(ui.subview==2);
    eh7_ui_handle(&ui,EH_KEY_ENTER);assert(ui.edit_action==EH15_NATIVE_CHECK);
    ui.edit_action=0;ui.native_status=2;eh15_manager_render(frame,&ui);
    assert(color_count(eh_palette(r.appearance).ink)>50);
    eh_toast_set(&ui.notice,"Appearance saved");eh7_overlay(frame,&ui);
    eh_text(frame,317,237,"wide",p.ink,4,2);
    eh_small_text(frame,316,232,"ENDRA",EH_RGB(53,219,252),1,6);
    assert(guarded[0]==0x1881 && guarded[320*240+1]==0x8118);
    puts("PASS tinted text, icon colors, settings preview and framebuffer bounds");
    return 0;
}
