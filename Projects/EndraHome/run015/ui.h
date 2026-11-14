#ifndef EH015_UI_H
#define EH015_UI_H
#include "../run005/logic7.h"
#include "registry.h"
#define EH15_MENU_COUNT 5
#define EH15_PALETTE_SIZE 42
enum {EH15_NONE=0,EH15_RENAME=1,EH15_REMOVE=2};
typedef struct {
 EhUI7 page;const EhRegistry *apps;
 int missing,request,assign_slot,edit_action;
 int manager_item,subview,renaming,confirm_remove,rename_cursor;
 char rename_input[EH7_LABEL_CAP],notice[48];
} EhUniversalUI;
extern const char eh15_palette[41];
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps);
int eh7_ui_handle(EhUniversalUI *s,EhKey key);
#endif
