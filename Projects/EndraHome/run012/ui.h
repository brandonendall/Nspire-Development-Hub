#ifndef EH012_UI_H
#define EH012_UI_H
#include "../run005/logic7.h"
#include "registry.h"
typedef struct {
 EhUI7 page;
 const EhRegistry *apps;
 int missing; /* informational overlay only for unconfigured slot */
 int request; /* slot index + 1, cleared when child returns */
} EhUniversalUI;
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps);
int eh7_ui_handle(EhUniversalUI *s,EhKey key);
#endif
