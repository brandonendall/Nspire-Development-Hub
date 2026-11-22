#include "render.h"
#include "desktop.h"
#include "display.h"
#include "art.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DEEP EH_RGB(4,13,29)

static uint16_t mix34(uint16_t a,uint16_t b,unsigned pc){
 unsigned q=100-pc;
 return (uint16_t)(((((a>>11)*q+(b>>11)*pc)/100)<<11) |
   (((((a>>5)&63)*q+((b>>5)&63)*pc)/100)<<5) |
   (((a&31)*q+(b&31)*pc)/100));
}
static void glass34(uint16_t *f,int x,int y,int w,int h,uint16_t tone,unsigned opacity){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x+w>320)w=320-x;
 if(y+h>240)h=240-y;
 if(w<=0||h<=0)return;
 for(int row=y;row<y+h;row++)
  for(int col=x;col<x+w;col++){
   size_t pos=(size_t)row*320+col;
   f[pos]=mix34(f[pos],tone,opacity);
  }
}
void eh_glass_panel(uint16_t *f,int x,int y,int w,int h,unsigned opacity) {
 if(!f)return;
 if(opacity>100)opacity=100;
 glass34(f,x,y,w,h,DEEP,opacity);
}
void eh_theme_glass(uint16_t *f,int x,int y,int w,int h,unsigned alpha,const EhPalette *p){
 if(f&&p)glass34(f,x,y,w,h,eh_skin(p,DEEP),alpha>100?100:alpha);
}

void eh_desktop_wallpaper(uint16_t *f,EhAppearance appearance){
 eh_wallpaper_paint(f,appearance);
 eh_header_paint(f,appearance);
}

void eh_icon_caption(uint16_t *f,int x,int y,const char *text,const EhPalette *p){
 eh_small_text_plain(f,x,y,text,p->ink,1,12);
}

/* A shallow capsule follows the label while leaving its interior transparent. */
static void caption_ring(uint16_t *f,int x,int y,int width,uint16_t ink){
 static const unsigned char inset[]={4,2,1,1,0,0,0,0,0,1,1,2,4};
 const int last=(int)sizeof(inset)-1;
 for(int row=0;row<=last;++row){
  int start=inset[row],span=width-2*start;
  if(row==0||row==last){
   eh_box(f,x+start,y+row,span,1,ink);
  }else{
   int shoulder=row<=last/2?inset[row-1]:inset[row+1];
   int stroke=shoulder-start;
   if(stroke<1)stroke=1;
   eh_box(f,x+start,y+row,stroke,1,ink);
   eh_box(f,x+width-start-stroke,y+row,stroke,1,ink);
  }
 }
}

static void draw_tile(uint16_t *f,const EhTile *t,const EhRegistry *r,const EhPalette *p,int selected){
 int cx=t->center,y=t->top;eh_icon_paint(f,r->appearance,cx-EH_ICON_SIZE/2,y);
 const char *name=t->slot==EH_FILES_SLOT?"Files":r->slots[t->slot].label;
 char label[13];size_t n=strlen(name);
 if(n>12){memcpy(label,name,9);memcpy(label+9,"...",4);n=12;}
 else {memcpy(label,name,n);label[n]=0;}
 int tx=cx-(int)n*3,ty=y+52;
 if(selected){
  int pad=n==12?3:5;
  int width=(int)n*6-1+2*pad;
  uint16_t ring=eh_skin(p,EH_RGB(131,240,255));
  uint16_t edge=eh_luma(ring)>125?EH_RGB(6,12,23):EH_RGB(225,239,246);
  caption_ring(f,tx-pad,ty-2,width,edge);
  caption_ring(f,tx-pad,ty-3,width,ring);
 }
 int white=r->appearance.wallpaper==EH_SOLID_WHITE;
 if((white && eh_luma(p->ink)>160) || (r->appearance.wallpaper==EH_SOLID_BLACK && eh_luma(p->ink)<95)){
  uint16_t edge=white?0:UINT16_MAX;
  eh_small_text_plain(f,tx-1,ty,label,edge,1,12);
  eh_small_text_plain(f,tx+1,ty,label,edge,1,12);
  eh_small_text_plain(f,tx,ty-1,label,edge,1,12);
  eh_small_text_plain(f,tx,ty+1,label,edge,1,12);
 }
 eh_icon_caption(f,tx,ty,label,p);
}
void eh_clock_seconds_label(char label[12],int second){
 if(second<0 || second>=86400){label[0]=0;return;}
 int hour=second/3600;
 snprintf(label,12,"%02d:%02d:%02d %s",hour%12?hour%12:12,
          second/60%60,second%60,hour<12?"AM":"PM");
}
void eh_desktop_clock(uint16_t *frame,int second,int hide_seconds){
 char label[12];
 eh_clock_seconds_label(label,second);
 /* One-pixel contour follows only the glyphs on bright wallpaper. */
 if(hide_seconds && label[0])memmove(label+5,label+8,4);
 const int x=hide_seconds?265:247,y=14;
 eh_small_text_plain(frame,x-1,y,label,0,1,11);
 eh_small_text_plain(frame,x+1,y,label,0,1,11);
 eh_small_text_plain(frame,x,y-1,label,0,1,11);
 eh_small_text_plain(frame,x,y+1,label,0,1,11);
 eh_small_text_plain(frame,x,y,label,UINT16_MAX,1,11);
}
void eh7_render(uint16_t *f,const EhUI7 *s,const EhRegistry *r){
 if(!f||!s||!r)return;
 EhPalette p=eh_palette(r->appearance);eh_desktop_wallpaper(f,r->appearance);
 EhTile tiles[EH7_SLOT_COUNT];int count=eh_desktop_layout(r,tiles);
 for(int i=0;i<count;i++)draw_tile(f,&tiles[i],r,&p,tiles[i].slot==s->selected);
}
