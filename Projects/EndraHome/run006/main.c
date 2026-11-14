/* GBA-derived Run003 platform stays unchanged. Run006 adds a single
 * confirmed child-program probe, not an emulator launcher. */
#include "../gba-ui/platform.h"
#include "../run005/render.h"
#include "ui.h"
#include "overlay.h"
#include <libndls.h>
#include <stdio.h>
void __attribute__((weak)) _fini(void){}
static void paint(const EhUI6 *s){
 uint16_t *fb=eh_framebuffer();
 eh5_render(fb,&s->page);
 if(s->confirm)eh6_confirm_overlay(fb);
 eh_platform_flip();
}
int main(int argc,char **argv){
 EhUI6 s;eh6_init(&s);
 if(!eh_platform_start())return 1;
 paint(&s);
 while(!s.page.should_exit&&!s.request_launch){
  EhKey key=eh_platform_readkey();
  if(eh6_handle(&s,key)&&!s.page.should_exit&&!s.request_launch)paint(&s);
  if(!s.page.should_exit&&!s.request_launch)eh_platform_sleep();
 }
 /* CRITICAL: restore the TI screen and clock BEFORE calling Ndless.
  * Do not initialize our video again after the test returns. */
 eh_platform_stop();
 if(!s.request_launch)return 0;
 char path[512];
 if(argc<1||!argv||!eh6_sibling_path(path,sizeof(path),argv[0])){
  show_msgbox("EndraHome 006","Could not resolve Documents path.");
  return 1;
 }
 FILE *f=fopen(path,"rb");
 if(!f){
  show_msgbox("EndraHome 006","Test target missing. Place both TNS files in one folder.");
  return 1;
 }
 fclose(f);
 msleep(180);
 /* OFFICIAL Ndless: no args -> nl_exec(path,0,NULL). Run001 had
  * incorrectly supplied the executable path as a program argument. */
 int result=nl_exec(path,0,NULL);
 if(result<0)show_msgbox("EndraHome 006","Ndless rejected the test launch.");
 refresh_osscr();
 return result<0?1:0;
}
