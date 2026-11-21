#include "preferences.h"
#include "registry.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct { char live[512], stage[512], backup[512]; } PrefPaths;
static int paths(const char *cfg, PrefPaths *p, unsigned version) {
    if (!cfg) return 0;
    const char *base = strrchr(cfg, '/');
    if (!base || strcmp(base + 1, EH7_CONFIG_FILE)) return 0;
    int prefix = (int)(base - cfg + 1);
    if ((size_t)prefix + sizeof(EH_PREFS_FILE) > sizeof(p->live)) return 0;
    snprintf(p->live, sizeof(p->live), "%.*s%s", prefix, cfg,
             version==4 ? EH_PREFS_FILE : version==3 ? EH_RUN038_PREFS_FILE : EH_LEGACY_PREFS_FILE);
    snprintf(p->stage, sizeof(p->stage), "%.*sEHLook4New.cfg.tns", prefix, cfg);
    snprintf(p->backup, sizeof(p->backup), "%.*s%s", prefix, cfg,
             version==4 ? "EHLook4Old.cfg.tns" : version==3 ? "EHLook3Old.cfg.tns" : "EHLookOld.cfg.tns");
    return 1;
}
static int valid(EhAppearance a) {
    return a.theme < EH_THEME_COUNT && a.folder < EH_COLOR_COUNT && a.font < EH_COLOR_COUNT &&
           a.wallpaper < EH_WALLPAPER_COUNT && a.icon < EH_ICON_COUNT;
}

static int read_prefs(const char *path, EhAppearance *out, unsigned version) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    char data[64] = {0};
    int ok = fgets(data, sizeof(data), f) && fgetc(f) == EOF && !ferror(f);
    if (fclose(f) != 0 || !ok) return 0;

    if (data[0] != 'v' ||
        (version==2 ? data[1] != '1' && data[1] != '2' : data[1] != (int)('0'+version))) return 0;

    const unsigned limits[] = {
        version==4 ? EH_THEME_COUNT : EH_LEGACY_THEME_COUNT,
        EH_COLOR_COUNT, EH_COLOR_COUNT, EH_WALLPAPER_COUNT, EH_ICON_COUNT
    };
    unsigned values[5] = {0};
    char *cursor = data + 2;
    unsigned count = version==2 ? 3 : 5;
    for (unsigned i = 0; i < count; ++i) {
        if (*cursor == '\n' && i >= 3) break;
        if (*cursor++ != ' ') return 0;
        char *end, *token_end = cursor + strcspn(cursor, " \n");
        errno = 0;
        long value = strtol(cursor, &end, 10);
        int valid = end != cursor && end == token_end && errno == 0 && value >= 0 && value < (long)limits[i];
        if (!valid && i < 3) return 0;
        if (valid) values[i] = (unsigned)value;
        cursor = token_end;
    }
    if (strcmp(cursor, "\n") != 0) return 0;
    if(version<4)values[0]=eh_legacy_theme(values[0]);
    *out = (EhAppearance){values[0], values[1], values[2], values[3], values[4]};
    return 1;
}

int eh_prefs_load(const char *cfg, EhAppearance *out) {
    if (!out) return 0;
    PrefPaths p;
    for(unsigned version=4;version>=2;--version)
        if(paths(cfg,&p,version) &&
           (read_prefs(p.live,out,version) || read_prefs(p.backup,out,version))) return 1;
    return 0;
}
static int present(const char *name) {
    struct stat st;
    if (stat(name, &st) == 0) return S_ISREG(st.st_mode) ? 1 : -1;
    return errno == ENOENT ? 0 : -1;
}
int eh_prefs_save(const char *cfg, EhAppearance a) {
    PrefPaths p;
    EhAppearance previous, verified;
    if (!valid(a) || !paths(cfg, &p, 4)) return 0;
    int live = present(p.live), backup = present(p.backup);
    if (live < 0 || backup < 0 || present(p.stage) < 0) return 0;
    /* Keep the last readable copy through an interrupted two-rename install. */
    if (backup && (!live || !read_prefs(p.live, &previous, 4))) {
        if (!read_prefs(p.backup, &previous, 4)) return 0;
        if (live && remove(p.live) != 0) return 0;
        if (rename(p.backup, p.live) != 0) return 0;
        live = 1;
    } else if (backup && remove(p.backup) != 0) return 0;
    FILE *f = fopen(p.stage, "wb");
    if (!f) return 0;
    int ok = fprintf(f, "v4 %u %u %u %u %u\n",
                     a.theme, a.folder, a.font, a.wallpaper, a.icon) > 0;
    if (fclose(f) || !ok || !read_prefs(p.stage, &verified, 4) ||
        verified.theme != a.theme || verified.folder != a.folder || verified.font != a.font ||
        verified.wallpaper != a.wallpaper || verified.icon != a.icon) {
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
