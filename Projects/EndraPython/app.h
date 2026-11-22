#pragma once
#include <stddef.h>
#include <stdint.h>
#define SOURCE_CAP 8192
#define NAME_CAP 128
#define PATH_CAP 384
#define FILE_CAP 96
#define EDIT_ROWS 12
#define EDIT_COLS 40
#define OUT_WIDTH 40
#define OUT_LINES 96
typedef enum { FILES, EDITOR, OUTPUT, SHELL, NAME, ACTIONS, DISCARD, FIND } Page;
typedef enum { UP=256, DOWN, LEFT, RIGHT, ENTER, ESC, MENU, SAVE, RUN, INDENT, UNDO, REDO, COPY, CUT, PASTE, SELECT_ALL, SEARCH, EXT_UP, EXT_DOWN, EXT_LEFT, EXT_RIGHT, TI_INPUT, MARKS } Key;
typedef struct {
    char name[NAME_CAP], text[SOURCE_CAP+1], original[SOURCE_CAP+1];
    int cursor, top, left, dirty, existing, crlf, anchor, selecting;
} Editor;
typedef struct {
    Page page, origin;
    Editor doc;
    char query[64];
    struct FileEntry { char name[NAME_CAP]; int folder; } entries[FILE_CAP];
    char name[NAME_CAP], status[80], folder[PATH_CAP], documents[PATH_CAP];
    int browsing, request_input, file_page, more_files;
    int count, selected, action, quit, error, error_line, symbols;
    char output[OUT_LINES][4*OUT_WIDTH+1], command[128], history[128];
    int first, rows, col, offset, output_bytes;
    const char *home, *root;
    void *stack;
} App;
extern App app;
void app_init(const char *root, void *stack);
void app_key(int key);
void render(uint16_t *pixels);
void edit_key(Editor *d,int k);
int line_start(const char *s,int pos);
int line_number(const char *s,int pos);
int valid_script(const char *s);
void files_refresh(void);
int load_script(const char *name);
int save_script(void);

void edit_reset_history(void);
void edit_find(Editor *d,const char *query);
extern const char *const ep_symbols[];
extern const int ep_symbol_count;
extern const char *const ep_actions[];
void edit_goto_line(Editor *d,int line);

int browse_folder(const char *path);
void browse_parent(void);
void app_insert_text(const char *value);
void edit_insert(Editor *doc,const char *value);
int utf_decode(const char *s,int *bytes);
int utf_prev(const char *s,int pos);
int utf_columns(const char *s,int bytes);
int utf_advance(const char *s,int pos,int end,int cols);
int utf16_input(const uint16_t *s,size_t units,char *out,size_t cap);
