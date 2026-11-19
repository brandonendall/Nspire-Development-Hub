// SPDX-License-Identifier: GPL-3.0-only
#include "diagnostic.h"

#include <errno.h>
#include <stddef.h>

ProbeResult probe_path(const ProbeOps *ops, const char *path, ProbeMode mode) {
    ProbeResult result = {0};
    if (mode == PROBE_META) {
        result.stage = STAGE_STAT;
        ops->stage(ops->context, result.stage);
        result.call = ops->metadata(path, &result.size, &result.mode);
        return result;
    }

    bool file = mode == PROBE_FILE;
    void *handle = NULL;
    result.stage = file ? STAGE_OPEN_FILE : STAGE_OPEN_DIR;
    ops->stage(ops->context, result.stage);
    result.call = ops->open(path, mode, &handle);
    if (result.call.code != 0) return result;

    if (file) {
        result.stage = STAGE_READ_BYTE;
        ops->stage(ops->context, result.stage);
        result.call = ops->read_byte(handle, &result.eof);
    }

    ProbeStage close_stage = file ? STAGE_CLOSE_FILE : STAGE_CLOSE_DIR;
    ops->stage(ops->context, close_stage);
    ProbeCall close_result = ops->close(handle, mode);
    if (result.call.code == 0) {
        result.stage = close_stage;
        result.call = close_result;
    }
    return result;
}

const char *probe_status(ProbeCall call) {
    if (call.code == 0) return "OK";
    switch (call.error) {
        case EBUSY: return "BUSY";
        case ENOENT: return "MISSING";
        case ENOTDIR: return "NOT A DIR";
        case EISDIR: return "IS A DIR";
        case EACCES:
        case EPERM: return "DENIED";
        case EIO: return "I/O ERROR";
#ifdef ETIMEDOUT
        case ETIMEDOUT: return "TIMEOUT";
#endif
        default: return "FAILED";
    }
}

const char *probe_stage(ProbeStage stage) {
    static const char *const names[] = {
        "Metadata (stat)", "Open directory", "Close directory",
        "Open file (read-only)", "Read first byte", "Close file"
    };
    return names[stage];
}
