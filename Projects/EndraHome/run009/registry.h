#ifndef EH007_REGISTRY_H
#define EH007_REGISTRY_H
#include <stddef.h>
#define EH7_SLOT_COUNT 7
#define EH7_LABEL_CAP 16
#define EH7_PATH_CAP 256
#define EH7_CONFIG_FILE "EndraHome_Apps009.cfg.tns"
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
} EhRegistry;
void eh7_registry_init(EhRegistry *apps);
int eh7_valid_path(const char *path);
int eh7_config_path(char *dst,size_t cap,const char *app_executable);
int eh7_registry_parse(EhRegistry *apps,const char *line);
int eh7_registry_load(EhRegistry *apps,const char *path);
#endif
