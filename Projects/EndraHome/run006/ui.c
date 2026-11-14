#include "ui.h"
#include <string.h>
#define TESTFILE "EndraHome_Test006.tns"
void eh6_init(EhUI6 *s){
 if(!s)return;
 eh7_init(&s->page);
 s->confirm=0;s->request_launch=0;
}
int eh6_handle(EhUI6 *s,EhKey key){
 if(!s||key==EH_KEY_NONE||s->request_launch||s->page.should_exit)return 0;
 if(s->confirm){
  if(key==EH_KEY_ESC||key==EH_KEY_MENU){s->confirm=0;return 1;}
  if(key==EH_KEY_ENTER){s->confirm=0;s->request_launch=1;return 1;}
  return 0;
 }
 /* Only the Tools tile (index 5) can request our disposable test target. */
 if(s->page.page==EH_PAGE_DETAIL&&s->page.selected==5&&key==EH_KEY_ENTER){
  s->confirm=1;return 1;
 }
 return eh7_dispatch(&s->page,key);
}
int eh6_sibling_path(char *dst,size_t size,const char *exe){
 if(!dst||!size)return 0;
 dst[0]=0;
 if(!exe||strncmp(exe,"/documents/",11)||strstr(exe,".."))return 0;
 const char *slash=strrchr(exe,'/');
 if(!slash||!slash[1])return 0;
 size_t n=(size_t)(slash-exe)+1;
 if(n+sizeof(TESTFILE)>size)return 0;
 memcpy(dst,exe,n);
 memcpy(dst+n,TESTFILE,sizeof(TESTFILE));
 return 1;
}
