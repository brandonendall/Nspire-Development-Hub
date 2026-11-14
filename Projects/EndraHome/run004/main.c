/*
 * EndraHome Run004: custom visual design over UNCHANGED Run003 platform/logic.
 * No app launch, file browsing, system hooks, or new hardware interactions.
 */
#include "render.h"
#include "../gba-ui/platform.h"
#include "../gba-ui/logic.h"
#include <libndls.h>
void __attribute__((weak)) _fini(void) {}
int main(void){
  EhUI ui;
  eh_ui_init(&ui);
  if(!eh_platform_start())return 1;
  eh_render004(eh_framebuffer(),&ui);
  eh_platform_flip();
  while(!ui.should_exit){
    EhKey key=eh_platform_readkey();
    if(eh_ui_dispatch(&ui,key) && !ui.should_exit){
      eh_render004(eh_framebuffer(),&ui);
      eh_platform_flip();
    }
    if(!ui.should_exit)eh_platform_sleep();
  }
  eh_platform_stop();
  return 0;
}
