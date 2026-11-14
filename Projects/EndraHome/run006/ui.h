#ifndef EH6_UI_H
#define EH6_UI_H
#include "../run005/logic7.h"
#include <stddef.h>
typedef struct {
 EhUI7 page;
 int confirm;
 int request_launch;
} EhUI6;
void eh6_init(EhUI6 *s);
int eh6_handle(EhUI6 *s,EhKey key);
int eh6_sibling_path(char *dst,size_t size,const char *exe);
#endif
