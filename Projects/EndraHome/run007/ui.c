#include "ui.h"
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps){
 if(!s)return;
 eh7_init(&s->page);
 s->apps=apps;
 s->confirm=0;s->missing=0;s->request=0;
}
int eh7_ui_handle(EhUniversalUI *s,EhKey key){
 if(!s||key==EH_KEY_NONE||s->page.should_exit||s->request)return 0;
 if(s->confirm||s->missing){
  if(key==EH_KEY_ESC||key==EH_KEY_MENU){
   s->confirm=0;s->missing=0;return 1;
  }
  if(key==EH_KEY_ENTER){
   if(s->confirm){s->request=s->confirm;s->confirm=0;}
   else s->missing=0;
   return 1;
  }
  return 0;
 }
 if(key==EH_KEY_ENTER&&s->page.page==EH_PAGE_DETAIL){
  int i=s->page.selected;
  if(s->apps&&s->apps->slots[i].path[0])s->confirm=i+1;
  else s->missing=1;
  return 1;
 }
 return eh7_dispatch(&s->page,key);
}
