/* Standalone read-only graphics test, separate from EndraHome launcher. */
#include "../gba-ui/platform.h"
#include "platform029.h"
#include "asset031.h"
#include <libndls.h>
#include <stdint.h>
void __attribute__((weak)) _fini(void){}
static uint16_t rgb(unsigned r,unsigned g,unsigned b){return (uint16_t)(((r>>3)<<11)|((g>>2)<<5)|(b>>3));}
int main(void){
 if(!eh_platform_start())return 1;
 uint16_t *fb=eh_framebuffer();
 for(int y=0;y<240;y++)for(int x=0;x<320;x++){
   unsigned glow=(unsigned)(x<160?x:319-x);
   unsigned r=2+(glow*7)/160;
   unsigned g=7+(glow*20)/160+(unsigned)(239-y)/25;
   unsigned b=19+(glow*45)/160+(unsigned)(239-y)/12;
   fb[y*320+x]=rgb(r,g,b);
 }
 /* Header and footer visual reference; no applications and no file writes. */
 for(int y=0;y<31;y++)for(int x=0;x<320;x++)fb[y*320+x]=rgb(3,13,28);
 for(int y=210;y<240;y++)for(int x=0;x<320;x++)fb[y*320+x]=rgb(3,13,28);
 /* Minimal border to verify full LCD extent. */
 for(int x=0;x<320;x++){fb[31*320+x]=rgb(19,116,204);fb[209*320+x]=rgb(19,116,204);}
 eh_platform_flip();
 for(;;){EhKey key=eh_platform_readkey();if(key==EH_KEY_ESC||eh_platform_exit_chord())break;eh_platform_sleep();}
 eh_platform_stop();return 0;
}
