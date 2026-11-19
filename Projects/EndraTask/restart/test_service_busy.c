// SPDX-License-Identifier: GPL-3.0-or-later
#include <assert.h>

#include "data.h"
#include "error.h"
#include "service.h"

static int cleanup_result;

struct packet packet_new(nspire_handle_t *handle) {
    (void)handle;
    return (struct packet){0};
}

int data_write_special(nspire_handle_t *handle, void *payload, size_t length,
                       void (*modify)(struct packet *)) {
    (void)payload;
    (void)modify;
    assert(handle->connected && length == 2);
    return cleanup_result;
}

int main(void) {
    nspire_handle_t handle = {0};
    assert(service_connect(&handle, 0x4060) == 0);
    assert(service_connect(&handle, 0x4060) == -NSPIRE_ERR_BUSY);
    cleanup_result = -NSPIRE_ERR_TIMEOUT;
    assert(service_disconnect(&handle) == -NSPIRE_ERR_TIMEOUT && handle.connected);
    cleanup_result = 0;
    assert(service_disconnect(&handle) == 0 && !handle.connected);
}
