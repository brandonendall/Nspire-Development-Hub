/* Run005: closer concept styling, unchanged native input/display from Run003. */
#include "../gba-ui/platform.h"
#include "render.h"
#include <libndls.h>
void __attribute__((weak)) _fini(void){}
int main(void){
 EhUI7 ui;eh7_init(&ui);
 if(!eh_platform_start())return 1;
 eh5_render(eh_framebuffer(),&ui);eh_platform_flip();
 while(!ui.should_exit){
  EhKey key=eh_platform_readkey();
  if(eh7_dispatch(&ui,key)&&!ui.should_exit){eh5_render(eh_framebuffer(),&ui);eh_platform_flip();}
  if(!ui.should_exit)eh_platform_sleep();
 }
 eh_platform_stop();return 0;
}
