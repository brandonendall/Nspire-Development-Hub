/* Text-only fixed-size UI on Run010 native RGB565 framebuffer. */
#include "picker.h"
/* Font pixels are defined by render.c; avoid a duplicate global definition. */
extern char font8x8_basic[128][8];
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void box(uint16_t *f,int x,int y,int w,int h,uint16_t col){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(w<=0||h<=0||x>=320||y>=240)return;
 if(x+w>320)w=320-x;
 if(y+h>240)h=240-y;
 for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)f[j*320+i]=col;
}
static void label(uint16_t *f,int x,int y,const char *s,uint16_t col,int max){
 for(int k=0;s&&s[k]&&k<max;k++){
  unsigned ch=(unsigned char)s[k];if(ch>127)ch='?';
  for(int row=0;row<8;row++){
   unsigned bits=(unsigned char)font8x8_basic[ch][row];
   for(int xx=0;xx<8;xx++)if(bits&(1u<<xx))
    box(f,x+k*8+xx,y+row,1,1,col);
  }
 }
}
void eh13_picker_render(uint16_t *f,const Eh13Picker *p,const EhRegistry *apps){
 if(!f||!p||!p->active||!apps)return;
 uint16_t bg=RGB(11,17,31),light=RGB(241,249,255),dim=RGB(155,181,204),cyan=RGB(53,205,250),highlight=RGB(28,87,125);
 box(f,0,0,320,240,bg);
 box(f,0,0,320,28,RGB(31,45,65));
 label(f,12,10,"ASSIGN APP",light,13);
 label(f,118,10,apps->slots[p->slot].label,cyan,16);
 box(f,8,35,304,29,RGB(25,37,55));
 label(f,12,42,"FOLDER:",cyan,8);
 /* Show path tail if very long; keeps last directory names visible. */
 const char *s=p->dir;size_t len=strlen(s);
 if(len>30)s+=len-30;
 label(f,77,42,s,light,30);
 int bottom=p->scroll+EH13_VIEW_ROWS;
 if(bottom>p->count)bottom=p->count;
 for(int i=p->scroll;i<bottom;i++){
  int y=70+(i-p->scroll)*17;
  if(i==p->selected)box(f,10,y-2,300,16,highlight);
  const Eh13PickEntry *e=p->items+i;
  label(f,17,y,e->directory?"DIR":"TNS",e->directory?cyan:dim,4);
  label(f,62,y,e->name,light,30);
 }
 if(!p->count)label(f,19,93,"NO APPS IN THIS FOLDER",dim,30);
 char status[64];
 snprintf(status,sizeof(status),"%d FILES/FOLDERS%s",p->count,p->overflow?" (LIMIT)":"");
 label(f,12,209,status,p->overflow?RGB(255,157,98):dim,35);
 box(f,0,223,320,17,RGB(24,39,58));
 label(f,7,227,"ENTER OPEN/ASSIGN ESC BACK",cyan,29);
}
