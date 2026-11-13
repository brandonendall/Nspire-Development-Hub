#ifndef NX_H
#define NX_H
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#define NX_PATH 512
#define NX_NAME 256
#define NX_MAX_ENTRIES 4096
typedef struct { char name[NX_NAME]; uint64_t size; time_t modified; int directory, selected; } NxEntry;
typedef struct { char path[NX_PATH]; NxEntry *items; size_t count, cursor, scroll; int sort, folders_only; char filter[64]; } NxPane;
extern char nx_error[160];
extern char nx_write_root[NX_PATH];
extern int (*nx_progress)(const char *, uint64_t);
int nx_fail(const char *message);
int nx_normalize(char *out, const char *path);
int nx_join(char *out, const char *dir, const char *name);
int nx_parent(char *path);
int nx_name_valid(const char *name);
int nx_exists(const char *path);
int nx_is_dir(const char *path);
int nx_writable(const char *path);
int nx_scan(NxPane *pane);
void nx_release(NxPane *pane);
void nx_sort(NxPane *pane);
int nx_copy(const char *source, const char *destination);
int nx_move(const char *source, const char *destination);
int nx_delete(const char *path);
int nx_mkdir(const char *path);
int nx_kind(const char *path);
void nx_size(char *out, size_t length, uint64_t size);
int nx_zip_create(const char *source, const char *archive);
int nx_zip_extract(const char *archive, const char *destination);
int nx_zip_list(const char *archive, char *out, size_t length);
int nx_zip_entry_valid(const char *name);
#endif
