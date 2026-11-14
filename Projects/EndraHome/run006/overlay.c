#include "overlay.h"
/* Font bytes are defined once by the renderer to avoid duplicate globals. */
extern char font8x8_basic[128][8];
#include <stdint.h>
#define C(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void rect(uint16_t *fb,int x,int y,int w,int h,uint16_t c){
 for(int j=0;j<h;j++)for(int i=0;i<w;i++){
  int xx=x+i,yy=y+j;
  if((unsigned)xx<320&&(unsigned)yy<240)fb[yy*320+xx]=c;
 }
}
static void txt(uint16_t *fb,int x,int y,const char *s,uint16_t col){
 for(int k=0;s[k];k++){
  unsigned ch=(unsigned char)s[k];if(ch>127)ch='?';
  for(int row=0;row<8;row++){
   unsigned bits=(unsigned char)font8x8_basic[ch][row];
   for(int cx=0;cx<8;cx++)if(bits&(1u<<cx))
    rect(fb,x+k*8+cx,y+row,1,1,col);
  }
 }
}
void eh6_confirm_overlay(uint16_t *fb){
 if(!fb)return;
 const uint16_t edge=C(63,211,243),bg=C(12,29,46);
 rect(fb,19,68,282,112,C(3,9,20));
 rect(fb,22,71,276,106,edge);
 rect(fb,24,73,272,102,bg);
 txt(fb,36,85,"SAFE LAUNCH TEST",edge);
 txt(fb,36,106,"TEST APP ONLY - NOT GBA",C(238,245,255));
 txt(fb,36,126,"Two .tns files together",C(172,194,216));
 txt(fb,36,151,"ENTER YES   ESC CANCEL",edge);
}
