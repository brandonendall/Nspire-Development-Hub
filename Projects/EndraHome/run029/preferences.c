#include "preferences.h"
#include "registry.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

typedef struct { char live[512], stage[512], backup[512]; } PrefPaths;
static int paths(const char *cfg, PrefPaths *p) {
    if (!cfg) return 0;
    const char *base = strrchr(cfg, '/');
    if (!base || strcmp(base + 1, EH7_CONFIG_FILE)) return 0;
    int prefix = (int)(base - cfg + 1);
    if ((size_t)prefix + sizeof(EH_PREFS_FILE) > sizeof(p->live)) return 0;
    snprintf(p->live, sizeof(p->live), "%.*s%s", prefix, cfg, EH_PREFS_FILE);
    snprintf(p->stage, sizeof(p->stage), "%.*sEHLookNew.cfg.tns", prefix, cfg);
    snprintf(p->backup, sizeof(p->backup), "%.*sEHLookOld.cfg.tns", prefix, cfg);
    return 1;
}
static int valid(EhAppearance a) {
    return a.theme < EH_COLOR_COUNT && a.folder < EH_COLOR_COUNT && a.font < EH_COLOR_COUNT;
}
static int read_prefs(const char *path, EhAppearance *out) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    char data[64]={0}, extra=0;
    int theme=-1,folder=-1,font=-1;
    int ok=fgets(data,sizeof(data),f)!=NULL &&
           fscanf(f,"%c",&extra)!=1 && !ferror(f);
    if(fclose(f)!=0 || !ok)return 0;
    /* Legacy v1 single-digit settings and v2 multi-digit settings. */
    if(sscanf(data,"v2 %d %d %d",&theme,&folder,&font)!=3 &&
       sscanf(data,"v1 %d %d %d",&theme,&folder,&font)!=3)return 0;
    char canonical[64];
    snprintf(canonical,sizeof(canonical),"%s %d %d %d\\n",data[1]=='2'?"v2":"v1",theme,folder,font);
    if(strcmp(data,canonical)!=0 || theme<0 || folder<0 || font<0)return 0;
    EhAppearance a={(unsigned)theme,(unsigned)folder,(unsigned)font};
    if(!valid(a))return 0;
    *out=a;
    return 1;
}
int eh_prefs_load(const char *cfg, EhAppearance *out) {
    PrefPaths p;
    return out && paths(cfg, &p) && (read_prefs(p.live, out) || read_prefs(p.backup, out));
}
static int present(const char *name) {
    struct stat st;
    if (stat(name, &st) == 0) return S_ISREG(st.st_mode) ? 1 : -1;
    return errno == ENOENT ? 0 : -1;
}
int eh_prefs_save(const char *cfg, EhAppearance a) {
    PrefPaths p;
    EhAppearance previous, verified;
    if (!valid(a) || !paths(cfg, &p)) return 0;
    int live = present(p.live), backup = present(p.backup);
    if (live < 0 || backup < 0 || present(p.stage) < 0) return 0;
    /* Keep the last readable copy through an interrupted two-rename install. */
    if (backup && (!live || !read_prefs(p.live, &previous))) {
        if (!read_prefs(p.backup, &previous)) return 0;
        if (live && remove(p.live) != 0) return 0;
        if (rename(p.backup, p.live) != 0) return 0;
        live = 1;
    } else if (backup && remove(p.backup) != 0) return 0;
    FILE *f = fopen(p.stage, "wb");
    if (!f) return 0;
    int ok = fprintf(f, "v1 %u %u %u\n", a.theme, a.folder, a.font) == 9;
    if (fclose(f) || !ok || !read_prefs(p.stage, &verified) ||
        verified.theme != a.theme || verified.folder != a.folder || verified.font != a.font) {
        remove(p.stage);
        return 0;
    }
    if (live && rename(p.live, p.backup) != 0) { remove(p.stage); return 0; }
    if (rename(p.stage, p.live) != 0) {
        if (live) rename(p.backup, p.live);
        return 0;
    }
    if (live) remove(p.backup);
    return 1;
}
