#include "ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhUI7Launch s;eh7launch_init(&s);
 /* Test Tools launch and cancel */
 for(int i=0;i<5;i++)eh7launch_handle(&s,EH_KEY_RIGHT);
 assert(s.page.selected==EH7_TOOLS);
 eh7launch_handle(&s,EH_KEY_ENTER);
 assert(s.page.page==EH_PAGE_DETAIL);
 eh7launch_handle(&s,EH_KEY_ENTER);
 assert(s.confirm_target==EH7_TOOLS&&!s.request_target);
 eh7launch_handle(&s,EH_KEY_ESC);assert(!s.confirm_target);
 eh7launch_handle(&s,EH_KEY_ENTER);
 eh7launch_handle(&s,EH_KEY_ENTER);
 assert(s.request_target==EH7_TOOLS);
 assert(!eh7launch_handle(&s,EH_KEY_ENTER));
 /* Test ZIP launch and double-confirm */
 EhUI7Launch z;eh7launch_init(&z);
 eh7launch_handle(&z,EH_KEY_RIGHT);
 eh7launch_handle(&z,EH_KEY_ENTER);
 eh7launch_handle(&z,EH_KEY_ENTER);
 assert(z.confirm_target==EH7_ZIP && !z.request_target);
 eh7launch_handle(&z,EH_KEY_MENU);assert(!z.confirm_target);
 eh7launch_handle(&z,EH_KEY_ENTER);
 eh7launch_handle(&z,EH_KEY_ENTER);
 assert(z.request_target==EH7_ZIP);
 /* GBA and other tiles have no launch path */
 EhUI7Launch g;eh7launch_init(&g);
 eh7launch_handle(&g,EH_KEY_ENTER);
 eh7launch_handle(&g,EH_KEY_ENTER);
 assert(g.page.page==EH_PAGE_ABOUT && !g.request_target);
 /* Confirm path is /documents, relative to this launcher, not supplied by UI */
 char p[512];
 assert(eh7launch_sibling(p,sizeof(p),"/documents/EndraHome/Run007.tns",EH7_ZIP));
 assert(!strcmp(p,"/documents/EndraHome/EndraHome_ZIP007.tns"));
 assert(eh7launch_sibling(p,sizeof(p),"/documents/EndraHome/Run007.tns",EH7_TOOLS));
 assert(!strcmp(p,"/documents/EndraHome/EndraHome_Test006.tns"));
 assert(!eh7launch_sibling(p,sizeof(p),"/documents/../z.tns",EH7_ZIP));
 assert(!eh7launch_sibling(p,sizeof(p),"/root/Run007.tns",EH7_ZIP));
 assert(!eh7launch_sibling(p,sizeof(p),"/documents/Run007.txt",EH7_ZIP));
 assert(!eh7launch_sibling(p,10,"/documents/a.tns",EH7_ZIP));
 assert(!eh7launch_sibling(p,sizeof(p),"/documents/g.tns",0));
 puts("Run007 safe one-real-app gate tests passed");
 return 0;
}
