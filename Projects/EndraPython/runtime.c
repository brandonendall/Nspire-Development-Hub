#include "runtime.h"
#include "app.h"
#include "py/builtin.h"
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
    mp_embed_init(heap, sizeof heap, stack_top);
    mp_stack_set_limit(16 * 1024);
}
void runtime_end(void) { mp_embed_deinit(); }

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

/* Copy names before closing a temporary editor VM. Shell browsing keeps its VM. */
int runtime_catalog(const char *module){
    nlr_buf_t handler;
    if(nlr_push(&handler)!=0)return 0;
    mp_obj_t obj=mp_import_name(qstr_from_str(module),mp_const_none,MP_OBJ_NEW_SMALL_INT(0));
    mp_obj_module_t *mod=MP_OBJ_TO_PTR(obj);
    const mp_map_t *map=&mod->globals->map;
    int count=0;
    for(size_t i=0;i<map->alloc && count<CATALOG_CAP;i++){
        if(!mp_map_slot_is_filled(map,i))continue;
        const mp_map_elem_t *e=map->table+i;
        const char *name=mp_obj_str_get_str(e->key);
        if(*name=='_' || strlen(name)>=sizeof catalog_entries[0].name)continue;
        strcpy(catalog_entries[count].name,name);
        catalog_entries[count++].callable=mp_obj_is_callable(e->value);
    }
    nlr_pop();return count;
}
