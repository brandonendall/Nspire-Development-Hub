// SPDX-License-Identifier: GPL-3.0-only
#include "probe.h"

#include <assert.h>
#include <string.h>

static void expect(const char *log, enum UsbReport result, const char *expected) {
    FILE *report = tmpfile();
    assert(report);
    assert(fputs(log, report) >= 0);
    rewind(report);
    char stage[USB_STAGE_SIZE];
    assert(read_usb_stage(report, stage) == result);
    assert(strcmp(stage, expected) == 0);
    fclose(report);
}

int main(void) {
    expect("", USB_NO_STAGE, "");
    expect("[V2 DIAG] stage=list_ok\n", USB_NO_STAGE, "");
    expect("[V2 PROBE] stage=connect path=/documents/A.tns\r\n"
           "[V2 PROBE] stage=file_attr_start path=/documents/A.tns\r\n"
           "[V2 PROBE] stage=file_attr_error path=/documents/A.tns error=Busy",
           USB_STAGE_FOUND, "file_attr_error");
    expect("[V2 PROBE] stage=file_attr_ok size=10\n"
           "[V2 PROBE] stage=bad-stage\n", USB_STAGE_FOUND, "file_attr_ok");
    char oversized[8193];
    memset(oversized, 'a', sizeof(oversized) - 1);
    oversized[sizeof(oversized) - 1] = '\0';
    expect(oversized, USB_REPORT_LIMIT, "");
    return 0;
}
