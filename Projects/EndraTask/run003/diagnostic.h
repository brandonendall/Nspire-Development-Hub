// SPDX-License-Identifier: GPL-3.0-only
#ifndef ENDRATASK_DIAGNOSTIC_H
#define ENDRATASK_DIAGNOSTIC_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PROBE_META, PROBE_DIR, PROBE_FILE
} ProbeMode;

typedef enum {
    STAGE_STAT, STAGE_OPEN_DIR, STAGE_CLOSE_DIR,
    STAGE_OPEN_FILE, STAGE_READ_BYTE, STAGE_CLOSE_FILE
} ProbeStage;

typedef struct {
    int code;
    int error;
} ProbeCall;

typedef struct {
    ProbeStage stage;
    ProbeCall call;
    unsigned long size;
    unsigned long mode;
    bool eof;
} ProbeResult;

typedef struct {
    void *context;
    void (*stage)(void *context, ProbeStage stage);
    ProbeCall (*metadata)(const char *path, unsigned long *size, unsigned long *mode);
    ProbeCall (*open)(const char *path, ProbeMode mode, void **handle);
    ProbeCall (*read_byte)(void *handle, bool *eof);
    ProbeCall (*close)(void *handle, ProbeMode mode);
} ProbeOps;

ProbeResult probe_path(const ProbeOps *ops, const char *path, ProbeMode mode);
const char *probe_status(ProbeCall call);
const char *probe_stage(ProbeStage stage);

#endif
