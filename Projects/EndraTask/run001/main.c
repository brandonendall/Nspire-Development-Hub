// SPDX-License-Identifier: GPL-3.0-only
#include "probe.h"
#include "tasks.h"
#include "common/ui.h"

#include <dirent.h>
#include <errno.h>
#include <libndls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 5u

static UiCanvas canvas;
static const UiTheme *const theme = &ui_themes[0];

typedef struct {
    Object *tasks;
    unsigned count;
    const char *task_error;
    char documents[56];
    char usb[56];
} Snapshot;

static void refresh(Snapshot *snapshot, bool task_supported) {
    free(snapshot->tasks);
    snapshot->tasks = NULL;
    snapshot->count = 0;
    if (task_supported) {
        int result = native_list(OBJECT_TASK, &snapshot->tasks, &snapshot->count);
        snapshot->task_error = result ? native_error(result) : NULL;
    }

    DIR *dir = opendir("/documents");
    if (dir) {
        closedir(dir);
        snprintf(snapshot->documents, sizeof(snapshot->documents), "Opened /documents");
    } else {
        snprintf(snapshot->documents, sizeof(snapshot->documents), "Open failed (errno %d)", errno);
    }

    FILE *report = fopen(USB_REPORT, "rb");
    if (!report) {
        if (errno == ENOENT) {
            snprintf(snapshot->usb, sizeof(snapshot->usb), "No imported report");
        } else {
            snprintf(snapshot->usb, sizeof(snapshot->usb), "Report open failed (errno %d)", errno);
        }
        return;
    }
    char stage[USB_STAGE_SIZE];
    enum UsbReport status = read_usb_stage(report, stage);
    fclose(report);
    switch (status) {
        case USB_STAGE_FOUND: snprintf(snapshot->usb, sizeof(snapshot->usb), "%s", stage); break;
        case USB_REPORT_LIMIT: snprintf(snapshot->usb, sizeof(snapshot->usb), "Report exceeds read limit"); break;
        case USB_REPORT_ERROR: snprintf(snapshot->usb, sizeof(snapshot->usb), "Report read failed"); break;
        case USB_NO_STAGE: snprintf(snapshot->usb, sizeof(snapshot->usb), "No Build 010 probe stage found"); break;
    }
}

static void draw(const Snapshot *snapshot, unsigned first) {
    char line[80];
    ui_canvas_begin(&canvas);
    ui_fill(canvas.gc, 0, 0, UI_WIDTH, UI_HEIGHT, theme->bg);
    ui_draw_header(canvas.gc, theme, "EndraTask | Run001");
    ui_draw_text(canvas.gc, "LOCAL DOCUMENTS", 9, 28, Regular9, theme->accent);
    ui_canvas_text(&canvas, 9, 48, 302, snapshot->documents, theme->fg);
    ui_draw_text(canvas.gc, "PHONE USB | IMPORTED BUILD 010 REPORT", 9, 61, Regular9, theme->accent);
    ui_canvas_text(&canvas, 9, 81, 302, snapshot->usb, theme->fg);
    if (snapshot->count) {
        unsigned last = first + ROWS < snapshot->count ? first + ROWS : snapshot->count;
        snprintf(line, sizeof(line), "TASK SNAPSHOT | %u-%u of %u", first + 1, last, snapshot->count);
    } else {
        snprintf(line, sizeof(line), "TASK SNAPSHOT | %s", snapshot->task_error ? "Unavailable" : "Empty");
    }
    ui_draw_text(canvas.gc, line, 9, 95, Regular9, theme->accent);
    if (snapshot->task_error) {
        ui_canvas_text(&canvas, 9, 122, 302, snapshot->task_error, theme->fg);
    }
    for (unsigned i = 0; i < ROWS && first + i < snapshot->count; ++i) {
        const Object *task = &snapshot->tasks[first + i];
        snprintf(line, sizeof(line), "%.8s  %s  P%u", task->name[0] ? task->name : "(unnamed)",
                 task_status(task->data.task.status), (unsigned)task->data.task.priority);
        ui_draw_row(canvas.gc, theme, 109 + (int)i * 20, line, (first + i) % 2 == 0);
    }
    ui_draw_footer(canvas.gc, theme, "Up/Down: scroll   R: refresh   Esc: exit");
    ui_canvas_end(&canvas);
    lcd_blit(ui_canvas_pixels(&canvas), SCR_320x240_565);
}

int main(void) {
    Snapshot snapshot = {0};
    if (!ui_canvas_open(&canvas)) {
        show_msgbox("EndraTask", "Could not open display");
        refresh_osscr();
        return 1;
    }

    bool supported = nl_ndless_rev() >= 2023;
    if (supported) supported = native_open(&snapshot.task_error);
    else snapshot.task_error = "Ndless r2023 or newer required";
    unsigned first = 0;
    refresh(&snapshot, supported);
    draw(&snapshot, first);
    for (;;) {
        if (isKeyPressed(KEY_NSPIRE_ESC)) break;
        if (isKeyPressed(KEY_NSPIRE_R)) {
            wait_no_key_pressed();
            refresh(&snapshot, supported);
            first = 0;
        } else if (first && isKeyPressed(KEY_NSPIRE_UP)) {
            wait_no_key_pressed();
            --first;
        } else if (first + ROWS < snapshot.count && isKeyPressed(KEY_NSPIRE_DOWN)) {
            wait_no_key_pressed();
            ++first;
        } else {
            msleep(40);
            continue;
        }
        draw(&snapshot, first);
    }
    wait_no_key_pressed();
    free(snapshot.tasks);
    ui_canvas_close(&canvas);
    refresh_osscr();
    return 0;
}
