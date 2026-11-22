#include "runtime.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "port/micropython_embed.h"
#include <string.h>

static uintptr_t heap[128 * 1024 / sizeof(uintptr_t)];
static unsigned polls;
static int error_line;
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
            if(!strcmp(qstr_str(frames[i]),name) && frames[i+1]>0){
                error_line=(int)frames[i+1];break;
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
