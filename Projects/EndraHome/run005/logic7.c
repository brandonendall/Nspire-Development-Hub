#include "logic7.h"
void eh7_init(EhUI7 *s){s->page=EH_PAGE_HOME;s->selected=0;s->should_exit=0;}
int eh7_dispatch(EhUI7 *s,EhKey k){
 if(!s||s->should_exit||k==EH_KEY_NONE)return 0;
 if(k==EH_KEY_ESC){
  if(s->page==EH_PAGE_HOME)s->should_exit=1;
  else if(s->page==EH_PAGE_ABOUT)s->page=EH_PAGE_DETAIL;
  else s->page=EH_PAGE_HOME;
  return 1;
 }
 if(k==EH_KEY_MENU){s->page=(s->page==EH_PAGE_HELP)?EH_PAGE_HOME:EH_PAGE_HELP;return 1;}
 if(k==EH_KEY_ENTER){
  if(s->page==EH_PAGE_HOME)s->page=EH_PAGE_DETAIL;
  else if(s->page==EH_PAGE_DETAIL)s->page=EH_PAGE_ABOUT;
  else if(s->page==EH_PAGE_ABOUT)s->page=EH_PAGE_DETAIL;
  else s->page=EH_PAGE_HOME;
  return 1;
 }
 if(s->page!=EH_PAGE_HOME)return 0;
 if(k==EH_KEY_LEFT||k==EH_KEY_UP){s->selected=(s->selected+EH_DOCK_COUNT-1)%EH_DOCK_COUNT;return 1;}
 if(k==EH_KEY_RIGHT||k==EH_KEY_DOWN){s->selected=(s->selected+1)%EH_DOCK_COUNT;return 1;}
 return 0;
}
