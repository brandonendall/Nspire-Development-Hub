#include "preferences.h"
#include "registry.h"
#include "config_save.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void write_text(const char *path,const char *s) {
    FILE *f=fopen(path,"wb"); assert(f); assert(fputs(s,f)>=0); assert(!fclose(f));
}
int main(void) {
    char dir[]="/tmp/eh29prefXXXXXX",cfg[128],live[128],bak[128],stage[128],legacy[128],run038[128],sample[64];
    assert(mkdtemp(dir));
    snprintf(cfg,sizeof(cfg),"%s/%s",dir,EH7_CONFIG_FILE);
    snprintf(live,sizeof(live),"%s/%s",dir,EH_PREFS_FILE);
    snprintf(bak,sizeof(bak),"%s/EH40Old.cfg.tns",dir);
    snprintf(stage,sizeof(stage),"%s/EH40New.cfg.tns",dir);
    snprintf(legacy,sizeof(legacy),"%s/%s",dir,EH_LEGACY_PREFS_FILE);
    snprintf(run038,sizeof(run038),"%s/%s",dir,EH_RUN038_PREFS_FILE);
    const char *original="# My shortcuts\n3=Files|/documents/Manager.tns\nT=4";
    write_text(cfg,original);
    EhRegistry r; eh7_registry_init(&r);
    assert(eh7_registry_load(&r,cfg)==1 && r.appearance.theme==EH_RED);
    assert(!eh_prefs_load(cfg,&r.appearance) && r.appearance.theme==EH_RED);
    EhAppearance new={1,2,3,4,9,EH_GREEN+1},out={0};
    write_text(legacy,"v1 4 2 3\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==EH_RED && out.folder==2 && out.font==3);
    assert(out.wallpaper==0 && out.icon==0);
    write_text(legacy,"v2 14 19 17\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==EH_RED && out.folder==19 && out.font==17);
    write_text(run038,"v3 5 7 2 999 8\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==EH_GREEN && out.folder==7 && out.font==2 &&
           out.wallpaper==0 && out.icon==8);
    write_text(run038,"v3 14 3 8 2 broken\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==EH_RED && out.wallpaper==2 && out.icon==0);
    write_text(run038,"v3 7 9 11\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==EH_YELLOW && out.wallpaper==0 && out.icon==0);
    static const unsigned expected[]={EH_BLUE,EH_BLUE,EH_BLACK,EH_BLACK,EH_RED,
       EH_GREEN,EH_PURPLE,EH_YELLOW,EH_ORANGE,EH_YELLOW,EH_WHITE,EH_BLACK,
       EH_WHITE,EH_WHITE,EH_RED,EH_GREEN,EH_PURPLE,EH_BLUE,EH_BLUE,EH_ORANGE};
    for(unsigned theme=0;theme<EH_LEGACY_THEME_COUNT;theme++){
        snprintf(sample,sizeof(sample),"v3 %u 19 18 4 9\n",theme);
        write_text(run038,sample);
        assert(eh_prefs_load(cfg,&out) && out.theme==expected[theme] &&
               out.folder==19 && out.font==18 && out.wallpaper==4 && out.icon==9);
    }
    assert(access(live,F_OK)!=0);
    write_text(live,"v4 8 1 1 0 0\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==EH_ORANGE && out.icon==9);
    unlink(live);

    write_text(live,"v4 3 6 7 2 8\n");
    assert(eh_prefs_load(cfg,&r.appearance) && r.appearance.theme==EH_YELLOW);
    assert(eh14_assign_save(&r,0,"/documents/EndraGBA.tns",cfg,NULL));
    EhRegistry saved;eh7_registry_init(&saved);
    assert(eh7_registry_load(&saved,cfg)==2 && saved.has_appearance);
    assert(saved.appearance.theme==EH_YELLOW && saved.appearance.icon==8);
    assert(!strcmp(saved.slots[3].path,"/documents/Manager.tns"));
    assert(eh7_registry_parse(&r,"# clock-offset 1140\n")==1);
    assert(r.clock_offset_seconds==68400);
    assert(eh_save_clock(&r,68426,1,cfg,NULL));
    assert(!eh_save_clock(&r,120,0,"",NULL) && r.clock_offset_seconds==68426);
    assert(eh_save_appearance(&r,new,cfg,NULL));
    assert(!eh_save_appearance(&r,(EhAppearance){.font=EH_COLOR_COUNT},cfg,NULL));
    assert(!eh_save_appearance(&r,(EhAppearance){.theme=EH_THEME_COUNT},cfg,NULL));
    assert(!access(live,F_OK) && access(stage,F_OK)!=0 && access(bak,F_OK)!=0);
    eh7_registry_init(&saved);
    assert(eh7_registry_load(&saved,cfg)==2 && saved.errors==0 && saved.has_appearance);
    assert(saved.clock_offset_seconds==68426 && saved.hide_seconds);
    out=saved.appearance;
    assert(out.icon_color==EH_GREEN+1 && out.theme==EH_RED && out.folder==2 && out.font==3 && out.wallpaper==4 && out.icon==9);
    assert(!strcmp(saved.slots[0].path,"/documents/EndraGBA.tns"));
    assert(!strcmp(saved.slots[3].path,"/documents/Manager.tns"));
    assert(!rename(cfg,bak));
    assert(eh14_recover_config(cfg,NULL));
    assert(access(bak,F_OK)!=0);
    assert(eh_save_appearance(&r,(EhAppearance){0},cfg,NULL));
    eh7_registry_init(&saved);
    assert(eh7_registry_load(&saved,cfg)==2 && saved.has_appearance);
    assert(saved.clock_offset_seconds==68426 && saved.hide_seconds);
    out=saved.appearance;
    assert(!out.theme && !out.folder && !out.font && !out.wallpaper && !out.icon);
    char buffer[256]={0}; FILE *f=fopen(cfg,"rb"); assert(f);
    assert(fread(buffer,1,sizeof(buffer)-1,f)>0); assert(!fclose(f));
    char *appearance=strstr(buffer,EH_APPEARANCE_LINE);
    assert(strstr(buffer,"# My shortcuts") && appearance &&
           !strstr(appearance+sizeof(EH_APPEARANCE_LINE)-1,EH_APPEARANCE_LINE));
    f=fopen(live,"rb"); assert(f);
    assert(fgets(buffer,sizeof(buffer),f) && !strcmp(buffer,"v4 3 6 7 2 8\n"));
    assert(!fclose(f));
    memset(buffer,0,sizeof(buffer)); f=fopen(legacy,"rb"); assert(f);
    assert(fread(buffer,1,sizeof(buffer)-1,f)==strlen("v2 14 19 17\n")); assert(!fclose(f));
    assert(!strcmp(buffer,"v2 14 19 17\n"));
    FILE *prior=fopen(run038,"rb"); assert(prior);
    memset(buffer,0,sizeof(buffer)); assert(fgets(buffer,sizeof(buffer),prior));
    assert(!fclose(prior) && !strcmp(buffer,sample));
    unlink(run038); unlink(legacy); unlink(live); unlink(cfg); assert(rmdir(dir)==0);
    puts("PASS unified settings, legacy import, shortcut preservation, backup recovery and defaults");
    return 0;
}
