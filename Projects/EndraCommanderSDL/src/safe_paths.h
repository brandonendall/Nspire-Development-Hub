#ifndef ENDRA_SDL_SAFE_PATHS_H
#define ENDRA_SDL_SAFE_PATHS_H
#include <stddef.h>
#define ES_PATH_MAX 512
/* Every app path is an absolute child of Documents; never scan system root. */
int es_path_allowed(const char *path);
int es_name_allowed(const char *name);
int es_path_join(char *dst, size_t capacity, const char *parent, const char *name);
int es_path_parent(char *path);
#endif
