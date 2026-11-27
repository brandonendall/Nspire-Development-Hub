#ifndef EH017_FILES_H
#define EH017_FILES_H
#include "picker.h"
#include "toast.h"
#include "registry.h"
#include "image_viewer.h"
#include "text_edit.h"
#include <stdint.h>
#define EH17_TEXT_CAP EH_TEXT_CAP
#define EH_FILES_FIND 7
#define EH_FILES_ARCHIVE 6
enum { EH_NEW_FILE=8, EH_CREATE_ZIP, EH_EXTRACT_ZIP, EH_FIND_NAME, EH_ACTION_COUNT };
#define EH_FILES_TEXT_EDIT 5
typedef struct {
 Eh13Picker nav,archive;
 char query[44],json_query[44];
 unsigned long match;
 int matched;
#define EH_FILES_FIND_JSON 8
 int pdf_requested;
 int active,screen,preview_ok,preview_truncated,line_count,scroll;
 unsigned long file_size,preview_offset;
 int json_view;
 Eh18Image image;
 EhTextEdit text_edit;
 char path[EH7_PATH_CAP];
 char content[EH17_TEXT_CAP+1];
 unsigned text_len;
 int edit_folder,edit_error;
 int menu_open,menu_item,confirm_delete,clip_mode,clip_is_dir;
 char clip_path[EH7_PATH_CAP];
 EhToast status;
 char edit_name[EH13_NAME_MAX];
} Eh17Files;
int eh17_files_open(Eh17Files *v,const char *root);
void eh17_files_close(Eh17Files *v);
int eh17_files_handle(Eh17Files *v,EhKey key);
void eh17_files_render(uint16_t *frame,const Eh17Files *v,const EhRegistry *apps);
int eh21_files_text(Eh17Files *v,int ch);
int eh17_is_text_name(const char *name);
#endif
