#ifndef EH25_FILE_OPS_H
#define EH25_FILE_OPS_H
#include <stddef.h>
#define EH25_MAX_DEPTH 16
#define EH25_MAX_ITEMS 4096
/* 0 success, nonzero failure. Never overwrite an existing destination. */
int eh25_copy(const char *src,const char *dst);
int eh25_move(const char *src,const char *dst);
int eh25_remove(const char *path);
int eh25_safe_name(const char *name);
int eh25_inside(const char *parent,const char *child);
#endif
