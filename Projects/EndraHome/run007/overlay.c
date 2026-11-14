#include "overlay.h"
extern char font8x8_basic[128][8]; /* Defined in single renderer object. */
#include <stdint.h>
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void rect(uint16_t *fb,int x,int y,int w,int h,uint16_t color){
 for(int row=0;row<h;row++)for(int col=0;col<w;col++){
  int xx=x+col,yy=y+row;
  if((unsigned)xx<320&&(unsigned)yy<240)fb[yy*320+xx]=color;
 }
}
static void line(uint16_t *fb,int x,int y,const char *s,uint16_t c){
 for(int i=0;s&&s[i]&&i<33;i++){
  unsigned ch=(unsigned char)s[i];if(ch>127)ch='?';
  for(int row=0;row<8;row++){
   unsigned char bits=(unsigned char)font8x8_basic[ch][row];
   for(int col=0;col<8;col++)
    if(bits&(1u<<col))rect(fb,x+i*8+col,y+row,1,1,c);
  }
 }
}
void eh7launch_dialog(uint16_t *fb,int target){
 if(!fb)return;
 uint16_t edge=RGB(49,205,247),white=RGB(235,244,252),dim=RGB(165,191,211);
 rect(fb,17,66,286,114,RGB(6,15,25));
 rect(fb,20,69,280,108,edge);
 rect(fb,22,71,276,104,RGB(17,31,49));
 line(fb,35,82,target==1?"LAUNCH ZIP APP?":"RETEST TOOLS APP?",edge);
 line(fb,35,104,target==1?"Real app - experimental":"Known tiny test program",white);
 line(fb,35,123,target==1?"No files will be changed here":"No files will be modified",dim);
 line(fb,35,149,"ENTER GO   ESC CANCEL",edge);
}
