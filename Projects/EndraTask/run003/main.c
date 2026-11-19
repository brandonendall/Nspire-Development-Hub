// SPDX-License-Identifier: GPL-3.0-only
#include "diagnostic.h"
#include "common/ui.h"

#include <errno.h>
#include <libndls.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>

typedef struct {
    const char *label;
    const char *path;
    ProbeMode access;
} Target;

static const Target targets[] = {
    {"Documents", "/documents", PROBE_DIR},
    {"Upload (Documents)", "/documents/upload", PROBE_DIR},
    {"Upload (root)", "/upload", PROBE_DIR},
    {"USB folder", "/documents/USB", PROBE_DIR},
    {"Run002 (Documents)", "/documents/upload/EndraTask_Run002.tns", PROBE_FILE},
    {"Run002 (root)", "/upload/EndraTask_Run002.tns", PROBE_FILE},
    {"Run002 export", "/documents/EndraTask_Run002_Report.txt.tns", PROBE_FILE},
};

#define TARGETS (sizeof(targets) / sizeof(targets[0]))

typedef struct {
    ProbeResult result;
    long elapsed;
    bool checked;
} Check;

typedef struct {
    unsigned selected;
    unsigned column;
    Check checks[TARGETS][2];
} Screen;

static UiCanvas canvas;
static const UiTheme *const theme = &ui_themes[0];

static void show(Screen *screen, const char *running) {
    char line[90];
    ui_canvas_begin(&canvas);
    ui_draw_header(canvas.gc, theme, "EndraTask Run003 | Local path checks");
    ui_canvas_text(&canvas, 173, 33, 65, "META", screen->column == 0 ? theme->accent : theme->muted);
    ui_canvas_text(&canvas, 244, 33, 70, "ACCESS", screen->column == 1 ? theme->accent : theme->muted);
    for (unsigned i = 0; i < TARGETS; ++i) {
        int y = 46 + (int)i * 16;
        bool selected = screen->selected == i;
        if (selected) ui_fill(canvas.gc, 5, y - 5, 310, 16, theme->highlight_bg);
        int color = selected ? theme->highlight_fg : theme->fg;
        ui_canvas_text(&canvas, 9, y, 158, targets[i].label, color);
        for (unsigned col = 0; col < 2; ++col) {
            const Check *check = &screen->checks[i][col];
            ui_canvas_text(&canvas, col == 0 ? 173 : 244, y, 70,
                check->checked ? probe_status(check->result.call) : "-",
                color);
        }
    }

    const Target *target = &targets[screen->selected];
    const Check *check = &screen->checks[screen->selected][screen->column];
    ui_canvas_text(&canvas, 9, 166, 302, target->path, theme->fg);
    if (running) {
        snprintf(line, sizeof(line), "Running: %s", running);
        ui_canvas_text(&canvas, 9, 184, 302, line, theme->accent);
        ui_canvas_text(&canvas, 9, 201, 302, "If stalled, note the stage shown.", theme->muted);
    } else if (check->checked) {
        const ProbeResult *result = &check->result;
        snprintf(line, sizeof(line), "%s | %s | rc %d, errno %d",
            probe_stage(result->stage), probe_status(result->call), result->call.code, result->call.error);
        ui_canvas_text(&canvas, 9, 184, 302, line, theme->fg);
        char elapsed[24] = "Time unavailable";
        if (check->elapsed >= 0) snprintf(elapsed, sizeof(elapsed), "%ld s (RTC)", check->elapsed);
        if (result->call.code == 0 && screen->column == 0) {
            snprintf(line, sizeof(line), "%s | mode 0x%lX | size %lu B",
                elapsed, result->mode, result->size);
        } else {
            snprintf(line, sizeof(line), "%s%s", elapsed,
                result->call.code == 0 && target->access == PROBE_FILE ?
                    (result->eof ? " | end of file" : " | one byte read") : "");
        }
        ui_canvas_text(&canvas, 9, 201, 302, line, theme->muted);
    } else {
        ui_canvas_text(&canvas, 9, 184, 302, "No check run for this target and operation.", theme->muted);
        ui_canvas_text(&canvas, 9, 201, 302, "Local access only. Lock owner unavailable.", theme->muted);
    }
    ui_draw_footer(canvas.gc, theme, "Arrows: select    Enter: check    Esc: exit");
    ui_canvas_end(&canvas);
    lcd_blit(ui_canvas_pixels(&canvas), SCR_320x240_565);
}

