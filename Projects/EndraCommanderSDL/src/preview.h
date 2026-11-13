#ifndef ENDRA_SDL_PREVIEW_H
#define ENDRA_SDL_PREVIEW_H
#include <stddef.h>
#define ES_PREVIEW_BYTES 2048
#define ES_PREVIEW_COLS 38
#define ES_PREVIEW_ROWS 96

typedef struct {
    char lines[ES_PREVIEW_ROWS][ES_PREVIEW_COLS + 1];
    unsigned count;
    unsigned truncated;
    size_t bytes;
} EsPreview;

/* Strictly read-only, bounded first 2 KiB; no fseek, ftell, heap allocations
 * or open file handles while user is browsing the preview. */
int es_preview_supported_name(const char *name);
int es_preview_decode(const unsigned char *data, size_t len, EsPreview *out);
int es_preview_read(const char *path, EsPreview *out);
#endif
