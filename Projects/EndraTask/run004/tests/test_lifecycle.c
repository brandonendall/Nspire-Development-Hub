// SPDX-License-Identifier: GPL-3.0-only
#include <assert.h>
#include <string.h>

#include "common/ui.h"
#include "libndls.h"

int endratask_main(void);

static char calls[8];
static unsigned length;
static unsigned revision;
static bool can_open;
static int menu_result;

static void mark(char event) {
    assert(length + 1 < sizeof(calls));
    calls[length++] = event;
    calls[length] = '\0';
}

unsigned int nl_ndless_rev(void) {
    mark('V');
    return revision;
}

void show_msgbox(const char *title, const char *message) {
    assert(strstr(title, "Run004") && *message);
    mark('M');
}

UiLanguage ui_language_init(void) {
    mark('L');
    return UI_ENGLISH;
}

bool ui_canvas_open(UiCanvas *canvas) {
    canvas->marker = 42;
    mark('O');
    return can_open;
}

int ui_menu(UiCanvas *canvas, const char *heading, const char *const *items,
            unsigned count, unsigned selected) {
    assert(canvas->marker == 42 && strstr(heading, "Exit check"));
    assert(count == 1 && selected == 0 && strstr(items[0], "Return"));
    mark('U');
    return menu_result;
}

void ui_canvas_close(UiCanvas *canvas) {
    assert(canvas->marker == 42);
    mark('C');
}

void refresh_osscr(void) {
    mark('R');
}

static void check(unsigned new_revision, bool open, int choice, int result, const char *expected) {
    length = 0;
    revision = new_revision;
    can_open = open;
    menu_result = choice;
    assert(endratask_main() == result);
    assert(strcmp(calls, expected) == 0);
}

int main(void) {
    check(2022, true, 0, 1, "VM");
    check(2023, false, 0, 1, "VLOMR");
    check(2023, true, -1, 0, "VLOUCR");
    check(2023, true, 0, 0, "VLOUCR");
    return 0;
}
