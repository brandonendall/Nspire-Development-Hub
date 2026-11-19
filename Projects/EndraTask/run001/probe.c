// SPDX-License-Identifier: GPL-3.0-only
#include "probe.h"

#include <string.h>

#define MAX_REPORT_BYTES 8192
#define MAX_LINE_BYTES 512

static void record_stage(char *line, char stage[USB_STAGE_SIZE]) {
    const char *prefix = "[V2 PROBE] stage=";
    const char *start = line + strlen(prefix);
    if (strncmp(line, prefix, strlen(prefix)) != 0) return;
    size_t len = strspn(start, "abcdefghijklmnopqrstuvwxyz0123456789_");
    if (len && len < USB_STAGE_SIZE &&
        (start[len] == ' ' || start[len] == '\r' || start[len] == '\0')) {
        memcpy(stage, start, len);
        stage[len] = '\0';
    }
}

enum UsbReport read_usb_stage(FILE *report, char stage[USB_STAGE_SIZE]) {
    char line[MAX_LINE_BYTES];
    size_t len = 0;
    int ch;
    stage[0] = '\0';
    for (unsigned i = 0; i < MAX_REPORT_BYTES; ++i) {
        ch = fgetc(report);
        if (ch == EOF || ch == '\n') {
            if (len < sizeof(line)) {
                line[len] = '\0';
                record_stage(line, stage);
            }
            len = 0;
            if (ch == EOF) {
                if (ferror(report)) return USB_REPORT_ERROR;
                return stage[0] ? USB_STAGE_FOUND : USB_NO_STAGE;
            }
        } else if (len < sizeof(line)) {
            // Reject binary lines and discard lines that exceed the buffer.
            if (ch == '\0') len = sizeof(line);
            else line[len++] = (char)ch;
        }
    }
    return USB_REPORT_LIMIT;
}
