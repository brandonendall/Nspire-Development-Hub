#include "session.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 EhRegistry apps;eh7_registry_init(&apps);
 assert(eh7_registry_parse(&apps,"5=Tools|/documents/EndraHome_Test006.tns")==1);
 EhUniversalUI ui;eh7_ui_init(&ui,&apps);
 for(int i=0;i<5;i++)eh7_ui_handle(&ui,EH_KEY_RIGHT);
 assert(ui.page.selected==5);
 eh7_ui_handle(&ui,EH_KEY_ENTER);
 assert(ui.request==6);
 eh11_resume_home(&ui);
 assert(ui.apps==&apps&&ui.page.selected==5);
 assert(ui.page.page==EH_PAGE_HOME);
 assert(!ui.request&&!ui.assign_slot&&!ui.page.should_exit);
 eh7_ui_handle(&ui,EH_KEY_MENU);
 eh7_ui_handle(&ui,EH_KEY_ENTER);
 assert(ui.assign_slot==6&&!ui.request);
 /* Manual picker cancel clears pending assign, leaves selected tile. */
 ui.assign_slot=0;
 eh7_ui_handle(&ui,EH_KEY_ESC);
 assert(ui.page.page==EH_PAGE_HOME);
 eh7_ui_handle(&ui,EH_KEY_ENTER);
 assert(ui.request==6);
 eh11_resume_home(&ui);
 eh7_ui_handle(&ui,EH_KEY_ESC);
 assert(ui.page.should_exit);
 puts("Run013 return-home, no relaunch, Menu assign, Esc tests passed");
 return 0;
}
