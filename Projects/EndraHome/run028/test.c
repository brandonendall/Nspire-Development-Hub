#include "registry.h"
#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhRegistry r;eh7_registry_init(&r);
 assert(eh7_registry_parse(&r,"0=GBA|/documents/GBA/App.tns")==1);
 EhUniversalUI s;eh7_ui_init(&s,&r);
 eh7_ui_handle(&s,EH_KEY_ENTER);
 assert(s.request==1); /* Home Enter still instant launch */
 EhUniversalUI m;eh7_ui_init(&m,&r);
 eh7_ui_handle(&m,EH_KEY_MENU);
 assert(m.page.page==EH_PAGE_DETAIL&&m.manager_item==0);
 eh7_ui_handle(&m,EH_KEY_ENTER);assert(m.assign_slot==1);
 EhUniversalUI n;eh7_ui_init(&n,&r);
 eh7_ui_handle(&n,EH_KEY_MENU);
 eh7_ui_handle(&n,EH_KEY_DOWN);
 eh7_ui_handle(&n,EH_KEY_ENTER);
 assert(n.renaming);
 assert(!eh16_ui_type(NULL,'A'));
 assert(eh16_ui_type(&n,'E'));
 assert(eh16_ui_type(&n,'n'));
 assert(eh16_ui_type(&n,'d'));
 assert(eh16_ui_type(&n,'r'));
 assert(eh16_ui_type(&n,'a'));
 assert(!strcmp(n.rename_input,"Endra"));
 assert(eh16_ui_type(&n,8)); /* DEL/backspace */
 assert(!strcmp(n.rename_input,"Endr"));
 assert(eh16_ui_type(&n,'a'));
 assert(!eh16_ui_type(&n,'|')); /* Config delimiter prohibited */
 assert(!strcmp(n.rename_input,"Endra"));
 assert(eh7_ui_handle(&n,EH_KEY_ENTER)); /* Enter saves */
 assert(n.edit_action==EH15_RENAME&&!n.renaming);
 assert(!eh16_ui_type(&n,'Z'));
 EhUniversalUI empty;eh7_ui_init(&empty,&r);
 eh7_ui_handle(&empty,EH_KEY_MENU);
 eh7_ui_handle(&empty,EH_KEY_DOWN);
 eh7_ui_handle(&empty,EH_KEY_ENTER);
 assert(empty.renaming);
 eh7_ui_handle(&empty,EH_KEY_ENTER);
 assert(empty.renaming&&!empty.edit_action);
 assert(!strcmp(empty.notice,"NAME REQUIRED - TYPE QWERTY"));
 eh7_ui_handle(&empty,EH_KEY_ESC);
 assert(!empty.renaming);
 EhUniversalUI max;eh7_ui_init(&max,&r);
 eh7_ui_handle(&max,EH_KEY_MENU);
 eh7_ui_handle(&max,EH_KEY_DOWN);
 eh7_ui_handle(&max,EH_KEY_ENTER);
 for(int i=0;i<EH7_LABEL_CAP-1;i++)assert(eh16_ui_type(&max,'a'));
 assert(!strcmp(max.rename_input,"aaaaaaaaaaaaaaa"));
 assert(eh16_ui_type(&max,'z'));
 assert(strlen(max.rename_input)==EH7_LABEL_CAP-1);
 assert(eh16_ui_type(&max,127));
 assert(strlen(max.rename_input)==EH7_LABEL_CAP-2);
 EhUniversalUI rm;eh7_ui_init(&rm,&r);
 eh7_ui_handle(&rm,EH_KEY_MENU);
 eh7_ui_handle(&rm,EH_KEY_DOWN);
 eh7_ui_handle(&rm,EH_KEY_DOWN);
 eh7_ui_handle(&rm,EH_KEY_ENTER);
 assert(rm.confirm_remove);
 eh7_ui_handle(&rm,EH_KEY_ESC);
 assert(!rm.confirm_remove&&!rm.edit_action);
 eh7_ui_handle(&rm,EH_KEY_ENTER);
 eh7_ui_handle(&rm,EH_KEY_ENTER);
 assert(rm.edit_action==EH15_REMOVE);
 EhUniversalUI info;eh7_ui_init(&info,&r);
 eh7_ui_handle(&info,EH_KEY_MENU);
 for(int i=0;i<3;i++)eh7_ui_handle(&info,EH_KEY_DOWN);
 eh7_ui_handle(&info,EH_KEY_ENTER);
 assert(info.page.page==EH_PAGE_HELP&&!info.subview);
 eh7_ui_handle(&info,EH_KEY_ESC);
 eh7_ui_handle(&info,EH_KEY_DOWN);
 eh7_ui_handle(&info,EH_KEY_ENTER);
 assert(info.page.page==EH_PAGE_HELP&&info.subview);
 /* Dynamic home shows assigned slots only; Files remains in MENU. */
 EhUniversalUI fm;eh7_ui_init(&fm,&r);
 assert(fm.page.selected==0);
 for(int i=0;i<3;i++)eh7_ui_handle(&fm,EH_KEY_RIGHT);
 assert(fm.page.selected==0);
 eh7_ui_handle(&fm,EH_KEY_MENU);
 for(int i=0;i<5;i++)eh7_ui_handle(&fm,EH_KEY_DOWN);
 assert(eh7_ui_handle(&fm,EH_KEY_ENTER));
 assert(fm.open_files&&!fm.request&&!fm.assign_slot);
 EhRegistry ext_apps;eh7_registry_init(&ext_apps);
 assert(eh7_registry_parse(&ext_apps,"3=Files|/documents/OtherBrowser.tns")==1);
 EhUniversalUI ext;eh7_ui_init(&ext,&ext_apps);
 assert(ext.page.selected==3);
 eh7_ui_handle(&ext,EH_KEY_ENTER);
 assert(ext.request==4&&!ext.open_files);
 puts("Run028 dynamic assigned-app navigation, QWERTY, manager and Files passed");
 return 0;
}
