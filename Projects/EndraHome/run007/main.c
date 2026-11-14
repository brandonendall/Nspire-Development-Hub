/* EndraHome Run007: preserve Run006 release-and-execute sequence.
 * New behavior: allow ONLY a sibling ZIP app, plus the existing Tools probe.
 * Run003 GBA-native display+input is compiled directly, completely unchanged.
 */
#include "../gba-ui/platform.h"
#include "../run005/render.h"
#include "ui.h"
#include "overlay.h"
#include <libndls.h>
#include <stdio.h>
void __attribute__((weak)) _fini(void){}
static void draw(const EhUI7Launch *s){
 uint16_t *frame=eh_framebuffer();
 eh5_render(frame,&s->page);
 if(s->confirm_target)eh7launch_dialog(frame,s->confirm_target);
 eh_platform_flip();
}
int main(int argc,char **argv){
 EhUI7Launch s;
 eh7launch_init(&s);
 if(!eh_platform_start())return 1;
 draw(&s);
 while(!s.page.should_exit&&!s.request_target){
  EhKey key=eh_platform_readkey();
  if(eh7launch_handle(&s,key)&&!s.page.should_exit&&!s.request_target)draw(&s);
  if(!s.page.should_exit&&!s.request_target)eh_platform_sleep();
 }
 /* The important proven Run006 ordering: LCD/timer/OS restored before
  * calling fopen, messagebox, or nl_exec. Never re-enter our framebuffer. */
 eh_platform_stop();
 if(!s.request_target)return 0;
 char path[512];
 if(argc<1||!argv||!eh7launch_sibling(path,sizeof(path),argv[0],s.request_target)){
  show_msgbox("EndraHome 007","Could not locate a safe Documents app path.");
  return 1;
 }
 FILE *f=fopen(path,"rb");
 if(!f){
  if(s.request_target==EH7_ZIP)
   show_msgbox("EndraHome 007","ZIP app missing.\nCopy a known-working ZIP .tns\nas EndraHome_ZIP007.tns beside this launcher.");
  else
   show_msgbox("EndraHome 007","Tools test app is missing.\nCopy EndraHome_Test006.tns beside this launcher.");
  return 1;
 }
 fclose(f);
 msleep(180);
 int result=nl_exec(path,0,NULL);
 if(result<0)show_msgbox("EndraHome 007","Ndless rejected this app.\nNo other apps were launched.");
 refresh_osscr();
 return result<0?1:0;
}
