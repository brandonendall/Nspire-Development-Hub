#ifndef EH007_REGISTRY_H
#define EH007_REGISTRY_H
#include <stddef.h>
#include "display.h"
#define EH7_SLOT_COUNT 7
#define EH_FILES_SLOT 3
#define EH7_LABEL_CAP 16
#define EH7_PATH_CAP 256
#define EH7_CONFIG_FILE "EndraHome_Apps.cfg.tns"
typedef struct {
 char label[EH7_LABEL_CAP];
 char path[EH7_PATH_CAP];
} EhAppSlot;
typedef struct {
 EhAppSlot slots[EH7_SLOT_COUNT];
 int file_loaded;
 int valid_lines;
 int errors;
 int enabled;
 int theme; /* Run028 shortcut file theme, used until separate preferences exist. */
 EhAppearance appearance;
} EhRegistry;
unsigned eh_legacy_theme(unsigned old);
void eh7_registry_init(EhRegistry *apps);
int eh7_valid_path(const char *path);
int eh7_config_path(char *dst,size_t cap,const char *app_executable);
int eh7_registry_parse(EhRegistry *apps,const char *line);
int eh7_registry_load(EhRegistry *apps,const char *path);
#endif
