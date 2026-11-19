// SPDX-License-Identifier: GPL-3.0-only
#include "diagnostic.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

static ProbeStage stages[4];
static unsigned stage_count;
static unsigned calls;
static ProbeCall open_error;
static ProbeCall read_error;
static ProbeCall close_error;
static int fake_handle;

static void stage(void *context, ProbeStage next) {
    assert(context == stages && stage_count < 4 && stage_count == calls);
    stages[stage_count++] = next;
}

static void check_call(ProbeStage expected) {
    assert(stage_count == calls + 1);
    assert(stages[calls++] == expected);
}

static ProbeCall meta(const char *path, unsigned long *size, unsigned long *mode) {
    check_call(STAGE_STAT);
    assert(strcmp(path, "/control") == 0);
    *size = 42;
    *mode = 0x8000;
    return (ProbeCall){0, 0};
}

static ProbeCall open_path(const char *path, ProbeMode mode, void **handle) {
    check_call(mode == PROBE_DIR ? STAGE_OPEN_DIR : STAGE_OPEN_FILE);
    assert(strcmp(path, "/control") == 0);
    if (!open_error.code) *handle = &fake_handle;
    return open_error;
}

static ProbeCall read_byte(void *handle, bool *eof) {
    assert(handle == &fake_handle);
    check_call(STAGE_READ_BYTE);
    *eof = !read_error.code;
    return read_error;
}

static ProbeCall close_path(void *handle, ProbeMode mode) {
    assert(handle == &fake_handle);
    check_call(mode == PROBE_DIR ? STAGE_CLOSE_DIR : STAGE_CLOSE_FILE);
    return close_error;
}

static ProbeResult run(ProbeMode mode, unsigned expected_calls) {
    stage_count = calls = 0;
    ProbeOps ops = {stages, stage, meta, open_path, read_byte, close_path};
    ProbeResult result = probe_path(&ops, "/control", mode);
    assert(stage_count == expected_calls && calls == expected_calls);
    return result;
}

int main(void) {
    ProbeResult result = run(PROBE_META, 1);
    assert(result.stage == STAGE_STAT && result.call.code == 0 && result.size == 42);
    result = run(PROBE_DIR, 2);
    assert(result.stage == STAGE_CLOSE_DIR && result.call.code == 0);
    result = run(PROBE_FILE, 3);
    assert(result.stage == STAGE_CLOSE_FILE && result.call.code == 0 && result.eof);

    open_error = (ProbeCall){-1, EBUSY};
    result = run(PROBE_DIR, 1);
    assert(result.stage == STAGE_OPEN_DIR && strcmp(probe_status(result.call), "BUSY") == 0);

    open_error = (ProbeCall){0, 0};
    close_error = (ProbeCall){-1, EACCES};
    result = run(PROBE_FILE, 3);
    assert(result.stage == STAGE_CLOSE_FILE && strcmp(probe_status(result.call), "DENIED") == 0);

    read_error = (ProbeCall){-1, EIO};
    result = run(PROBE_FILE, 3);
    assert(result.stage == STAGE_READ_BYTE && strcmp(probe_status(result.call), "I/O ERROR") == 0);
    assert(strcmp(probe_status((ProbeCall){-1, 0}), "FAILED") == 0);
    assert(strcmp(probe_status((ProbeCall){-1, ENOENT}), "MISSING") == 0);
    return 0;
}