static void show_stage(void *context, ProbeStage stage) {
    show(context, probe_stage(stage));
}

static ProbeCall metadata(const char *path, unsigned long *size, unsigned long *mode) {
    struct stat info;
    errno = 0;
    int rc = stat(path, &info);
    int error = errno;
    if (rc == 0) {
        *size = (unsigned long)info.st_size;
        *mode = (unsigned long)info.st_mode;
    }
    return (ProbeCall){rc, rc == 0 ? 0 : error};
}

static ProbeCall open_path(const char *path, ProbeMode mode, void **handle) {
    errno = 0;
    *handle = mode == PROBE_DIR ? (void *)nuc_opendir(path) : (void *)fopen(path, "rb");
    int error = errno;
    return (ProbeCall){*handle ? 0 : -1, *handle ? 0 : error};
}

static ProbeCall read_byte(void *handle, bool *eof) {
    FILE *file = handle;
    errno = 0;
    if (setvbuf(file, NULL, _IONBF, 0) != 0) return (ProbeCall){-1, errno};
    int ch = fgetc(file);
    int error = errno;
    if (ch != EOF) return (ProbeCall){0, 0};
    if (ferror(file)) return (ProbeCall){-1, error};
    *eof = true;
    return (ProbeCall){0, 0};
}

static ProbeCall close_path(void *handle, ProbeMode mode) {
    errno = 0;
    int rc = mode == PROBE_DIR ? nuc_closedir(handle) : fclose(handle);
    int error = errno;
    return (ProbeCall){rc, rc == 0 ? 0 : error};
}

static int64_t now_seconds(void) {
    struct timeval time;
    if (gettimeofday(&time, NULL) != 0) return -1;
    return time.tv_sec;
}

static void check_selected(Screen *screen) {
    const Target *target = &targets[screen->selected];
    Check *check = &screen->checks[screen->selected][screen->column];
    ProbeOps ops = {screen, show_stage, metadata, open_path, read_byte, close_path};
    int64_t start = now_seconds();
    check->result = probe_path(&ops, target->path, screen->column == 0 ? PROBE_META : target->access);
    int64_t end = now_seconds();
    check->elapsed = start >= 0 && end >= start && end - start <= 2147483647 ?
        (long)(end - start) : -1;
    check->checked = true;
}

typedef enum { NONE, UP, DOWN, LEFT, RIGHT, CHECK, EXIT } Key;

static Key key_pressed(void) {
    if (isKeyPressed(KEY_NSPIRE_ESC)) return EXIT;
    if (isKeyPressed(KEY_NSPIRE_UP)) return UP;
    if (isKeyPressed(KEY_NSPIRE_DOWN)) return DOWN;
    if (isKeyPressed(KEY_NSPIRE_LEFT)) return LEFT;
    if (isKeyPressed(KEY_NSPIRE_RIGHT)) return RIGHT;
    if (isKeyPressed(KEY_NSPIRE_ENTER) || isKeyPressed(KEY_NSPIRE_RET)) return CHECK;
    return NONE;
}

int main(void) {
    if (nl_ndless_rev() < 2023 || !ui_canvas_open(&canvas)) {
        show_msgbox("EndraTask", "Run003 requires Ndless r2023 and a supported display.");
        return 1;
    }

    Screen screen = {0};
    Key previous = key_pressed();
    show(&screen, NULL);
    for (;;) {
        Key key = key_pressed();
        if (key == EXIT) break;
        if (previous == NONE && key != NONE) {
            switch (key) {
                case UP: if (screen.selected > 0) --screen.selected; break;
                case DOWN: if (screen.selected + 1 < TARGETS) ++screen.selected; break;
                case LEFT: screen.column = 0; break;
                case RIGHT: screen.column = 1; break;
                case CHECK: check_selected(&screen); break;
                default: break;
            }
            show(&screen, NULL);
        }
        previous = key;
        msleep(30);
    }
    ui_canvas_close(&canvas);
    refresh_osscr();
    return 0;
}
