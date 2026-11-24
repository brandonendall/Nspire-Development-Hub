#include "app.h"
#include "py/builtin.h"
#include "py/runtime.h"
#include "py/stream.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

/* Script I/O has a virtual working directory. Browsing or importing modules
 * continues to use the editor's project folder. */
static char working_dir[PATH_CAP];

void project_io_start(void) {
    working_dir[0] = 0;
}

static void prepare_home_for_write(void) {
    if (!strcmp(app.home, app.root) && !ensure_scripts_folder())
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("%s"), app.status);
}

static void file_error(void) {
    mp_raise_OSError(errno ? errno : EIO);
}

static void project_path(mp_obj_t path, char full[PATH_CAP]) {
    if (!mp_obj_is_str(path)) mp_raise_TypeError(MP_ERROR_TEXT("expected a path string"));
    size_t size;
    const char *src = mp_obj_str_get_data(path, &size);
    if (!size || memchr(src, 0, size)) mp_raise_ValueError(MP_ERROR_TEXT("invalid path"));
    size_t base = strlen(app.root);
    const char *cwd = src[0] == '/' ? "" : working_dir;
    size_t used = base + strlen(cwd);
    if (used >= PATH_CAP) mp_raise_OSError(ENAMETOOLONG);
    memcpy(full, app.root, base);
    strcpy(full + base, cwd);
    while (*src) {
        if (*src == '/') { src++; continue; }
        size_t n = strcspn(src, "/");
        for (size_t i = 0; i < n; i++) {
            unsigned char c = src[i];
            if (c < 32 || c == 127 || c == '\\' || c == ':')
                mp_raise_ValueError(MP_ERROR_TEXT("invalid path"));
        }
        if (n == 2 && !memcmp(src, "..", 2)) {
            if (used == base) mp_raise_OSError(EACCES);
            used = strrchr(full, '/') - full;
            full[used] = 0;
        } else if (!(n == 1 && src[0] == '.')) {
            if (n == 7 && !strncasecmp(src, ".saving", n)) mp_raise_OSError(EACCES);
            if (used + 1 + n >= PATH_CAP) mp_raise_OSError(ENAMETOOLONG);
            full[used++] = '/';
            memcpy(full + used, src, n);
            used += n;
            full[used] = 0;
#ifndef __arm__
            struct stat st;
            if (!lstat(full, &st) && S_ISLNK(st.st_mode)) mp_raise_OSError(EACCES);
#endif
        }
        src += n;
    }
}

typedef struct {
    mp_obj_base_t base;
    FILE *fp;
    bool writing;
} ProjectFile;

static mp_uint_t stream_read(mp_obj_t obj, void *buf, mp_uint_t size, int *err) {
    ProjectFile *f = MP_OBJ_TO_PTR(obj);
    if (!f->fp || f->writing) { *err = EBADF; return MP_STREAM_ERROR; }
    size_t n = fread(buf, 1, size, f->fp);
    if (ferror(f->fp)) { *err = errno ? errno : EIO; return MP_STREAM_ERROR; }
    return n;
}

static mp_uint_t stream_write(mp_obj_t obj, const void *buf, mp_uint_t size, int *err) {
    ProjectFile *f = MP_OBJ_TO_PTR(obj);
    if (!f->fp || !f->writing) { *err = EBADF; return MP_STREAM_ERROR; }
    if (fwrite(buf, 1, size, f->fp) != size) {
        *err = errno ? errno : EIO;
        return MP_STREAM_ERROR;
    }
    return size;
}

static mp_uint_t stream_ioctl(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *err) {
    ProjectFile *f = MP_OBJ_TO_PTR(obj);
    if (request == MP_STREAM_CLOSE) {
        FILE *fp = f->fp;
        f->fp = NULL;
        if (!fp || !fclose(fp)) return 0;
    } else if (!f->fp) {
        *err = EBADF;
        return MP_STREAM_ERROR;
    } else if (request == MP_STREAM_FLUSH) {
        if (!f->writing || !fflush(f->fp)) return 0;
    } else if (request == MP_STREAM_SEEK) {
        struct mp_stream_seek_t *s = (void *)arg;
        if (!fseek(f->fp, s->offset, s->whence)) {
            long pos = ftell(f->fp);
            if (pos >= 0) { s->offset = pos; return 0; }
        }
    } else {
        *err = EINVAL;
        return MP_STREAM_ERROR;
    }
    *err = errno ? errno : EIO;
    return MP_STREAM_ERROR;
}

