// SPDX-License-Identifier: GPL-3.0-only
#include <libndls.h>

#include "common/ui.h"

int main(void) {
    if (nl_ndless_rev() < 2023) {
        show_msgbox("EndraTask Run004", "Ndless r2023 or later is required.");
        return 1;
    }

    ui_language_init();
    UiCanvas canvas;
    if (!ui_canvas_open(&canvas)) {
        show_msgbox("EndraTask Run004", "Could not open the display.");
        refresh_osscr();
        return 1;
    }

    const char *const items[] = {"Return to TI Documents"};
    ui_menu(&canvas, "EndraTask Run004 | Exit check", items, 1, 0);
    ui_canvas_close(&canvas);
    refresh_osscr();
    return 0;
}
