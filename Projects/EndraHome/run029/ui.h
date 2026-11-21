#ifndef EH016_UI_H
#define EH016_UI_H
#include "../run005/logic7.h"
#include "registry.h"
#include "toast.h"
enum {EH_MANAGER_ADD, EH_MANAGER_REASSIGN, EH_MANAGER_RENAME, EH_MANAGER_REMOVE,
      EH_MANAGER_INFO, EH_MANAGER_APPEARANCE, EH_MANAGER_FILES, EH_MANAGER_TI,
      EH15_MENU_COUNT};
enum {EH15_NONE=0, EH15_RENAME=1, EH15_REMOVE=2, EH15_THEME=3, EH15_NATIVE_CHECK=4};
enum {EH_LOOK_THEME, EH_LOOK_FOLDER, EH_LOOK_TEXT, EH_LOOK_WALLPAPER,
      EH_LOOK_ICON, EH_LOOK_DEFAULTS};
typedef struct {
 EhUI7 page;
 const EhRegistry *apps;
 int missing,request,assign_slot,adding_app,edit_action,open_files;
 int manager_item,subview,renaming,confirm_remove;
 int setting_row, native_status;
 EhAppearance preview;
 EhToast notice;
 char rename_input[EH7_LABEL_CAP];
} EhUniversalUI;
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps);
int eh7_ui_handle(EhUniversalUI *s,EhKey key);
/* Input from physical TI-Nspire QWERTY; pure function safe in host tests.
 * ASCII backspace (8 or 127) deletes. 1=>redraw, 0=>ignored. */
int eh16_ui_type(EhUniversalUI *s,int c);
#endif
