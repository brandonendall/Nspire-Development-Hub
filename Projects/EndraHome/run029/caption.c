#include "caption.h"
#include <string.h>

void eh_caption(const char *label, int cols, char first[EH7_LABEL_CAP], char second[EH7_LABEL_CAP]) {
    if (!label || !*label) label = "App";
    if (cols < 4) cols = 4;
    if (cols >= EH7_LABEL_CAP) cols = EH7_LABEL_CAP - 1;
    size_t len = strlen(label);
    size_t width = (size_t)cols;
    size_t split = len < width ? len : width;
    if (len > width) {
        for (size_t i = split; i > 0; --i) {
            if (label[i] == ' ' || label[i] == '_' || label[i] == '-') { split = i; break; }
        }
    }
    memcpy(first, label, split);
    first[split] = 0;
    const char *rest = label + split;
    while (*rest == ' ' || *rest == '_' || *rest == '-') ++rest;
    size_t n = strlen(rest);
    size_t count = n < width ? n : width;
    memcpy(second, rest, count);
    second[count] = 0;
    if (n > width) memcpy(second + width - 3, "...", 3);
}
