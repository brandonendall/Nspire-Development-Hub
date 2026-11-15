/* Run016: physical QWERTY renaming; remove Run015's on-screen alphabet. */
#include "ui.h"
#include <string.h>
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps){
 if(!s)return;
 eh7_init(&s->page);s->apps=apps;
 s->missing=s->request=s->assign_slot=s->edit_action=s->open_files=0;
 s->manager_item=s->subview=s->renaming=s->confirm_remove=0;
 s->rename_input[0]=s->notice[0]=0;
}
static int save_label(EhUniversalUI *s){
 if(!s->rename_input[0]){
  strcpy(s->notice,"NAME REQUIRED - TYPE QWERTY");
  return 1;
 }
 s->renaming=0;
 s->edit_action=EH15_RENAME;
 return 1;
}
int eh16_ui_type(EhUniversalUI *s,int c){
 if(!s||!s->renaming||s->request||s->edit_action)return 0;
 if(c==8||c==127){
  size_t len=strlen(s->rename_input);
  if(len)s->rename_input[len-1]=0;
  s->notice[0]=0;
  return 1;
 }
 /* Every character is one keypress, no on-screen palette needed. */
 if(c<32||c>126||c=='|')return 0;
 size_t len=strlen(s->rename_input);
 if(len>=EH7_LABEL_CAP-1){
  strcpy(s->notice,"MAX 15 CHARACTERS");
  return 1;
 }
 s->rename_input[len]=(char)c;
 s->rename_input[len+1]=0;
 s->notice[0]=0;
 return 1;
}
int eh7_ui_handle(EhUniversalUI *s,EhKey k){
 if(!s||k==EH_KEY_NONE||s->page.should_exit||
    s->request||s->assign_slot||s->edit_action||s->open_files)return 0;
 /* Keep errors visible while renaming; don't clear them on unrelated keys. */
 if(!s->renaming)s->notice[0]=0;
 if(s->renaming){
  if(k==EH_KEY_ESC){s->renaming=0;s->notice[0]=0;return 1;}
  if(k==EH_KEY_ENTER)return save_label(s);
  return 0;
 }
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
   int i=s->page.selected;
   if(s->apps&&s->apps->slots[i].path[0])s->request=i+1;
   else if(i==3)s->open_files=1;
   else s->assign_slot=i+1;
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
    case 1:s->renaming=1;s->rename_input[0]=0;break;
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
