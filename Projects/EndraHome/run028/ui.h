#ifndef EH016_UI_H
#define EH016_UI_H
#include "../run005/logic7.h"
#include "registry.h"
#define EH15_MENU_COUNT 6
enum {EH15_NONE=0, EH15_RENAME=1, EH15_REMOVE=2, EH15_THEME=3};
typedef struct {
 EhUI7 page;
 const EhRegistry *apps;
 int missing,request,assign_slot,edit_action,open_files;
 int manager_item,subview,renaming,confirm_remove;
 int preview_theme;
 char rename_input[EH7_LABEL_CAP],notice[48];
} EhUniversalUI;
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps);
int eh7_ui_handle(EhUniversalUI *s,EhKey key);
/* Input from physical TI-Nspire QWERTY; pure function safe in host tests.
 * ASCII backspace (8 or 127) deletes. 1=>redraw, 0=>ignored. */
int eh16_ui_type(EhUniversalUI *s,int c);
#endif
