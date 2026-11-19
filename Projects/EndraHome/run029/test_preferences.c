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
    char dir[]="/tmp/eh29prefXXXXXX",cfg[128],live[128],bak[128],stage[128];
    assert(mkdtemp(dir));
    snprintf(cfg,sizeof(cfg),"%s/%s",dir,EH7_CONFIG_FILE);
    snprintf(live,sizeof(live),"%s/%s",dir,EH_PREFS_FILE);
    snprintf(bak,sizeof(bak),"%s/EHLookOld.cfg.tns",dir);
    snprintf(stage,sizeof(stage),"%s/EHLookNew.cfg.tns",dir);
    const char *original="# My shortcuts\n3=Files|/documents/Manager.tns\nT=4";
    write_text(cfg,original);
    EhRegistry r; eh7_registry_init(&r);
    assert(eh7_registry_load(&r,cfg)==1 && r.appearance.theme==4);
    assert(!eh_prefs_load(cfg,&r.appearance) && r.appearance.theme==4);
    EhAppearance new={1,2,3},out={0,0,0};
    assert(eh_prefs_save(cfg,new)); assert(eh_prefs_load(cfg,&out));
    assert(out.theme==1 && out.folder==2 && out.font==3);
    assert(!eh_prefs_save(cfg,(EhAppearance){0,0,EH_COLOR_COUNT}));
    assert(eh_prefs_save(cfg,(EhAppearance){19,18,17}));
    assert(eh_prefs_load(cfg,&out) && out.theme==19 && out.folder==18 && out.font==17);
    write_text(stage,"incomplete"); assert(rename(live,bak)==0);
    assert(eh_prefs_load(cfg,&out) && out.font==3);
    new.font=5; assert(eh_prefs_save(cfg,new));
    assert(eh_prefs_load(cfg,&out) && out.font==5);
    assert(rename(live,bak)==0); write_text(live,"invalid");
    assert(eh_prefs_load(cfg,&out) && out.font==5);
    assert(eh_prefs_save(cfg,(EhAppearance){0,0,0}));
    assert(eh_prefs_load(cfg,&out) && !out.theme && !out.folder && !out.font);
    char buffer[128]={0}; FILE *f=fopen(cfg,"rb"); assert(f);
    assert(fread(buffer,1,sizeof(buffer)-1,f)==strlen(original)); assert(!fclose(f));
    assert(!strcmp(original,buffer)); assert(!eh_prefs_save("/missing",(EhAppearance){0,0,0}));
    unlink(live); unlink(cfg); assert(rmdir(dir)==0);
    puts("PASS appearance persistence, legacy config, interrupted saves, defaults");
    return 0;
}
