#include "toast.h"
#include <stdio.h>

void eh_toast_set(EhToast *toast, const char *message) {
    snprintf(toast->text, sizeof(toast->text), "%s", message);
    toast->pending = 1;
}

int eh_toast_tick(EhToast *toast, uint32_t now) {
    if (!toast->text[0]) return 0;
    if (toast->pending) {
        toast->start = now;
        toast->pending = 0;
    } else if ((uint32_t)(now - toast->start) >= 3u * EH_TICKS_PER_SECOND) {
        toast->text[0] = '\0';
        return 1;
    }
    return 0;
}
