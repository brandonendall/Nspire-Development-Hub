#include "overlay.h"
/* Defined once in render.c, as in the verified Run006 renderer. */
extern char font8x8_basic[128][8];
#include <stdint.h>
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void rect(uint16_t *f,int x,int y,int w,int h,uint16_t col){
 for(int j=0;j<h;j++)for(int i=0;i<w;i++){
  int a=x+i,b=y+j;
  if((unsigned)a<320&&(unsigned)b<240)f[b*320+a]=col;
 }
}
static void txt(uint16_t *f,int x,int y,const char *s,uint16_t col,int max){
 for(int k=0;s&&s[k]&&k<max;k++){
  unsigned ch=(unsigned char)s[k];if(ch>127)ch='?';
  for(int row=0;row<8;row++){
   unsigned bits=(unsigned char)font8x8_basic[ch][row];
   for(int colx=0;colx<8;colx++)if(bits&(1u<<colx))
    rect(f,x+k*8+colx,y+row,1,1,col);
  }
 }
}
void eh7_overlay(uint16_t *fb,const EhUniversalUI *s){
 if(!fb||!s||(!s->confirm&&!s->missing))return;
 uint16_t edge=RGB(69,211,238),fg=RGB(236,243,252),dim=RGB(168,188,207);
 rect(fb,12,64,296,120,RGB(6,13,24));
 rect(fb,15,67,290,114,edge);
 rect(fb,17,69,286,110,RGB(18,33,51));
 if(s->missing){
  txt(fb,29,82,"SHORTCUT NOT SET",edge,27);
  txt(fb,29,104,"Edit the apps config file",fg,31);
  txt(fb,29,126,"EndraHome_Apps007.cfg.tns",dim,31);
  txt(fb,29,153,"ENTER / ESC - BACK",edge,27);
 }else{
  int slot=s->confirm-1;
  txt(fb,29,82,"LAUNCH APPLICATION?",edge,26);
  txt(fb,29,103,s->apps->slots[slot].label,fg,15);
  txt(fb,29,124,"App exits to TI Documents",dim,31);
  txt(fb,29,153,"ENTER YES   ESC CANCEL",edge,30);
 }
}
