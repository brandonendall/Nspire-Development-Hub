#ifndef EH017_FILES_H
#define EH017_FILES_H
#include "picker.h"
#include "registry.h"
#include "image_viewer.h"
#include <stdint.h>
#define EH17_TEXT_CAP 4096
typedef struct {
 Eh13Picker nav;
 int active,screen,preview_ok,preview_truncated,line_count,scroll;
 unsigned long file_size;
 Eh18Image image;
 char path[EH7_PATH_CAP];
 char content[EH17_TEXT_CAP+1];
 unsigned text_len;
 int edit_folder,edit_error;
 char edit_name[EH13_NAME_MAX];
} Eh17Files;
int eh17_files_open(Eh17Files *v,const char *root);
void eh17_files_close(Eh17Files *v);
int eh17_files_handle(Eh17Files *v,EhKey key);
void eh17_files_render(uint16_t *frame,const Eh17Files *v,const EhRegistry *apps);
int eh21_files_text(Eh17Files *v,int ch);
int eh17_is_text_name(const char *name);
#endif
