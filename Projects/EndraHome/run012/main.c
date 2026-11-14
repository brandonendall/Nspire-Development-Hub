/* EndraHome Run012 quick launch; preserves Run011 return-home and Run010 fast exit. */
#include "../gba-ui/platform.h"
#include "registry.h"
#include "ui.h"
#include "render.h"
#include "overlay.h"
#include "session.h"
#include <libndls.h>
#include <string.h>
void __attribute__((weak)) _fini(void){}
static void paint(const EhUniversalUI *s){
 uint16_t *fb=eh_framebuffer();
 eh7_render(fb,&s->page,s->apps);
 eh7_overlay(fb,s);
 eh_platform_flip();
}
/* Display/timer MUST be restored before any child launch. */
static void launch_selected(const EhUniversalUI *ui,int argc,char **argv){
 int slot=ui->request-1;
 if(slot<0||slot>=EH7_SLOT_COUNT)return;
 const char *path=ui->apps->slots[slot].path;
 if(!eh7_valid_path(path)){
  show_msgbox("EndraHome 012","Invalid path. Check EndraHome_Apps.cfg.tns.");return;
 }
 if(argc>0&&argv&&argv[0]&&!strcmp(path,argv[0])){
  show_msgbox("EndraHome 012","Cannot launch EndraHome itself.");return;
 }
 int result=nl_exec(path,0,NULL);
 if(result<0)show_msgbox("EndraHome 012","App failed to launch. Check path and Ndless.");
}
int main(int argc,char **argv){
 EhRegistry apps;
 eh7_registry_init(&apps);
 char cfg[512];
 if(argc>0&&argv&&eh7_config_path(cfg,sizeof(cfg),argv[0]))
  eh7_registry_load(&apps,cfg);
 EhUniversalUI ui;
 eh7_ui_init(&ui,&apps);
 for(;;){
  if(!eh_platform_start())return 1;
  paint(&ui);
  while(!ui.page.should_exit&&!ui.request){
   EhKey key=eh_platform_readkey();
   if(eh7_ui_handle(&ui,key)&&!ui.page.should_exit&&!ui.request)
    paint(&ui);
   if(!ui.page.should_exit&&!ui.request)eh_platform_sleep();
  }
  eh_platform_stop(); /* preserved native LCD/timer restore, no browser refresh */
  if(ui.page.should_exit||!ui.request)return 0;
  launch_selected(&ui,argc,argv);
  /* Only when child returns from nl_exec: return to EndraHome without relaunch. */
  eh11_resume_home(&ui);
 }
}
