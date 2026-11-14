/*
 * EndraHome Run008 (UI polish): the universal seven-shortcut launcher with clearer captions/status.
 * Run003's proven GBA-native LCD and keys are compiled UNMODIFIED.
 * No hard-coded targets; uses the SAME EndraHome_Apps007.cfg.tns as Run007.
 */
#include "../gba-ui/platform.h"
#include "../run007/registry.h"
#include "../run007/ui.h"
#include "../run007/render.h"
#include "../run007/overlay.h"
#include <libndls.h>
#include <stdio.h>
#include <string.h>
void __attribute__((weak)) _fini(void){}
static void paint(const EhUniversalUI *s){
 uint16_t *fb=eh_framebuffer();
 eh7_render(fb,&s->page,s->apps);
 eh7_overlay(fb,s);
 eh_platform_flip();
}
int main(int argc,char **argv){
 EhRegistry apps;
 eh7_registry_init(&apps);
 char cfg[512];
 /* Read once before taking over the display. Never write configuration. */
 if(argc>0&&argv&&eh7_config_path(cfg,sizeof(cfg),argv[0]))
   eh7_registry_load(&apps,cfg);
 EhUniversalUI s;
 eh7_ui_init(&s,&apps);
 if(!eh_platform_start())return 1;
 paint(&s);
 while(!s.page.should_exit&&!s.request){
  EhKey key=eh_platform_readkey();
  if(eh7_ui_handle(&s,key)&&!s.page.should_exit&&!s.request)paint(&s);
  if(!s.page.should_exit&&!s.request)eh_platform_sleep();
 }
 /* Proven from Run006: restore the original LCD and TI OS BEFORE executing
  * any child program. Do not redraw EndraHome after the child returns. */
 eh_platform_stop();
 if(!s.request)return 0;
 int slot=s.request-1;
 if(slot<0||slot>=EH7_SLOT_COUNT)return 1;
 const char *path=apps.slots[slot].path;
 if(!eh7_valid_path(path)){
  show_msgbox("EndraHome 008","Invalid app path. Check your config.");
  return 1;
 }
 if(argc>0&&argv&&argv[0]&&!strcmp(path,argv[0])){
  show_msgbox("EndraHome 008","A shortcut cannot launch EndraHome itself.");
  return 1;
 }
 FILE *f=fopen(path,"rb");
 if(!f){
  show_msgbox("EndraHome 008","App not found. Check its exact .tns path.");
  return 1;
 }
 fclose(f);
 msleep(180);
 int result=nl_exec(path,0,NULL);
 if(result<0)show_msgbox("EndraHome 008","Ndless could not launch this app.");
 refresh_osscr();
 return result<0?1:0;
}
