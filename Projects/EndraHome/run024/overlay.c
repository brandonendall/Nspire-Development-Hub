/* Run013 nonblocking result toast and no unnecessary confirmations. */
#include "overlay.h"
extern char font8x8_basic[128][8];
#include <stdint.h>
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void rect(uint16_t *f,int x,int y,int w,int h,uint16_t c){
 for(int j=0;j<h;j++)for(int i=0;i<w;i++){
  int a=x+i,b=y+j;
  if((unsigned)a<320&&(unsigned)b<240)f[b*320+a]=c;
 }
}
static void txt(uint16_t *f,int x,int y,const char *s,uint16_t c,int max){
 for(int k=0;s&&s[k]&&k<max;k++){
  unsigned ch=(unsigned char)s[k];if(ch>127)ch='?';
  for(int row=0;row<8;row++){
   unsigned bits=(unsigned char)font8x8_basic[ch][row];
   for(int xx=0;xx<8;xx++)if(bits&(1u<<xx))
    rect(f,x+k*8+xx,y+row,1,1,c);
  }
 }
}
void eh7_overlay(uint16_t *fb,const EhUniversalUI *s){
 if(!fb||!s)return;
 if(s->notice[0]){
  rect(fb,12,122,296,17,RGB(11,34,45));
  txt(fb,18,126,s->notice,RGB(107,250,204),36);
  return;
 }
 if(!s->missing)return;
 uint16_t edge=RGB(69,211,238),fg=RGB(236,243,252),dim=RGB(168,188,207);
 rect(fb,12,64,296,120,RGB(6,13,24));
 rect(fb,15,67,290,114,edge);
 rect(fb,17,69,286,110,RGB(18,33,51));
 txt(fb,29,82,"SHORTCUT NOT SET",edge,27);
 txt(fb,29,105,"MENU THEN ENTER TO ASSIGN",fg,31);
 txt(fb,29,126,EH7_CONFIG_FILE,dim,31);
 txt(fb,29,153,"MENU INFO   ESC BACK",edge,28);
}
