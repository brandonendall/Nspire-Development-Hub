#ifndef EH013_UI_H
#define EH013_UI_H
#include "../run005/logic7.h"
#include "registry.h"
typedef struct {
 EhUI7 page;
 const EhRegistry *apps;
 int missing;
 int request;       /* 1..7, launches app from Home with one Enter */
 int assign_slot;   /* 1..7, open file picker; does NOT launch */
 char notice[48];   /* brief non-modal status after saving a shortcut */
} EhUniversalUI;
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps);
int eh7_ui_handle(EhUniversalUI *s,EhKey key);
#endif
