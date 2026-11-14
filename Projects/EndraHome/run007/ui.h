#ifndef EH007_UI_H
#define EH007_UI_H
#include "../run005/logic7.h"
#include "registry.h"
typedef struct {
 EhUI7 page;
 const EhRegistry *apps;
 int confirm;  /* index + 1, zero = no confirmation */
 int missing;  /* show setup instructions on unconfigured tile */
 int request;  /* index + 1, zero = no launch requested */
} EhUniversalUI;
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps);
int eh7_ui_handle(EhUniversalUI *s,EhKey key);
#endif
