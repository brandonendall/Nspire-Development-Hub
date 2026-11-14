#include "state.h"
void eh_state_init(EhState *s) {
  s->selected=0; s->help=0; s->quit=0; s->last_action=0; s->held=0;
}
/* Key edges only: do not repeatedly activate while a key stays pressed. */
int eh_state_input(EhState *s,int key) {
  if(key==EH_NONE){s->held=0;return 0;}
  if(s->held || s->quit)return 0;
  s->held=1;
  if(key==EH_ESC){s->quit=1;return 1;}
  if(s->help){
    if(key==EH_MENU || key==EH_ENTER){s->help=0;return 1;}
    return 0;
  }
  if(key==EH_MENU){s->help=1;return 1;}
  if(key==EH_LEFT || key==EH_RIGHT){
    s->selected=(s->selected+(key==EH_LEFT?3:1))%4;
    s->last_action=0;
    return 1;
  }
  if(key==EH_ENTER){s->last_action=s->selected+1;return 1;}
  return 0;
}
