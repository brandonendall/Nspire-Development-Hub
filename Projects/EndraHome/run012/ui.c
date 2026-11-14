/* Enter: immediate launch. Menu: app info. No confirmation screens. */
#include "ui.h"
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps){
 if(!s)return;
 eh7_init(&s->page);
 s->apps=apps;s->missing=0;s->request=0;
}
int eh7_ui_handle(EhUniversalUI *s,EhKey key){
 if(!s||key==EH_KEY_NONE||s->page.should_exit||s->request)return 0;
 if(s->missing){
  if(key==EH_KEY_ESC||key==EH_KEY_ENTER){s->missing=0;return 1;}
  if(key==EH_KEY_MENU){s->missing=0;s->page.page=EH_PAGE_DETAIL;return 1;}
  return 0;
 }
 if(key==EH_KEY_ENTER){
  if(s->page.page==EH_PAGE_HELP){s->page.page=EH_PAGE_HOME;return 1;}
  int i=s->page.selected;
  if(s->apps&&s->apps->slots[i].path[0])s->request=i+1;
  else s->missing=1;
  return 1;
 }
 if(key==EH_KEY_MENU){
  if(s->page.page==EH_PAGE_HOME)s->page.page=EH_PAGE_DETAIL;
  else if(s->page.page==EH_PAGE_DETAIL)s->page.page=EH_PAGE_HELP;
  else s->page.page=EH_PAGE_DETAIL;
  return 1;
 }
 if(key==EH_KEY_ESC){
  if(s->page.page==EH_PAGE_HOME)s->page.should_exit=1;
  else if(s->page.page==EH_PAGE_HELP)s->page.page=EH_PAGE_DETAIL;
  else s->page.page=EH_PAGE_HOME;
  return 1;
 }
 if(s->page.page==EH_PAGE_HOME)return eh7_dispatch(&s->page,key);
 return 0;
}
