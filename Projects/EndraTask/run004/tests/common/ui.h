#ifndef TEST_UI_H
#define TEST_UI_H
#include <stdbool.h>

typedef struct { unsigned marker; } UiCanvas;
typedef enum { UI_ENGLISH } UiLanguage;

UiLanguage ui_language_init(void);
bool ui_canvas_open(UiCanvas *canvas);
int ui_menu(UiCanvas *canvas, const char *heading, const char *const *items,
            unsigned count, unsigned selected);
void ui_canvas_close(UiCanvas *canvas);
#endif
