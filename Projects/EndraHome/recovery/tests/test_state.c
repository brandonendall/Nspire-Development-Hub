#include <assert.h>
#include <stdio.h>
#include "state.h"
int main(void){
  EhState s;
  eh_state_init(&s);
  assert(s.selected==0&&!s.help&&!s.quit);
  assert(eh_state_input(&s,EH_MENU));
  assert(s.help);
  assert(!eh_state_input(&s,EH_MENU)); /* held key must not repeat */
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_ENTER));
  assert(!s.help);
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_RIGHT));
  assert(s.selected==1);
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_LEFT));
  assert(s.selected==0);
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_LEFT));
  assert(s.selected==3);
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_ENTER));
  assert(s.last_action==4&&!s.quit);
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_MENU));
  eh_state_input(&s,EH_NONE);
  assert(eh_state_input(&s,EH_ESC));
  assert(s.quit);
  assert(!eh_state_input(&s,EH_ENTER));
  puts("Run002 Safe input-state tests passed");
  return 0;
}
