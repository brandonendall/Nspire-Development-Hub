#include "ui.h"
#include <string.h>
static const char *target_name(int target){
 if(target==EH7_ZIP)return "EndraHome_ZIP007.tns";
 if(target==EH7_TOOLS)return "EndraHome_Test006.tns";
 return 0;
}
void eh7launch_init(EhUI7Launch *s){
 if(!s)return;
 eh7_init(&s->page);
 s->confirm_target=EH7_NONE;
 s->request_target=EH7_NONE;
}
int eh7launch_handle(EhUI7Launch *s,EhKey key){
 if(!s||s->page.should_exit||s->request_target||key==EH_KEY_NONE)return 0;
 if(s->confirm_target){
  if(key==EH_KEY_ESC||key==EH_KEY_MENU){s->confirm_target=EH7_NONE;return 1;}
  if(key==EH_KEY_ENTER){s->request_target=s->confirm_target;s->confirm_target=EH7_NONE;return 1;}
  return 0;
 }
 /* Keep every other app in preview mode. Only ZIP and Tools can launch. */
 int slot=s->page.selected;
 if(s->page.page==EH_PAGE_DETAIL&&key==EH_KEY_ENTER&&
    (slot==EH7_ZIP||slot==EH7_TOOLS)){
   s->confirm_target=slot;return 1;
 }
 return eh7_dispatch(&s->page,key);
}
int eh7launch_sibling(char *dst,size_t cap,const char *from,int target){
 const char *file=target_name(target);
 if(!dst||!cap)return 0;
 dst[0]=0;
 if(!file||!from||strncmp(from,"/documents/",11))return 0;
 if(strstr(from,"..")||strstr(from,"//")||strstr(from,"/./"))return 0;
 const char *slash=strrchr(from,'/');
 if(!slash||!slash[1])return 0;
 size_t from_len=strlen(from);
 if(from_len<15||strcmp(from+from_len-4,".tns"))return 0;
 for(const char *p=from;*p;p++)
   if((unsigned char)*p<32||(unsigned char)*p>126||*p=='|')return 0;
 size_t dirname=(size_t)(slash-from)+1;
 size_t length=strlen(file)+1;
 if(dirname+length>cap)return 0;
 memcpy(dst,from,dirname);
 memcpy(dst+dirname,file,length);
 return 1;
}
