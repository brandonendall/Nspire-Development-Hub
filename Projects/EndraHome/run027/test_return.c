#include "session.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 EhRegistry apps;eh7_registry_init(&apps);
 eh7_registry_parse(&apps,"5=Tools|/documents/EndraHome_Test006.tns");
 EhUniversalUI s;eh7_ui_init(&s,&apps);
 for(int i=0;i<5;i++)eh7_ui_handle(&s,EH_KEY_RIGHT);
 eh7_ui_handle(&s,EH_KEY_ENTER);assert(s.request==6);
 eh11_resume_home(&s);
 assert(s.page.selected==5&&s.page.page==EH_PAGE_HOME);
 assert(!s.request&&!s.assign_slot&&!s.edit_action);
 eh7_ui_handle(&s,EH_KEY_MENU);assert(s.page.page==EH_PAGE_DETAIL);
 eh7_ui_handle(&s,EH_KEY_ESC);assert(s.page.page==EH_PAGE_HOME);
 eh7_ui_handle(&s,EH_KEY_ENTER);assert(s.request==6);
 eh11_resume_home(&s);eh7_ui_handle(&s,EH_KEY_ESC);
 assert(s.page.should_exit);
 puts("Run015 Return Home / no auto-relaunch / fast exit tests passed");
 return 0;
}
