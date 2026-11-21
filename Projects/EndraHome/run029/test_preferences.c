#include "preferences.h"
#include "registry.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void write_text(const char *path,const char *s) {
    FILE *f=fopen(path,"wb"); assert(f); assert(fputs(s,f)>=0); assert(!fclose(f));
}
int main(void) {
    char dir[]="/tmp/eh29prefXXXXXX",cfg[128],live[128],bak[128],stage[128],legacy[128];
    assert(mkdtemp(dir));
    snprintf(cfg,sizeof(cfg),"%s/%s",dir,EH7_CONFIG_FILE);
    snprintf(live,sizeof(live),"%s/%s",dir,EH_PREFS_FILE);
    snprintf(bak,sizeof(bak),"%s/EHLook3Old.cfg.tns",dir);
    snprintf(stage,sizeof(stage),"%s/EHLook3New.cfg.tns",dir);
    snprintf(legacy,sizeof(legacy),"%s/%s",dir,EH_LEGACY_PREFS_FILE);
    const char *original="# My shortcuts\n3=Files|/documents/Manager.tns\nT=4";
    write_text(cfg,original);
    EhRegistry r; eh7_registry_init(&r);
    assert(eh7_registry_load(&r,cfg)==1 && r.appearance.theme==4);
    assert(!eh_prefs_load(cfg,&r.appearance) && r.appearance.theme==4);
    EhAppearance new={1,2,3,4,9},out={0};
    write_text(legacy,"v1 4 2 3\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==4 && out.folder==2 && out.font==3);
    assert(out.wallpaper==0 && out.icon==0);
    write_text(legacy,"v2 14 19 17\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==14 && out.folder==19 && out.font==17);
    write_text(live,"v3 5 7 2 999 8\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==5 && out.folder==7 && out.font==2 &&
           out.wallpaper==0 && out.icon==8);
    write_text(live,"v3 14 3 8 2 broken\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==14 && out.wallpaper==2 && out.icon==0);
    write_text(live,"v3 7 9 11\n");
    assert(eh_prefs_load(cfg,&out) && out.theme==7 && out.wallpaper==0 && out.icon==0);
    unlink(live);

    assert(eh_prefs_save(cfg,new)); assert(eh_prefs_load(cfg,&out));
    assert(out.theme==1 && out.folder==2 && out.font==3 && out.wallpaper==4 && out.icon==9);
    assert(!eh_prefs_save(cfg,(EhAppearance){.font=EH_COLOR_COUNT}));
    assert(eh_prefs_save(cfg,(EhAppearance){19,18,17,3,8}));
    assert(eh_prefs_load(cfg,&out) && out.theme==19 && out.folder==18 && out.font==17 && out.wallpaper==3 && out.icon==8);
    assert(eh_prefs_save(cfg,new));
    write_text(stage,"incomplete"); assert(rename(live,bak)==0);
    assert(eh_prefs_load(cfg,&out) && out.font==3);
    new.font=5; assert(eh_prefs_save(cfg,new));
    assert(eh_prefs_load(cfg,&out) && out.font==5);
    assert(rename(live,bak)==0); write_text(live,"invalid");
    assert(eh_prefs_load(cfg,&out) && out.font==5);
    assert(eh_prefs_save(cfg,(EhAppearance){0}));
    assert(eh_prefs_load(cfg,&out) && !out.theme && !out.folder && !out.font && !out.wallpaper && !out.icon);
    char buffer[128]={0}; FILE *f=fopen(cfg,"rb"); assert(f);
    assert(fread(buffer,1,sizeof(buffer)-1,f)==strlen(original)); assert(!fclose(f));
    assert(!strcmp(original,buffer)); assert(!eh_prefs_save("/missing",(EhAppearance){0}));
    memset(buffer,0,sizeof(buffer)); f=fopen(legacy,"rb"); assert(f);
    assert(fread(buffer,1,sizeof(buffer)-1,f)==strlen("v2 14 19 17\n")); assert(!fclose(f));
    assert(!strcmp(buffer,"v2 14 19 17\n"));
    unlink(legacy); unlink(live); unlink(cfg); assert(rmdir(dir)==0);
    puts("PASS appearance persistence, legacy config, interrupted saves, defaults");
    return 0;
}
