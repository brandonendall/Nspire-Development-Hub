#include <assert.h>
#include <stdio.h>
#include "logic.h"
int main(void){
  EhUI s; eh_ui_init(&s);
  assert(s.page==EH_PAGE_HOME&&s.selected==0&&!s.should_exit);
  assert(eh_ui_dispatch(&s,EH_KEY_RIGHT));
  assert(s.selected==1);
  assert(eh_ui_dispatch(&s,EH_KEY_DOWN));
  assert(s.selected==2);
  assert(eh_ui_dispatch(&s,EH_KEY_ENTER));
  assert(s.page==EH_PAGE_DETAIL&&!s.should_exit);
  assert(!eh_ui_dispatch(&s,EH_KEY_LEFT));
  assert(eh_ui_dispatch(&s,EH_KEY_ENTER));
  assert(s.page==EH_PAGE_ABOUT);
  assert(eh_ui_dispatch(&s,EH_KEY_ESC));
  assert(s.page==EH_PAGE_DETAIL);
  assert(eh_ui_dispatch(&s,EH_KEY_MENU));
  assert(s.page==EH_PAGE_HELP);
  assert(eh_ui_dispatch(&s,EH_KEY_ENTER));
  assert(s.page==EH_PAGE_HOME);
  assert(eh_ui_dispatch(&s,EH_KEY_LEFT));
  assert(s.selected==1);
  assert(eh_ui_dispatch(&s,EH_KEY_ESC));
  assert(s.should_exit);
  assert(!eh_ui_dispatch(&s,EH_KEY_ENTER));
  puts("GBA-style Run003 navigation state tests passed");
  return 0;
}
