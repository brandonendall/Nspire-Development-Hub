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
static void file_icon(uint16_t *f,int x,int y,const char *name,int dir,uint16_t accent){
 uint16_t white=RGB(238,245,255),gold=RGB(241,185,71);
 if(dir){
  box(f,x+1,y+3,9,3,gold);box(f,x,y+6,15,9,gold);
  box(f,x+2,y+8,11,5,RGB(255,216,126));return;
 }
 box(f,x+2,y+1,12,15,white);
 box(f,x+10,y+1,4,4,RGB(153,171,190));
 int len=(int)strlen(name);
 const char *ext=strrchr(name,'.');
 if(ext&&(!strcmp(ext,".zip")||!strcmp(ext,".ZIP"))){
  box(f,x+2,y+1,12,15,RGB(48,169,103));label(f,x+4,y+5,"Z",white,1);
 }else if(ext&&(!strcmp(ext,".jpg")||!strcmp(ext,".png")||!strcmp(ext,".bmp"))){
  box(f,x+4,y+6,8,6,RGB(62,179,129));box(f,x+5,y+9,5,2,white);
 }else if(ext&&(!strcmp(ext,".tns"))){
  box(f,x+5,y+6,6,6,accent);
 }else if(ext&&(!strcmp(ext,".cfg")||!strcmp(ext,".txt")||!strcmp(ext,".pdf"))){
  box(f,x+4,y+6,8,1,accent);box(f,x+4,y+9,7,1,accent);
 }else if(len>0){box(f,x+5,y+8,6,2,accent);}
}
static void size_label(char *out,size_t cap,unsigned long long n){
 const char *units[]={"B","KB","MB","GB","TB"};
 unsigned unit=0;unsigned long long whole=n,rem=0;
 while(whole>=1024&&unit<4){rem=whole%1024;whole/=1024;unit++;}
 if(unit==0)snprintf(out,cap,"%lu B",(unsigned long)whole);
 else{
  unsigned tenth=(unsigned)((rem*10+512)/1024);
  if(tenth==10){whole++;tenth=0;}
  if(tenth)snprintf(out,cap,"%lu.%u %s",(unsigned long)whole,tenth,units[unit]);
  else snprintf(out,cap,"%lu %s",(unsigned long)whole,units[unit]);
 }
}
void eh13_picker_render(uint16_t *f,const Eh13Picker *p,const EhRegistry *apps){
 if(!f||!p||!p->active||!apps)return;
 uint16_t bg=RGB(25,29,34),light=RGB(239,246,248),dim=RGB(180,195,204),cyan=RGB(68,210,237),highlight=RGB(40,113,139);
 box(f,0,0,320,240,bg);
 box(f,0,0,320,28,RGB(48,55,63));
 label(f,12,10,p->show_all?"ENDRA FILES":"ASSIGN APP",light,13);
 if(!p->show_all)label(f,118,10,apps->slots[p->slot].label,cyan,16);
 box(f,8,35,304,29,RGB(57,64,72));
 label(f,12,42,"PATH:",cyan,8);
 /* Show path tail if very long; keeps last directory names visible. */
 const char *s=p->dir;size_t len=strlen(s);
 if(len>30)s+=len-30;
 label(f,61,42,s,light,31);
 if(p->show_all){label(f,39,65,"NAME",cyan,6);label(f,272,65,"SIZE",cyan,5);}
 int bottom=p->scroll+EH13_VIEW_ROWS;
 if(bottom>p->count)bottom=p->count;
 for(int i=p->scroll;i<bottom;i++){
  int y=81+(i-p->scroll)*15;
  if(i==p->selected)box(f,10,y-2,300,15,highlight);
  const Eh13PickEntry *e=p->items+i;
  file_icon(f,17,y-3,e->name,e->directory,cyan);
  /* 23 glyphs for names; size column starts at x=255. */
  /* Never paint filenames into the right-aligned SIZE column. */
  int maxname=p->show_all?23:32;
  label(f,39,y,e->name,light,maxname);
  if(p->show_all){
   char sz[24];
   if(e->directory)snprintf(sz,sizeof(sz),"--");
   else size_label(sz,sizeof(sz),e->bytes);
   int chars=(int)strlen(sz);
   label(f,312-chars*8,y,sz,i==p->selected?light:dim,chars);
  }
 }
 if(!p->count)label(f,19,93,p->show_all?"EMPTY FOLDER":"NO APPS IN THIS FOLDER",dim,30);
 char status[64];
 snprintf(status,sizeof(status),"%d FILES/FOLDERS%s",p->count,p->overflow?" (LIMIT)":"");
 label(f,12,209,status,p->overflow?RGB(255,157,98):dim,35);
 box(f,0,223,320,17,RGB(43,49,56));
 label(f,7,227,p->show_all?"ENTER OPEN/INFO ESC BACK":"ENTER OPEN/ASSIGN ESC BACK",cyan,32);
}
