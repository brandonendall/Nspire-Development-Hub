#ifndef EH_GBA_UI_LOGIC_H
#define EH_GBA_UI_LOGIC_H
typedef enum { EH_KEY_NONE,EH_KEY_UP,EH_KEY_DOWN,EH_KEY_LEFT,EH_KEY_RIGHT,
               EH_KEY_ENTER,EH_KEY_ESC,EH_KEY_MENU } EhKey;
typedef enum {EH_PAGE_HOME,EH_PAGE_DETAIL,EH_PAGE_HELP,EH_PAGE_ABOUT} EhPage;
typedef struct {EhPage page;int selected;int should_exit;unsigned handled;} EhUI;
void eh_ui_init(EhUI *ui);
int eh_ui_dispatch(EhUI *ui,EhKey key);
#endif
