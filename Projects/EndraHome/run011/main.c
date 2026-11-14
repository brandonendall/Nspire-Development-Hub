/*
 * EndraHome Run011: return to launcher after nl_exec returns from a child.
 * Keep Run010's fast shutdown and tested video/timer restore sequence.
 * This experimental approach cannot intercept crashes, resets or forced exits.
 */
#include "../gba-ui/platform.h"
#include "registry.h"
#include "../run007/ui.h"
#include "render.h"
#include "../run007/overlay.h"
#include "session.h"
#include <libndls.h>
#include <string.h>
void __attribute__((weak)) _fini(void){}
static void paint(const EhUniversalUI *s) {
 uint16_t *fb=eh_framebuffer();
 eh7_render(fb,&s->page,s->apps);
 eh7_overlay(fb,s);
 eh_platform_flip();
}
/* Call only after eh_platform_stop(), never while controlling video. */
static void launch_selected(const EhUniversalUI *ui,int argc,char **argv) {
 int slot=ui->request-1;
 if(slot<0||slot>=EH7_SLOT_COUNT)return;
 const char *path=ui->apps->slots[slot].path;
 if(!eh7_valid_path(path)) {
  show_msgbox("EndraHome 011","Invalid path. Check EndraHome_Apps.cfg.tns.");
  return;
 }
 if(argc>0&&argv&&argv[0]&&!strcmp(path,argv[0])) {
  show_msgbox("EndraHome 011","EndraHome cannot launch itself.");
  return;
 }
 /* Ndless zero-argument form; no hooks or document-browser refresh. */
 int result=nl_exec(path,0,NULL);
 if(result<0)show_msgbox("EndraHome 011","Could not launch app. Check path and Ndless.");
}
int main(int argc,char **argv) {
 EhRegistry apps;
 eh7_registry_init(&apps);
 char cfg[512];
 if(argc>0&&argv&&eh7_config_path(cfg,sizeof(cfg),argv[0]))
  eh7_registry_load(&apps,cfg);
 EhUniversalUI ui;
 eh7_ui_init(&ui,&apps);
 for(;;) {
  if(!eh_platform_start())return 1;
  paint(&ui);
  while(!ui.page.should_exit&&!ui.request) {
   EhKey key=eh_platform_readkey();
   if(eh7_ui_handle(&ui,key)&&!ui.page.should_exit&&!ui.request)
    paint(&ui);
   if(!ui.page.should_exit&&!ui.request)eh_platform_sleep();
  }
  /* Restore TI LCD + timer before touching another Ndless app. */
  eh_platform_stop();
  if(ui.page.should_exit||!ui.request)return 0;
  launch_selected(&ui,argc,argv);
  /* Child execution has returned: clear launch/confirmation state. */
  eh11_resume_home(&ui);
 }
}
