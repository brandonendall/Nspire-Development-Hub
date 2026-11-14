#include "logic7.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 EhUI7 s;eh7_init(&s);
 assert(s.page==EH_PAGE_HOME&&s.selected==0);
 for(int i=0;i<7;i++){assert(s.selected==i);eh7_dispatch(&s,EH_KEY_RIGHT);}
 assert(s.selected==0);
 eh7_dispatch(&s,EH_KEY_LEFT);assert(s.selected==6);
 eh7_dispatch(&s,EH_KEY_ENTER);assert(s.page==EH_PAGE_DETAIL);
 eh7_dispatch(&s,EH_KEY_ENTER);assert(s.page==EH_PAGE_ABOUT);
 eh7_dispatch(&s,EH_KEY_ESC);assert(s.page==EH_PAGE_DETAIL);
 eh7_dispatch(&s,EH_KEY_MENU);assert(s.page==EH_PAGE_HELP);
 eh7_dispatch(&s,EH_KEY_MENU);assert(s.page==EH_PAGE_HOME);
 eh7_dispatch(&s,EH_KEY_ESC);assert(s.should_exit);
 puts("Run005 7-app navigation tests passed");return 0;
}
