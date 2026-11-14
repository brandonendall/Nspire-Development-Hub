#include "registry.h"
#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhRegistry r;eh7_registry_init(&r);
 assert(eh7_registry_parse(&r,"0=GBA|/documents/GBA/App.tns")==1);
 EhUniversalUI s;eh7_ui_init(&s,&r);
 eh7_ui_handle(&s,EH_KEY_ENTER);assert(s.request==1);
 EhUniversalUI m;eh7_ui_init(&m,&r);
 eh7_ui_handle(&m,EH_KEY_MENU);
 assert(m.page.page==EH_PAGE_DETAIL&&m.manager_item==0);
 eh7_ui_handle(&m,EH_KEY_ENTER);assert(m.assign_slot==1);
 EhUniversalUI n;eh7_ui_init(&n,&r);
 eh7_ui_handle(&n,EH_KEY_MENU);eh7_ui_handle(&n,EH_KEY_DOWN);
 eh7_ui_handle(&n,EH_KEY_ENTER);assert(n.renaming);
 eh7_ui_handle(&n,EH_KEY_ENTER);eh7_ui_handle(&n,EH_KEY_RIGHT);
 eh7_ui_handle(&n,EH_KEY_ENTER);
 assert(!strcmp(n.rename_input,"AB"));
 eh7_ui_handle(&n,EH_KEY_MENU);
 assert(n.edit_action==EH15_RENAME&&!n.renaming);
 EhUniversalUI x;eh7_ui_init(&x,&r);
 eh7_ui_handle(&x,EH_KEY_MENU);eh7_ui_handle(&x,EH_KEY_DOWN);
 eh7_ui_handle(&x,EH_KEY_DOWN);eh7_ui_handle(&x,EH_KEY_ENTER);
 assert(x.confirm_remove);
 eh7_ui_handle(&x,EH_KEY_ESC);assert(!x.confirm_remove&&!x.edit_action);
 eh7_ui_handle(&x,EH_KEY_ENTER);eh7_ui_handle(&x,EH_KEY_ENTER);
 assert(x.edit_action==EH15_REMOVE);
 EhUniversalUI y;eh7_ui_init(&y,&r);
 eh7_ui_handle(&y,EH_KEY_MENU);
 for(int i=0;i<3;i++)eh7_ui_handle(&y,EH_KEY_DOWN);
 eh7_ui_handle(&y,EH_KEY_ENTER);
 assert(y.page.page==EH_PAGE_HELP&&!y.subview);
 eh7_ui_handle(&y,EH_KEY_ESC);eh7_ui_handle(&y,EH_KEY_DOWN);
 eh7_ui_handle(&y,EH_KEY_ENTER);
 assert(y.page.page==EH_PAGE_HELP&&y.subview==1);
 puts("Run015 Home fast launch, manager, palette rename, safe remove tests passed");
 return 0;
}