static const mp_rom_map_elem_t file_methods[] = {
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj)},
    {MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    {MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj)},
    {MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj)},
    {MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj)},
    {MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj)},
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj)},
    {MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj)},
    {MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj)},
};
static MP_DEFINE_CONST_DICT(file_dict, file_methods);
static const mp_stream_p_t text_stream = {
    .read=stream_read, .write=stream_write, .ioctl=stream_ioctl, .is_text=true,
};
static const mp_stream_p_t binary_stream = {
    .read=stream_read, .write=stream_write, .ioctl=stream_ioctl,
};
static MP_DEFINE_CONST_OBJ_TYPE(text_file, MP_QSTR_TextIOWrapper, MP_TYPE_FLAG_ITER_IS_STREAM,
    protocol, &text_stream, locals_dict, &file_dict);
static MP_DEFINE_CONST_OBJ_TYPE(binary_file, MP_QSTR_FileIO, MP_TYPE_FLAG_ITER_IS_STREAM,
    protocol, &binary_stream, locals_dict, &file_dict);

mp_obj_t mp_builtin_open(size_t argc, const mp_obj_t *argv, mp_map_t *kwargs) {
    static const mp_arg_t signature[] = {
        {MP_QSTR_file, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        {MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_r)}},
    };
    mp_arg_val_t args[2];
    mp_arg_parse_all(argc, argv, kwargs, 2, signature, args);
    char full[PATH_CAP];
    project_path(args[0].u_obj, full);
    size_t n;
    const char *mode = mp_obj_str_get_data(args[1].u_obj, &n);
    if (!n || n > 2 || !memchr("rwa", mode[0], 3) ||
        (n == 2 && mode[1] != 't' && mode[1] != 'b'))
        mp_raise_ValueError(MP_ERROR_TEXT("use r, w or a with optional b or t"));
    bool binary = n == 2 && mode[1] == 'b';
    ProjectFile *f = mp_obj_malloc_with_finaliser(ProjectFile, binary ? &binary_file : &text_file);
    char native_mode[] = {mode[0], 'b', 0};
    f->writing = mode[0] != 'r';
    f->fp = NULL;
    if (f->writing) prepare_home_for_write();
    f->fp = fopen(full, native_mode);
    if (!f->fp) file_error();
#ifdef __arm__
    /* The pinned SDK can return descriptor zero for a failed create. */
    if (fileno(f->fp) < 3) {
        fclose(f->fp);
        f->fp = NULL;
        mp_raise_OSError(EIO);
    }
#endif
    return MP_OBJ_FROM_PTR(f);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 0, mp_builtin_open);

static mp_obj_t project_getcwd(void) {
    return *working_dir ? mp_obj_new_str(working_dir, strlen(working_dir)) : MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
}
static MP_DEFINE_CONST_FUN_OBJ_0(getcwd_obj, project_getcwd);

static mp_obj_t project_chdir(mp_obj_t name) {
    char path[PATH_CAP];
    project_path(name, path);
    DIR *d = opendir(path);
    if (!d) file_error();
    closedir(d);
    strcpy(working_dir, path + strlen(app.root));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(chdir_obj, project_chdir);

static mp_obj_t project_mkdir(mp_obj_t name) {
    char path[PATH_CAP];
    project_path(name, path);
    prepare_home_for_write();
    if (mkdir(path, 0755)) file_error();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mkdir_obj, project_mkdir);

static mp_obj_t project_listdir(size_t argc, const mp_obj_t *args) {
    char path[PATH_CAP];
    project_path(argc ? args[0] : mp_obj_new_str(".", 1), path);
    mp_obj_t entries = mp_obj_new_list(0, NULL);
    DIR *d = opendir(path);
    if (!d) file_error();
    nlr_buf_t cleanup;
    if (nlr_push(&cleanup) != 0) {
        closedir(d);
        nlr_jump(cleanup.ret_val);
    }
    struct dirent *entry;
    for (;;) {
        errno = 0;
        entry = readdir(d);
        if (!entry) {
#ifndef __arm__
            if (errno) file_error();
#endif
            break;
        }
        const char *s = entry->d_name;
        if (strcmp(s, ".") && strcmp(s, "..") && strcasecmp(s, ".saving"))
            mp_obj_list_append(entries, mp_obj_new_str(s, strlen(s)));
    }
    closedir(d);
    nlr_pop();
    return entries;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(listdir_obj, 0, 1, project_listdir);

static const mp_rom_map_elem_t os_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_os)},
    {MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&getcwd_obj)},
    {MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&chdir_obj)},
    {MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mkdir_obj)},
    {MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&listdir_obj)},
};
static MP_DEFINE_CONST_DICT(os_dict, os_table);
const mp_obj_module_t project_os = {.base={&mp_type_module}, .globals=(mp_obj_dict_t *)&os_dict};
MP_REGISTER_MODULE(MP_QSTR_os, project_os);
