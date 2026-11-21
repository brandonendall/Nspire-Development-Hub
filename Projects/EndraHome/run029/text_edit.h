#ifndef ENDRAHOME_TEXT_EDIT_H
#define ENDRAHOME_TEXT_EDIT_H
#include "picker.h"

#define EH_TEXT_CAP 4096
#define EH_TEXT_COLUMNS 36
#define EH_TEXT_ROWS 12

typedef struct {
    char original[EH_TEXT_CAP + 1];
    unsigned original_len, cursor, top;
    int dirty, confirm_discard, crlf;
} EhTextEdit;

typedef struct {
    unsigned start, end, next;
    int more;
} EhTextRow;

int eh_text_editable(const char *path,const char *data,unsigned length);
void eh_text_begin(EhTextEdit *edit,const char *data,unsigned length);
int eh_text_type(EhTextEdit *edit,char *data,unsigned *length,int ch);
void eh_text_move(EhTextEdit *edit,const char *data,unsigned length,EhKey key);
EhTextRow eh_text_row(const char *data,unsigned length,unsigned start);
unsigned eh_text_cursor_row(const char *data,unsigned length,unsigned cursor);
const char *eh_text_save(EhTextEdit *edit,const char *path,const char *data,unsigned length);
#endif
