#ifndef EH7_UI_H
#define EH7_UI_H
#include "../run005/logic7.h"
#include <stddef.h>
enum { EH7_NONE=0, EH7_ZIP=1, EH7_TOOLS=5 };
typedef struct {
  EhUI7 page;
  int confirm_target; /* EH7_NONE, EH7_ZIP, EH7_TOOLS */
  int request_target; /* nonzero only after explicit second confirmation */
} EhUI7Launch;
void eh7launch_init(EhUI7Launch *s);
int eh7launch_handle(EhUI7Launch *s,EhKey key);
int eh7launch_sibling(char *dst,size_t cap,const char *exepath,int target);
#endif
