#include "render.h"
#include "display.h"
#include "art.h"
#include "file_icons.h"
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
    r.appearance=(EhAppearance){EH_RED,2,1,0,0};
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
    /* Low contrast used to paint a solid white rectangle behind Files. */
    EhAppearance caption_look={.theme=EH_BLACK,.font=7,.wallpaper=4,.icon=8};
    EhPalette caption_palette=eh_palette(caption_look);
    eh_box(frame,0,0,320,240,caption_palette.ink);
    eh_icon_caption(frame,30,30,"Files",&caption_palette);
    assert(color_count(caption_palette.ink)==320*240);
    for(unsigned theme=0;theme<EH_THEME_COUNT;theme++){
        caption_look.theme=theme;
        r.appearance=caption_look;
        ui.page.selected=-1;
        eh7_render(raw_wallpaper,&ui.page,&r);
        ui.page.selected=EH_FILES_SLOT;
        eh7_render(frame,&ui.page,&r);
        int changed=0;
        for(int y=0;y<240;y++)for(int x=0;x<320;x++){
            if(frame[y*320+x]==raw_wallpaper[y*320+x])continue;
            assert(x>=center-20 && x<=center+20 && y>=top+49 && y<=top+62);
            assert(x<center-15 || x>=center+14 || y<top+52 || y>=top+59);
            changed++;
        }
        assert(changed>20);
    }
    r.appearance=(EhAppearance){EH_RED,2,1,0,0};
    uint32_t theme_hash[EH_THEME_COUNT];
    for(unsigned i=0;i<EH_THEME_COUNT;i++){
        eh_wallpaper_paint(frame,(EhAppearance){.theme=i});
        theme_hash[i]=fingerprint();
        for(unsigned j=0;j<i;j++)assert(theme_hash[j]!=theme_hash[i]);
    }
    EhPalette black=eh_palette((EhAppearance){.theme=EH_BLACK,.font=7,.folder=7});
    EhPalette white=eh_palette((EhAppearance){.theme=EH_WHITE});
    assert(eh_luma(white.panel)>eh_luma(black.panel)+125);
    eh_box(frame,0,0,320,240,black.panel);
    eh_small_text(frame,10,10,"Dark text",black.ink,1,9);
    assert(color_count(black.ink)>15 && color_count(EH_RGB(244,248,252))>15);
    assert(frame[9*320+9]==black.panel && frame[13*320+36]==black.panel);
    eh_box(frame,0,0,320,240,white.panel);
    eh_text(frame,10,10,"White",white.ink,5,1);
    assert(color_count(white.ink)>15 && color_count(EH_RGB(4,8,15))>15);

    static const char *const names[EH_FILE_KIND_COUNT]={
        "Folder.JPG.TNS","notes.TxT.TnS","pack.ZiP.TNS","shot.JpEg.TnS","guide.PDF.TNS",
        "look.CfG.TNS","list.CsV.TnS","game.GbZ.TnS","app.ZeHn.TnS","doc.TnS","data.bin"
    };
    uint32_t file_hash[EH_FILE_KIND_COUNT];
    assert(eh_file_kind("game.GBA.TNS",0)==EH_FILE_ROM);
    assert(eh_file_kind("helper.elf",0)==EH_FILE_PROGRAM);
    assert(eh_file_kind("EndraZIP.tns",0)==EH_FILE_TI);
    assert(eh_file_kind("data.bin.tns",0)==EH_FILE_TI);
    assert(eh_file_kind("noext",0)==EH_FILE_UNKNOWN);
    assert(eh_file_kind("fake.pdf.tns.bak",0)==EH_FILE_UNKNOWN);
    for(unsigned i=0;i<EH_FILE_KIND_COUNT;i++){
        assert(eh_file_kind(names[i],i==EH_FILE_FOLDER)==(EhFileKind)i);
        assert(eh_file_kind(names[i],1)==EH_FILE_FOLDER);
        eh_box(frame,0,0,320,240,white.panel);
        eh_file_kind_icon(frame,20,20,(EhFileKind)i,&white);
        file_hash[i]=fingerprint();
        for(unsigned j=0;j<i;j++)assert(file_hash[j]!=file_hash[i]);
        for(int yy=19;yy<=35;yy++)
            for(int xx=18;xx<=37;xx++)
                if(yy<20||yy>=35||xx<(i==EH_FILE_FOLDER?19:20)||xx>=(i==EH_FILE_FOLDER?37:36))assert(frame[yy*320+xx]==white.panel);
        eh_file_kind_icon(frame,-3,-2,(EhFileKind)i,&black);
        eh_file_kind_icon(frame,319,239,(EhFileKind)i,&black);
    }
    uint32_t wallpaper_hash[EH_WALLPAPER_COUNT], icon_hash[EH_ICON_COUNT];
    for(unsigned i=0;i<EH_WALLPAPER_COUNT;i++){
        EhAppearance a={.wallpaper=i};
        eh_wallpaper_paint(frame,a);
        wallpaper_hash[i]=fingerprint();
        for(unsigned j=0;j<i;j++)assert(wallpaper_hash[j]!=wallpaper_hash[i]);
        a.theme=EH_GREEN;eh_wallpaper_paint(frame,a);
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
        a.theme=EH_RED;
        eh_box(frame,0,0,320,240,0x1234);
        eh_icon_paint(frame,a,0,0);
        assert(fingerprint()!=icon_hash[i]);
        eh_icon_paint(frame,a,-1,-1);
        eh_icon_paint(frame,a,319,239);
    }
    eh7_ui_handle(&ui,EH_KEY_MENU);eh15_manager_render(frame,&ui);
    assert(color_count(p.ink)>100);
    ui.manager_item=EH_MANAGER_APPEARANCE;eh7_ui_handle(&ui,EH_KEY_ENTER);
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
    r.appearance.theme=EH_RED;eh17_files_render(frame,&files,&r);
    uint16_t red=frame[24*320+57];
    r.appearance.theme=0;eh17_files_render(frame,&files,&r);
    assert(frame[24*320+57]!=red);
    r.appearance.theme=EH_RED;
    char label[9];
    eh_clock_label(label,0);assert(!strcmp(label,"12:00 AM"));
    eh_clock_label(label,719);assert(!strcmp(label,"11:59 AM"));
    eh_clock_label(label,720);assert(!strcmp(label,"12:00 PM"));
    eh_clock_label(label,1439);assert(!strcmp(label,"11:59 PM"));
    eh_clock_label(label,-1);assert(!*label);
    ui.renaming=1;
    for(int n=0;n<=15;n++){
        memset(ui.rename_input,'A',n);ui.rename_input[n]=0;
        eh15_manager_render(frame,&ui);
        assert(frame[129*320+24+n*8]==eh_skin(&p,EH_RGB(55,219,252)));
    }
    eh_toast_set(&ui.notice,"Appearance saved");eh7_overlay(frame,&ui);
    eh_text(frame,317,237,"wide",p.ink,4,2);
    eh_small_text(frame,316,232,"ENDRA",EH_RGB(53,219,252),1,6);
    assert(guarded[0]==0x1881 && guarded[320*240+1]==0x8118);
    puts("PASS tinted text, icon colors, settings preview and framebuffer bounds");
    return 0;
}
