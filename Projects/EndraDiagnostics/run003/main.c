/*
 * EndraDiagnostics Run003 — INPUT + EXIT ONLY.
 * No console, benchmark, filesystem, battery/MMIO probes or heap allocation.
 * Uses identical proven Run003-origin platform implementation from Run002.
 */
#include "../run002/platform.h"
#include <stdint.h>
#include "font8x8_basic.h"
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static uint16_t *fb;
void __attribute__((weak)) _fini(void) {}
static void rect(int x,int y,int w,int h,uint16_t c){
  if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;}
  if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
  if(x+w>EH_W)w=EH_W-x;
  if(y+h>EH_H)h=EH_H-y;
  for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)fb[j*EH_W+i]=c;
}
static void print8(int x,int y,const char *s,int max,uint16_t c){
  for(int i=0;s&&s[i]&&i<max;i++){
    unsigned char letter=(unsigned char)s[i];
    if(letter>127)letter='?';
    for(int row=0;row<8;row++){
      unsigned char bits=font8x8_basic[letter][row];
      for(int col=0;col<8;col++)if(bits&(1u<<col))rect(x+i*8+col,y+row,1,1,c);
    }
  }
}
static const char *key_label(EhKey k){
  switch(k){
    case EH_KEY_UP:return "UP";
    case EH_KEY_DOWN:return "DOWN";
    case EH_KEY_LEFT:return "LEFT";
    case EH_KEY_RIGHT:return "RIGHT";
    case EH_KEY_ENTER:return "ENTER";
    case EH_KEY_MENU:return "MENU";
    case EH_KEY_ESC:return "ESC";
    default:return "NONE";
  }
}
static void draw(EhKey last, unsigned steps){
  const uint16_t dark=RGB(9,17,29),panel=RGB(25,41,58),
      white=RGB(238,244,249),teal=RGB(66,224,199),dim=RGB(155,180,193);
  fb=eh_framebuffer();
  rect(0,0,EH_W,EH_H,dark);
  rect(0,0,EH_W,34,panel);rect(0,0,4,34,teal);
  print8(10,12,"ENDRA DIAGNOSTICS",18,teal);
  print8(275,12,"003",3,white);
  print8(13,52,"LIVE KEYPAD TEST",32,white);
  print8(13,78,"LAST KEY:",31,dim);
  print8(110,78,key_label(last),14,teal);
  print8(13,107,"ARROWS / ENTER / MENU",31,white);
  print8(13,128,"ESC EXITS IMMEDIATELY",31,white);
  print8(13,153,"NO MEMORY OR CPU TESTS",31,dim);
  print8(13,172,"NO BATTERY REGISTER READ",31,dim);
  rect(9,197,302,31,panel);
  print8(14,202,"AUTO EXIT AT ~12 SECONDS",32,teal);
  rect(15,218,(int)((steps*290u)/800u),4,teal);
  eh_platform_flip();
}
int main(void){
  EhKey last=EH_KEY_NONE;
  if(!eh_platform_start())return 1;
  draw(last,0);
  for(unsigned i=0;i<800u;i++){
    EhKey key=eh_platform_readkey();
    if(key==EH_KEY_ESC)break;
    if(key!=EH_KEY_NONE)last=key;
    /* Limited refresh rate. All platform state restored on normal exit. */
    if(key!=EH_KEY_NONE || (i%80u)==79u)draw(last,i+1);
    eh_platform_sleep();
  }
  eh_platform_stop();
  return 0;
}
