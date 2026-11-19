// SPDX-License-Identifier: GPL-3.0-or-later
#include <cassert>
#include <libusb.h>

#include "cx2.h"
#include "error.h"
#include "handle.h"

static int input_calls;
static int output_calls;
static int write_result;

extern "C" int LIBUSB_CALL libusb_bulk_transfer(libusb_device_handle *,
    unsigned char endpoint, unsigned char *, int length, int *transferred, unsigned int) {
    if (endpoint == 0x81) {
        ++input_calls;
        *transferred = 0;
        return LIBUSB_ERROR_TIMEOUT;
    }
    assert(endpoint == 0x01);
    ++output_calls;
    *transferred = write_result ? 0 : length;
    return write_result;
}

int main() {
    nspire_handle handle = {};
    char data[] = {0};
    assert(packet_send_cx2(&handle, data, sizeof(data)) == -NSPIRE_ERR_BUSY);
    assert(input_calls == 10 && output_calls == 0);

    handle.cx2_handshake_complete = true;
    write_result = LIBUSB_ERROR_NO_DEVICE;
    assert(packet_send_cx2(&handle, data, sizeof(data)) == -NSPIRE_ERR_BUSY);
    assert(input_calls == 10 && output_calls == 1);

    write_result = 0;
    assert(packet_send_cx2(&handle, data, sizeof(data)) == -NSPIRE_ERR_BUSY);
    assert(input_calls == 20 && output_calls == 2);
}
