#include "registry.h"
#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhRegistry apps;eh7_registry_init(&apps);
 assert(!strcmp(apps.slots[0].label,"EndraGBA"));
 assert(!apps.slots[0].path[0]);
 assert(eh7_valid_path("/documents/Apps/SomeApp.tns"));
 assert(!eh7_valid_path("/documents/../bad.tns"));
 assert(!eh7_valid_path("/root/not.tns"));
 assert(eh7_registry_parse(&apps,"0=Game|/documents/Apps/SomeApp.tns")==1);
 assert(eh7_registry_parse(&apps,"5=Tools|/documents/EndraHome_Test006.tns")==1);
 char cfg[300];
 assert(eh7_config_path(cfg,sizeof(cfg),"/documents/EndraHome_Run013_AppPicker.tns"));
 assert(!strcmp(cfg,"/documents/EndraHome_Apps.cfg.tns"));
 EhUniversalUI ui;eh7_ui_init(&ui,&apps);
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER)); /* configured: single Enter launches */
 assert(ui.request==1&&!ui.assign_slot);
 assert(!eh7_ui_handle(&ui,EH_KEY_ENTER));
 EhUniversalUI info;eh7_ui_init(&info,&apps);
 assert(eh7_ui_handle(&info,EH_KEY_MENU));
 assert(info.page.page==EH_PAGE_DETAIL);
 assert(eh7_ui_handle(&info,EH_KEY_ENTER)); /* Info Enter now ASSIGNS */
 assert(info.assign_slot==1&&!info.request);
 assert(!eh7_ui_handle(&info,EH_KEY_ENTER));
 EhUniversalUI help;eh7_ui_init(&help,&apps);
 eh7_ui_handle(&help,EH_KEY_MENU);
 eh7_ui_handle(&help,EH_KEY_MENU);
 assert(help.page.page==EH_PAGE_HELP);
 eh7_ui_handle(&help,EH_KEY_ESC);
 assert(help.page.page==EH_PAGE_DETAIL);
 EhUniversalUI missing;eh7_ui_init(&missing,&apps);
 eh7_ui_handle(&missing,EH_KEY_RIGHT);
 assert(eh7_ui_handle(&missing,EH_KEY_ENTER));
 assert(missing.assign_slot==2&&!missing.request);
 EhUniversalUI last;eh7_ui_init(&last,&apps);
 for(int i=0;i<6;i++)eh7_ui_handle(&last,EH_KEY_RIGHT);
 assert(last.page.selected==6);
 assert(eh7_ui_handle(&last,EH_KEY_ENTER));
 assert(last.request==0&&last.assign_slot==7);
 puts("Run013 immediate launch, Menu-Enter assign, unconfigured picker tests passed");
 return 0;
}
