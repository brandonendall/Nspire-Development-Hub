// SPDX-License-Identifier: GPL-3.0-only
#ifndef ENDRATASK_PROBE_H
#define ENDRATASK_PROBE_H

#include <stdio.h>

#define USB_STAGE_SIZE 40
#define USB_REPORT "/documents/EndraTask_USB.txt.tns"

enum UsbReport { USB_NO_STAGE, USB_STAGE_FOUND, USB_REPORT_LIMIT, USB_REPORT_ERROR };
enum UsbReport read_usb_stage(FILE *report, char stage[USB_STAGE_SIZE]);

#endif
