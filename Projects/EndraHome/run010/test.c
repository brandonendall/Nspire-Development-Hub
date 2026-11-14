#include "registry.h"
#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhRegistry apps;eh7_registry_init(&apps);
 assert(!strcmp(apps.slots[0].label,"EndraGBA"));
 assert(!apps.slots[0].path[0]);
 assert(eh7_valid_path("/documents/Emulators/UltraGBA_Run007.tns"));
 assert(!eh7_valid_path("/documents/../bad.tns"));
 assert(!eh7_valid_path("/documents//bad.tns"));
 assert(!eh7_valid_path("/root/not.tns"));
 assert(!eh7_valid_path("/documents/text.txt"));
 assert(!eh7_registry_parse(&apps,"# hello\n"));
 assert(eh7_registry_parse(&apps,"0=Game|/documents/Apps/SomeApp.tns\n")==1);
 assert(!strcmp(apps.slots[0].label,"Game"));
 assert(!strcmp(apps.slots[0].path,"/documents/Apps/SomeApp.tns"));
 assert(eh7_registry_parse(&apps,"6=Faves|/documents/Nspire/faves.tns\r\n")==1);
 assert(eh7_registry_parse(&apps,"1=ZIP|")==1);
 assert(!apps.slots[1].path[0]);
 assert(eh7_registry_parse(&apps,"4=X|/documents/../unsafe.tns")==-1);
 assert(!apps.slots[4].path[0]);
 assert(eh7_registry_parse(&apps,"8=X|/documents/Apps/a.tns")==-1);
 char cfg[300];
 assert(eh7_config_path(cfg,sizeof(cfg),"/documents/Endra/EndraHome_Run010_NoRefresh.tns"));
 assert(!strcmp(cfg,"/documents/Endra/EndraHome_Apps.cfg.tns"));
 assert(!strstr(cfg,"Apps010"));
 assert(!strstr(cfg,"Apps007"));
 assert(!eh7_config_path(cfg,10,"/documents/Endra/Start.tns"));
 EhUniversalUI ui;eh7_ui_init(&ui,&apps);
 /* Home Enter -> details, then Enter -> confirm, ESC cancel */
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER));
 assert(ui.page.page==EH_PAGE_DETAIL);
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER));
 assert(ui.confirm==1&&!ui.request);
 assert(eh7_ui_handle(&ui,EH_KEY_ESC));
 assert(!ui.confirm&&!ui.request);
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER));
 assert(ui.confirm==1);
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER));
 assert(ui.request==1);
 assert(!eh7_ui_handle(&ui,EH_KEY_ENTER));
 /* Unconfigured slots never launch */
 EhUniversalUI missing;eh7_ui_init(&missing,&apps);
 eh7_ui_handle(&missing,EH_KEY_RIGHT);
 eh7_ui_handle(&missing,EH_KEY_ENTER);
 eh7_ui_handle(&missing,EH_KEY_ENTER);
 assert(missing.missing&&!missing.request);
 eh7_ui_handle(&missing,EH_KEY_ENTER);
 assert(!missing.request&&!missing.missing);
 /* All seven slots are available, not hard-wired by app type */
 EhUniversalUI last;eh7_ui_init(&last,&apps);
 for(int i=0;i<6;i++)eh7_ui_handle(&last,EH_KEY_RIGHT);
 assert(last.page.selected==6);
 eh7_ui_handle(&last,EH_KEY_ENTER);
 eh7_ui_handle(&last,EH_KEY_ENTER);
 assert(last.confirm==7);
 eh7_ui_handle(&last,EH_KEY_ENTER);
 assert(last.request==7);
 puts("Run010 permanent-config registry and launch-state tests passed");
 return 0;
}
