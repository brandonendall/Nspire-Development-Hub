#include "logic.h"
/* Pure input-to-page reducer. Does not touch Nspire hardware. */
void eh_ui_init(EhUI *s) {
  s->page=EH_PAGE_HOME;
  s->selected=0;
  s->should_exit=0;
  s->handled=0;
}
int eh_ui_dispatch(EhUI *s, EhKey k) {
  if(!s || s->should_exit || k==EH_KEY_NONE)return 0;
  s->handled++;
  if(k==EH_KEY_ESC){
    if(s->page==EH_PAGE_HOME)s->should_exit=1;
    else if(s->page==EH_PAGE_ABOUT)s->page=EH_PAGE_DETAIL;
    else s->page=EH_PAGE_HOME;
    return 1;
  }
  if(k==EH_KEY_MENU){
    s->page=(s->page==EH_PAGE_HELP)?EH_PAGE_HOME:EH_PAGE_HELP;
    return 1;
  }
  if(k==EH_KEY_ENTER){
    if(s->page==EH_PAGE_HOME)s->page=EH_PAGE_DETAIL;
    else if(s->page==EH_PAGE_DETAIL)s->page=EH_PAGE_ABOUT;
    else if(s->page==EH_PAGE_ABOUT)s->page=EH_PAGE_DETAIL;
    else s->page=EH_PAGE_HOME;
    return 1;
  }
  if(s->page!=EH_PAGE_HOME)return 0;
  if(k==EH_KEY_LEFT||k==EH_KEY_UP){
    s->selected=(s->selected+3)%4;return 1;
  }
  if(k==EH_KEY_RIGHT||k==EH_KEY_DOWN){
    s->selected=(s->selected+1)%4;return 1;
  }
  return 0;
}
