#ifndef ENDRAHOME_FILE_ICONS_H
#define ENDRAHOME_FILE_ICONS_H
#include "display.h"

typedef enum {
    EH_FILE_FOLDER, EH_FILE_TEXT, EH_FILE_ARCHIVE, EH_FILE_IMAGE, EH_FILE_PDF,
    EH_FILE_CONFIG, EH_FILE_CSV, EH_FILE_ROM, EH_FILE_PROGRAM, EH_FILE_TI,
    EH_FILE_UNKNOWN, EH_FILE_KIND_COUNT
} EhFileKind;

/* Display classification only. A suffix is never evidence that a file can run. */
EhFileKind eh_file_kind(const char *name, int directory);
void eh_file_kind_icon(uint16_t *frame,int x,int y,EhFileKind kind,const EhPalette *palette);
#endif
