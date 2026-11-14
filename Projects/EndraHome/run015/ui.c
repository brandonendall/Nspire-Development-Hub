#include "ui.h"
#include <string.h>
const char eh15_palette[41]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_ .";
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps){
 if(!s)return;
 eh7_init(&s->page);s->apps=apps;
 s->missing=s->request=s->assign_slot=s->edit_action=0;
 s->manager_item=s->subview=s->renaming=s->confirm_remove=s->rename_cursor=0;
 s->rename_input[0]=s->notice[0]=0;
}
static int save_label(EhUniversalUI *s){
 if(!s->rename_input[0]){strcpy(s->notice,"NAME REQUIRED");return 1;}
 s->renaming=0;s->edit_action=EH15_RENAME;return 1;
}
static int rename_input(EhUniversalUI *s,EhKey k){
 int i=s->rename_cursor,j=i;
 if(k==EH_KEY_ESC){s->renaming=0;return 1;}
 if(k==EH_KEY_MENU)return save_label(s);
 if(k==EH_KEY_LEFT)j=i%7?i-1:i+6;
 else if(k==EH_KEY_RIGHT)j=i%7==6?i-6:i+1;
 else if(k==EH_KEY_UP)j=(i+35)%42;
 else if(k==EH_KEY_DOWN)j=(i+7)%42;
 if(j!=i){s->rename_cursor=j;return 1;}
 if(k==EH_KEY_ENTER){
  size_t n=strlen(s->rename_input);
  if(i==41)return save_label(s);
  if(i==40){if(n)s->rename_input[n-1]=0;}
  else if(n<EH7_LABEL_CAP-1){
   s->rename_input[n]=eh15_palette[i];s->rename_input[n+1]=0;
  }else strcpy(s->notice,"MAXIMUM 15 CHARACTERS");
  return 1;
 }
 return 0;
}
int eh7_ui_handle(EhUniversalUI *s,EhKey k){
 if(!s||k==EH_KEY_NONE||s->page.should_exit||s->request||s->assign_slot||s->edit_action)return 0;
 s->notice[0]=0;
 if(s->renaming)return rename_input(s,k);
 if(s->confirm_remove){
  if(k==EH_KEY_ENTER){s->confirm_remove=0;s->edit_action=EH15_REMOVE;return 1;}
  if(k==EH_KEY_ESC||k==EH_KEY_MENU){s->confirm_remove=0;return 1;}
  return 0;
 }
 if(s->missing){
  if(k==EH_KEY_ESC||k==EH_KEY_ENTER){s->missing=0;return 1;}
  if(k==EH_KEY_MENU){s->missing=0;s->page.page=EH_PAGE_DETAIL;return 1;}
  return 0;
 }
 if(s->page.page==EH_PAGE_HOME){
  if(k==EH_KEY_MENU){s->page.page=EH_PAGE_DETAIL;s->manager_item=0;return 1;}
  if(k==EH_KEY_ESC){s->page.should_exit=1;return 1;}
  if(k==EH_KEY_ENTER){
   int slot=s->page.selected;
   if(s->apps&&s->apps->slots[slot].path[0])s->request=slot+1;
   else s->assign_slot=slot+1;
   return 1;
  }
  return eh7_dispatch(&s->page,k);
 }
 if(s->page.page==EH_PAGE_DETAIL){
  if(k==EH_KEY_ESC||k==EH_KEY_MENU){s->page.page=EH_PAGE_HOME;return 1;}
  if(k==EH_KEY_UP){if(s->manager_item>0)s->manager_item--;return 1;}
  if(k==EH_KEY_DOWN){if(s->manager_item<EH15_MENU_COUNT-1)s->manager_item++;return 1;}
  if(k==EH_KEY_ENTER){
   switch(s->manager_item){
    case 0:s->assign_slot=s->page.selected+1;break;
    case 1:s->renaming=1;s->rename_cursor=0;s->rename_input[0]=0;break;
    case 2:s->confirm_remove=1;break;
    case 3:s->subview=0;s->page.page=EH_PAGE_HELP;break;
    case 4:s->subview=1;s->page.page=EH_PAGE_HELP;break;
   }
   return 1;
  }
 }
 if(s->page.page==EH_PAGE_HELP&&(k==EH_KEY_ESC||k==EH_KEY_MENU||k==EH_KEY_ENTER)){
  s->page.page=EH_PAGE_DETAIL;return 1;
 }
 return 0;
}
