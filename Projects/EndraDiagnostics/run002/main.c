/*
 * EndraDiagnostics Run002 -- screen and controlled exit probe.
 * No console, malloc, benchmark, filesystem writes or app launch.
 * platform.c is copied verbatim from the Run003 EndraHome baseline.
 */
#include "platform.h"
#include <stdint.h>
#include <string.h>
#include "font8x8_basic.h"

#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static uint16_t *fb;
void __attribute__((weak)) _fini(void) {}

static void rect(int x,int y,int w,int h,uint16_t c){
  if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;}
  if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
  if(x+w>EH_W)w=EH_W-x;
  if(y+h>EH_H)h=EH_H-y;
  for(int j=y;j<y+h;j++)
    for(int i=x;i<x+w;i++)fb[j*EH_W+i]=c;
}
static void print8(int x,int y,const char *s,int max,uint16_t c){
  for(int i=0;s&&s[i]&&i<max;i++){
    unsigned char letter=(unsigned char)s[i];
    if(letter>127)letter='?';
    for(int row=0;row<8;row++){
      unsigned char pixels=font8x8_basic[letter][row];
      for(int col=0;col<8;col++)
        if(pixels&(1u<<col))rect(x+i*8+col,y+row,1,1,c);
    }
  }
}
static void render(void){
  uint16_t dark=RGB(9,17,29), panel=RGB(25,41,58);
  uint16_t white=RGB(237,244,248), teal=RGB(66,224,199), dim=RGB(159,181,196);
  fb=eh_framebuffer();
  rect(0,0,EH_W,EH_H,dark);
  rect(0,0,EH_W,34,panel);
  rect(0,0,4,34,teal);
  print8(10,12,"ENDRA DIAGNOSTICS",18,teal);
  print8(275,12,"002",3,white);
  print8(12,51,"INTERFACE / EXIT TEST",28,white);
  print8(12,73,"CPU: ARM926",28,white);
  print8(12,91,"ARCH: ARMv5TEJ",28,white);
  print8(12,109,"D-CACHE: 8 KiB",28,white);
  print8(12,127,"I-CACHE: 16 KiB",28,white);
  print8(12,145,"OS 6.4.0.74 (TARGET)",30,white);
  print8(12,168,"REFERENCE DATA ONLY",30,dim);
  rect(7,191,306,36,panel);
  print8(16,200,"ESC = EXIT",28,teal);
  print8(16,213,"AUTO EXIT AFTER ~12S",28,dim);
  eh_platform_flip();
}
int main(void){
  if(!eh_platform_start())return 1;
  render();
  /* A bounded loop avoids relying solely on keypad exit.
     Uses the original Run003 15 ms sleep/readkey handling. */
  for(unsigned i=0;i<800u;i++){
    EhKey key=eh_platform_readkey();
    if(key==EH_KEY_ESC)break;
    eh_platform_sleep();
  }
  eh_platform_stop();
  return 0;
}
