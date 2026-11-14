#include "session.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 EhRegistry apps;eh7_registry_init(&apps);
 assert(eh7_registry_parse(&apps,"5=Tools|/documents/EndraHome_Test006.tns")==1);
 EhUniversalUI ui;eh7_ui_init(&ui,&apps);
 for(int i=0;i<5;i++)eh7_ui_handle(&ui,EH_KEY_RIGHT);
 assert(ui.page.selected==5);
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER));
 assert(ui.request==6);
 eh11_resume_home(&ui);
 assert(ui.apps==&apps&&ui.page.selected==5);
 assert(ui.page.page==EH_PAGE_HOME);
 assert(!ui.request&&!ui.missing&&!ui.page.should_exit);
 eh7_ui_handle(&ui,EH_KEY_MENU);
 assert(ui.page.page==EH_PAGE_DETAIL);
 eh7_ui_handle(&ui,EH_KEY_ESC);
 assert(ui.page.page==EH_PAGE_HOME);
 eh7_ui_handle(&ui,EH_KEY_ENTER);
 assert(ui.request==6);
 eh11_resume_home(&ui);
 assert(!ui.request);
 eh7_ui_handle(&ui,EH_KEY_ESC);
 assert(ui.page.should_exit);
 eh11_resume_home(NULL);
 puts("Run012 return-home, no repeat launch, ESC tests passed");
 return 0;
}
