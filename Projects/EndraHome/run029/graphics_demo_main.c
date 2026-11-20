/* Standalone read-only graphics preview: selected artwork is decoded from
 * background_asset.h, with no app tiles or configuration access. */
#include "../gba-ui/platform.h"
#include "platform029.h"
#include "background_asset.h"
#include <libndls.h>
#include <stdint.h>
#include <stddef.h>
void __attribute__((weak)) _fini(void){}
static int decode(uint16_t *dst){
 size_t src=0,out=0;
 while(src<ENDRA_BG_WORDS && out<320u*240u){
  uint16_t tag=endra_bg_rle[src++];
  size_t count=(tag&127u)+1u;
  if(out+count>320u*240u)return 0;
  if(tag&0x8000u){
   if(src>=ENDRA_BG_WORDS)return 0;
   uint16_t color=endra_bg_rle[src++];
   for(size_t j=0;j<count;j++)dst[out++]=color;
  }else{
   if(src+count>ENDRA_BG_WORDS)return 0;
   for(size_t j=0;j<count;j++)dst[out++]=endra_bg_rle[src++];
  }
 }
 return out==320u*240u;
}
int main(void){
 if(!eh_platform_start())return 1;
 if(!decode(eh_framebuffer())){eh_platform_stop();return 2;}
 eh_platform_flip();
 for(;;){EhKey k=eh_platform_readkey();if(k==EH_KEY_ESC||eh_platform_exit_chord())break;eh_platform_sleep();}
 eh_platform_stop();return 0;
}
