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

#define ROWS 4u
#define EXPORT_PATH "/documents/EndraTask_Run002_Report.txt.tns"

static UiCanvas canvas;
static const UiTheme *const theme = &ui_themes[0];

typedef struct {
    Object *tasks;
    unsigned count;
    const char *task_error;
    char documents[56];
    char usb[56];
    int ndless_rev;
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

static bool export_report(const Snapshot *snapshot) {
    FILE *file = fopen(EXPORT_PATH, "wb");
    if (!file) return false;

    bool ok = fprintf(file,
        "EndraTask Run002\nndless_rev=%d\nlocal_documents=%s\n"
        "phone_usb_import=%s\ntask_count=%u\ntask_result=%s\n"
        "cpu_usage=unavailable\ntotal_task_memory=unavailable\n"
        "open_handles=unavailable\nfile_lock_owner=unavailable\n"
        "snapshot_id,name,state,priority,stack_bytes\n",
        snapshot->ndless_rev, snapshot->documents, snapshot->usb, snapshot->count,
        snapshot->task_error ? snapshot->task_error : "ok") >= 0;

    for (unsigned i = 0; ok && i < snapshot->count; ++i) {
        const Object *task = &snapshot->tasks[i];
        char name[TASK_NAME_SIZE + 1] = {0};
        for (unsigned j = 0; j < TASK_NAME_SIZE && task->name[j]; ++j) {
            unsigned char ch = (unsigned char)task->name[j];
            name[j] = ch >= 32 && ch <= 126 && ch != ',' && ch != '"' ? (char)ch : '_';
        }
        ok = fprintf(file, "%08lX,%.8s,%s,%u,%u\n",
            (unsigned long)task->address, name, task_status(task->data.task.status),
            (unsigned)task->data.task.priority, task->data.task.stack_size) >= 0;
    }
    int closed = fclose(file);
    return ok && closed == 0;
}

static void draw(const Snapshot *snapshot, unsigned first, const char *notice, bool confirm_export) {
    char line[80];
    ui_canvas_begin(&canvas);
    ui_fill(canvas.gc, 0, 0, UI_WIDTH, UI_HEIGHT, theme->bg);
    ui_draw_header(canvas.gc, theme, "EndraTask | Run002");
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
        int y = 108 + (int)i * 23;
        snprintf(line, sizeof(line), "%.8s  ID %08lX  P%u",
                 task->name[0] ? task->name : "(unnamed)", (unsigned long)task->address,
                 (unsigned)task->data.task.priority);
        ui_draw_text(canvas.gc, line, 9, y, Regular9, theme->fg);
        snprintf(line, sizeof(line), "%s | stack %u B",
                 task_status(task->data.task.status), task->data.task.stack_size);
        ui_draw_text(canvas.gc, line, 17, y + 10, Regular9, theme->muted);
    }
    if (confirm_export) {
        ui_fill(canvas.gc, 4, 95, 312, 120, theme->bg);
        ui_draw_text(canvas.gc, "Save / replace the EndraTask report?", 9, 120, Regular10, theme->fg);
        ui_draw_text(canvas.gc, "EndraTask_Run002_Report.txt.tns", 9, 142, Regular9, theme->muted);
        ui_draw_footer(canvas.gc, theme, "Enter: save   Esc: cancel");
    } else {
        ui_canvas_text(&canvas, 9, 207, 302, notice ? notice : "CPU / task memory / lock owner: unavailable", theme->muted);
        ui_draw_footer(canvas.gc, theme, "Up/Down: scroll  R: refresh  E: export  Esc: exit");
    }
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

    snapshot.ndless_rev = nl_ndless_rev();
    bool supported = snapshot.ndless_rev >= 2023;
    if (supported) supported = native_open(&snapshot.task_error);
    else snapshot.task_error = "Ndless r2023 or newer required";
    unsigned first = 0;
    refresh(&snapshot, supported);
    const char *notice = NULL;
    bool confirm_export = false;
    draw(&snapshot, first, notice, confirm_export);
    for (;;) {
        if (confirm_export) {
            if (isKeyPressed(KEY_NSPIRE_ESC)) {
                wait_no_key_pressed();
                confirm_export = false;
            } else if (isKeyPressed(KEY_NSPIRE_ENTER) || isKeyPressed(KEY_NSPIRE_RET)) {
                wait_no_key_pressed();
                notice = export_report(&snapshot) ? "Report saved in Documents" : "Report write failed";
                confirm_export = false;
            } else {
                msleep(40);
                continue;
            }
            draw(&snapshot, first, notice, confirm_export);
            continue;
        }
        if (isKeyPressed(KEY_NSPIRE_ESC)) break;
        if (isKeyPressed(KEY_NSPIRE_R)) {
            wait_no_key_pressed();
            refresh(&snapshot, supported);
            first = 0;
            notice = NULL;
        } else if (isKeyPressed(KEY_NSPIRE_E)) {
            wait_no_key_pressed();
            confirm_export = true;
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
        draw(&snapshot, first, notice, confirm_export);
    }
    wait_no_key_pressed();
    free(snapshot.tasks);
    ui_canvas_close(&canvas);
    refresh_osscr();
    return 0;
}
