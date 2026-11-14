#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhUI6 s;eh6_init(&s);
 for(int i=0;i<5;i++)eh6_handle(&s,EH_KEY_RIGHT);
 assert(s.page.selected==5);
 eh6_handle(&s,EH_KEY_ENTER);
 assert(s.page.page==EH_PAGE_DETAIL&&!s.confirm);
 eh6_handle(&s,EH_KEY_ENTER);assert(s.confirm);
 eh6_handle(&s,EH_KEY_ESC);assert(!s.confirm&&!s.request_launch);
 eh6_handle(&s,EH_KEY_ENTER);assert(s.confirm);
 eh6_handle(&s,EH_KEY_ENTER);assert(s.request_launch);
 assert(!eh6_handle(&s,EH_KEY_ENTER));
 EhUI6 t;eh6_init(&t);
 eh6_handle(&t,EH_KEY_ENTER);
 eh6_handle(&t,EH_KEY_ENTER);
 assert(t.page.page==EH_PAGE_ABOUT&&!t.confirm);
 char p[256];
 assert(eh6_sibling_path(p,sizeof(p),"/documents/EndraHome/Run006.tns"));
 assert(!strcmp(p,"/documents/EndraHome/EndraHome_Test006.tns"));
 assert(!eh6_sibling_path(p,sizeof(p),"/etc/test.tns"));
 assert(!eh6_sibling_path(p,sizeof(p),"/documents/../test.tns"));
 assert(!eh6_sibling_path(p,8,"/documents/a.tns"));
 puts("Run006 Tools probe gating/path tests passed");
 return 0;
}
