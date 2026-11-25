#include "runtime.h"
#include "device.h"
#include "app.h"
#include "py/builtin.h"
#include "py/gc.h"
#include "py/objmodule.h"
#include <stdlib.h>
#include "py/compile.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "port/micropython_embed.h"
#include <string.h>

#include "shared/readline/readline.h"

static uintptr_t heap[128 * 1024 / sizeof(uintptr_t)];
static unsigned polls;
static int error_line;
static char error_file[NAME_CAP];

const char *runtime_error_file(void) { return error_file; }

mp_import_stat_t mp_import_stat(const char *path) {
    char name[NAME_CAP];
    return resolve_module_file(path, name) ? MP_IMPORT_STAT_FILE : MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    char name[NAME_CAP];
    const char *source;
    if (!resolve_module_file(qstr_str(filename), name))
        mp_raise_msg_varg(&mp_type_ImportError, MP_ERROR_TEXT("cannot find %q"), filename);
    qstr label = qstr_from_str(name);
    if (!read_script_source(name, &source))
        mp_raise_msg_varg(&mp_type_ImportError, MP_ERROR_TEXT("%q: %s"), label, app.status);
    size_t len = strlen(source);
    char *copy = m_new(char, len + 1);
    memcpy(copy, source, len + 1);
    return mp_lexer_new_from_str_len(label, copy, len, len + 1);
}

int runtime_error_line(void){return error_line;}
void runtime_start(void *stack_top) {
    project_io_start();
    device_reset();
    mp_embed_init(heap, sizeof heap, stack_top);
    mp_stack_set_limit(16 * 1024);
}
void runtime_end(void) {
    gc_sweep_all();
    mp_embed_deinit();
}

void ep_vm_poll(void) {
    if ((++polls & 63u) == 0 && cancel_pressed())
        nlr_raise(mp_obj_new_exception(&mp_type_KeyboardInterrupt));
}
void mp_hal_stdout_tx_strn_cooked(const char *data, size_t size) {
    console_write(data, size);
}
int runtime_exec(const char *name, const char *src, int interactive) {
    error_line=0;
    error_file[0]=0;
    nlr_buf_t handler;
    if (nlr_push(&handler) == 0) {
        qstr label = qstr_from_str(name);
        mp_lexer_t *lexer = mp_lexer_new_from_str_len(label, src, strlen(src), 0);
        mp_parse_tree_t tree = mp_parse(lexer, interactive ? MP_PARSE_SINGLE_INPUT : MP_PARSE_FILE_INPUT);
        mp_call_function_0(mp_compile(&tree, label, interactive));
        nlr_pop();
        return 1;
    }
    mp_obj_t exc=MP_OBJ_FROM_PTR(handler.ret_val);
    if(mp_obj_is_exception_instance(exc)){
        size_t n,*frames;
        mp_obj_exception_get_traceback(exc,&n,&frames);
        for(size_t i=0;i+2<n;i+=3){
            const char *file = qstr_str(frames[i]);
            if (valid_script(file) && frames[i+1] > 0) {
                strcpy(error_file, file);
                error_line = (int)frames[i+1];
                break;
            }
        }
    }
    mp_obj_print_exception(&mp_plat_print,exc);
    return 0;
}

const char endra_help_text[] =
    "EndraPython shell\n"
    "help('modules')   List libraries\n"
    "import math       Load a module\n"
    "dir(math)         List its names\n"
    "help(math.sqrt)   Inspect an object\n"
    "Up recalls your last command.\n"
    "Left/Right scroll output.\n";

int readline(vstr_t *line,const char *prompt){
    (void)prompt;
    Page caller=app.page,origin=app.origin;
    app.page=READ_INPUT;app.reply[0]=0;app.input_result=0;
    platform_wait_input();
    app.page=caller;app.origin=origin;
    if(app.input_result<=0){console_write("\n",1);return CHAR_CTRL_C;}
    vstr_add_str(line,app.reply);
    console_write(app.reply,strlen(app.reply));console_write("\n",1);
    return 0;
}

static void catalog_add_module(const char *name, size_t len) {
    if (!len || len >= sizeof catalog_groups[0] || catalog_group_count >= CATALOG_GROUP_CAP) return;
    for (int i = 1; i < catalog_group_count; i++)
        if (!strncmp(catalog_groups[i], name, len) && !catalog_groups[i][len]) return;
    memcpy(catalog_groups[catalog_group_count], name, len);
    catalog_groups[catalog_group_count++][len] = 0;
}

void runtime_catalog_modules(void) {
    const mp_map_t *maps[] = { &mp_builtin_module_map, &mp_builtin_extensible_module_map };
    catalog_group_count = 1;
    strcpy(catalog_groups[0], "Common");
    for (size_t i = 0; i < sizeof maps / sizeof maps[0]; i++)
        for (size_t j = 0; j < maps[i]->alloc; j++) {
            if (!mp_map_slot_is_filled(maps[i], j)) continue;
            const char *name = mp_obj_str_get_str(maps[i]->table[j].key);
            catalog_add_module(name, strlen(name));
        }
    extern const char mp_frozen_names[];
    for (const char *name = mp_frozen_names; *name; name += strlen(name) + 1)
        catalog_add_module(name, strcspn(name, "/."));
}

static int catalog_copy_members(const mp_map_t *map, const char *prefix, int count) {
    for (size_t i = 0; i < map->alloc && count < CATALOG_CAP; i++) {
        if (!mp_map_slot_is_filled(map, i)) continue;
        const mp_map_elem_t *entry = &map->table[i];
        const char *name = mp_obj_str_get_str(entry->key);
        if (*name == '_' || strlen(prefix)+strlen(name) >= sizeof catalog_entries[0].name) continue;
        strcpy(catalog_entries[count].name, prefix);
        strcat(catalog_entries[count].name, name);
        catalog_entries[count++].callable = mp_obj_is_callable(entry->value);
    }
    return count;
}

/* Copy names before closing a temporary editor VM. Shell browsing keeps its VM. */
int runtime_catalog(const char *module) {
    nlr_buf_t handler;
    if (nlr_push(&handler) != 0) return 0;
    mp_obj_t obj = mp_import_name(qstr_from_str(module), mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
    if (!mp_obj_is_type(obj, &mp_type_module)) { nlr_pop(); return 0; }
    const mp_map_t *map = &mp_obj_module_get_globals(obj)->map;
    int count = catalog_copy_members(map, "", 0);
    if (!strcmp(module, "pygame")) {
        const char *const namespaces[] = {"display","draw","event","key","sprite","font","image","time"};
        for (size_t i = 0; i < sizeof namespaces / sizeof namespaces[0]; i++) {
            const mp_map_elem_t *entry = mp_map_lookup((mp_map_t *)map,
                MP_OBJ_NEW_QSTR(qstr_from_str(namespaces[i])), MP_MAP_LOOKUP);
            if (!entry) continue;
            const mp_obj_type_t *type = mp_obj_get_type(entry->value);
            const mp_map_t *members;
            if (type == &mp_type_module) members = &mp_obj_module_get_globals(entry->value)->map;
            else if (MP_OBJ_TYPE_HAS_SLOT(type, locals_dict))
                members = &MP_OBJ_TYPE_GET_SLOT(type, locals_dict)->map;
            else continue;
            char prefix[16];
            strcpy(prefix, namespaces[i]); strcat(prefix, ".");
            count = catalog_copy_members(members, prefix, count);
        }
    }
    nlr_pop();
    return count;
}
