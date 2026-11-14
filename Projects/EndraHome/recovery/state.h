#ifndef ENDRAHOME_RECOVERY_STATE_H
#define ENDRAHOME_RECOVERY_STATE_H
enum {EH_NONE=0,EH_ESC,EH_MENU,EH_ENTER,EH_LEFT,EH_RIGHT};
typedef struct {
  int selected;
  int help;
  int quit;
  int last_action;
  int held;
} EhState;
void eh_state_init(EhState *s);
int eh_state_input(EhState *s,int key);
#endif
