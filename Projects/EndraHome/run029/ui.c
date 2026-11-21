/* Run016: physical QWERTY renaming; remove Run015's on-screen alphabet. */
#include "ui.h"
#include <string.h>
#include "desktop.h"
void eh7_ui_init(EhUniversalUI *s,const EhRegistry *apps){
 if(!s)return;
 eh7_init(&s->page);s->apps=apps;
 s->page.selected=eh_desktop_select(apps,-1,EH_KEY_NONE);
 s->missing=s->request=s->assign_slot=s->adding_app=s->edit_action=s->open_files=0;
 s->manager_item=s->subview=s->renaming=s->confirm_remove=0;
 s->preview=apps?apps->appearance:(EhAppearance){0};
 s->setting_row=0;s->native_status=0;
 s->notice=(EhToast){0};
 s->rename_input[0]=0;
}
static int save_label(EhUniversalUI *s){
 if(!s->rename_input[0]){
  eh_toast_set(&s->notice,"NAME REQUIRED - TYPE QWERTY");
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
  s->notice.text[0]=0;
  return 1;
 }
 /* Every character is one keypress, no on-screen palette needed. */
 if(c<32||c>126||c=='|')return 0;
 size_t len=strlen(s->rename_input);
 if(len>=EH7_LABEL_CAP-1){
  eh_toast_set(&s->notice,"MAX 15 CHARACTERS");
  return 1;
 }
 s->rename_input[len]=(char)c;
 s->rename_input[len+1]=0;
 s->notice.text[0]=0;
 return 1;
}
int eh7_ui_handle(EhUniversalUI *s,EhKey k){
 if(!s||k==EH_KEY_NONE||s->page.should_exit||
    s->request||s->assign_slot||s->edit_action||s->open_files)return 0;
 if(s->renaming){
  if(k==EH_KEY_ESC){s->renaming=0;s->notice.text[0]=0;return 1;}
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
   if(i==EH_FILES_SLOT)s->open_files=1;
   else if(s->apps&&s->apps->slots[i].path[0])s->request=i+1;
   else s->page.selected=eh_desktop_select(s->apps,i,EH_KEY_NONE);
   return 1;
  }
  if(k==EH_KEY_LEFT||k==EH_KEY_UP||k==EH_KEY_RIGHT||k==EH_KEY_DOWN){
   s->page.selected=eh_desktop_select(s->apps,s->page.selected,k);
   return 1;
  }
  return 0;
 }
 if(s->page.page==EH_PAGE_DETAIL){
  if(k==EH_KEY_ESC||k==EH_KEY_MENU){s->page.selected=eh_desktop_select(s->apps,s->page.selected,EH_KEY_NONE);s->page.page=EH_PAGE_HOME;return 1;}
  if(k==EH_KEY_LEFT||k==EH_KEY_RIGHT){
   s->page.selected=(s->page.selected+EH7_SLOT_COUNT+(k==EH_KEY_RIGHT?1:-1))%EH7_SLOT_COUNT;
   return 1;
  }
  if(k==EH_KEY_UP){if(s->manager_item>0)s->manager_item--;return 1;}
  if(k==EH_KEY_DOWN){if(s->manager_item<EH15_MENU_COUNT-1)s->manager_item++;return 1;}
  if(k==EH_KEY_ENTER){
   switch(s->manager_item){
    case EH_MANAGER_ADD:
     if(s->apps){
      for(int i=0;i<EH7_SLOT_COUNT;i++){
       if(i!=EH_FILES_SLOT&&!s->apps->slots[i].path[0]){
        s->assign_slot=i+1;s->adding_app=1;break;
       }
      }
     }
     if(!s->assign_slot)eh_toast_set(&s->notice,"APP SLOTS FULL - FREE A SLOT FIRST");
     break;
    case EH_MANAGER_REASSIGN:
     if(s->page.selected!=EH_FILES_SLOT&&s->apps&&s->apps->slots[s->page.selected].path[0]){
      s->assign_slot=s->page.selected+1;s->adding_app=0;
     }else eh_toast_set(&s->notice,"USE ADD APP OR SELECT A LINKED APP");
     break;
    case EH_MANAGER_RENAME:
     if(s->page.selected!=EH_FILES_SLOT){s->renaming=1;s->rename_input[0]=0;}
     break;
    case EH_MANAGER_REMOVE:s->confirm_remove=1;break;
    case EH_MANAGER_INFO:s->subview=0;s->page.page=EH_PAGE_HELP;break;
    case EH_MANAGER_APPEARANCE:s->subview=1;s->preview=s->apps?s->apps->appearance:(EhAppearance){0};s->setting_row=EH_LOOK_THEME;s->page.page=EH_PAGE_HELP;break;
    case EH_MANAGER_FILES:s->open_files=1;break;
    case EH_MANAGER_TI:s->subview=2;s->native_status=0;s->page.page=EH_PAGE_HELP;break;
   }
   return 1;
  }
 }
 if(s->page.page==EH_PAGE_HELP&&s->subview==2){
  if(k==EH_KEY_ENTER){s->edit_action=EH15_NATIVE_CHECK;return 1;}
  if(k==EH_KEY_ESC||k==EH_KEY_MENU){s->subview=0;s->page.page=EH_PAGE_DETAIL;return 1;}
  return 0;
 }
 if(s->page.page==EH_PAGE_HELP&&s->subview==1){
  if(k==EH_KEY_UP || k==EH_KEY_DOWN){
   if(k==EH_KEY_UP && s->setting_row>0) --s->setting_row;
   if(k==EH_KEY_DOWN && s->setting_row<EH_LOOK_DEFAULTS) ++s->setting_row;
   return 1;
  }
  if((k==EH_KEY_LEFT||k==EH_KEY_RIGHT) && s->setting_row<EH_LOOK_DEFAULTS){
   unsigned *values[]={&s->preview.theme,&s->preview.folder,&s->preview.font,
                       &s->preview.wallpaper,&s->preview.icon};
   const unsigned counts[]={EH_THEME_COUNT,EH_COLOR_COUNT,EH_COLOR_COUNT,
                            EH_WALLPAPER_COUNT,EH_ICON_COUNT};
   unsigned n=counts[s->setting_row], *value=values[s->setting_row];
   *value=(*value+(k==EH_KEY_RIGHT?1:n-1))%n;
   return 1;
  }
  if(k==EH_KEY_ENTER){
   if(s->setting_row==EH_LOOK_DEFAULTS){
    s->preview=(EhAppearance){0};
    s->setting_row=EH_LOOK_THEME;
   }else{
    s->edit_action=EH15_THEME;
   }
   return 1;
  }
  if(k==EH_KEY_ESC||k==EH_KEY_MENU){
   s->preview=s->apps?s->apps->appearance:(EhAppearance){0};
   s->page.page=EH_PAGE_DETAIL;
   return 1;
  }
  return 0;
 }
 if(s->page.page==EH_PAGE_HELP&&(k==EH_KEY_ESC||k==EH_KEY_MENU||k==EH_KEY_ENTER)){
  s->page.page=EH_PAGE_DETAIL;return 1;
 }
 return 0;
}
