#ifndef ENDRAHOME_SHORTCUTS_H
#define ENDRAHOME_SHORTCUTS_H
#define EH_SLOTS 4
#define EH_NAME_CAP 16
#define EH_PATH_CAP 256
typedef struct { char name[EH_NAME_CAP]; char path[EH_PATH_CAP]; } EhShortcut;
void eh_defaults(EhShortcut slots[EH_SLOTS]);
int eh_safe_path(const char *path);
int eh_parse_line(EhShortcut slots[EH_SLOTS], const char *line);
int eh_load(EhShortcut slots[EH_SLOTS], const char *filename);
#endif
