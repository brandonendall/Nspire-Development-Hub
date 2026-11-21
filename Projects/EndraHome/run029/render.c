#include "render.h"
#include "desktop.h"
#include "display.h"
#include "art.h"
#include <stddef.h>
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
 unsigned brightness=(p->ink>>11)*3+((p->ink>>5)&63)*3+(p->ink&31);
 uint16_t edge=brightness>145?EH_RGB(2,5,10):EH_RGB(225,239,250);
 eh_small_text(f,x-1,y,text,edge,1,12);
 eh_small_text(f,x+1,y,text,edge,1,12);
 eh_small_text(f,x,y-1,text,edge,1,12);
 eh_small_text(f,x,y+1,text,edge,1,12);
 eh_small_text(f,x,y,text,p->ink,1,12);
}

static void draw_tile(uint16_t *f,const EhTile *t,const EhRegistry *r,const EhPalette *p,int selected){
 int cx=t->center,y=t->top;eh_icon_paint(f,r->appearance,cx-EH_ICON_SIZE/2,y);
 const char *name=t->slot==EH_FILES_SLOT?"Files":r->slots[t->slot].label;
 char label[13];size_t n=strlen(name);
 if(n>12){memcpy(label,name,9);memcpy(label+9,"...",4);n=12;}
 else {memcpy(label,name,n);label[n]=0;}
 int tx=cx-(int)n*3,ty=y+52;
 if(selected)eh_theme_glass(f,tx-4,ty-3,(int)n*6+8,15,55,p);
 eh_icon_caption(f,tx,ty,label,p);
 if(selected){
  eh_box(f,tx-2,ty+10,(int)n*6+4,1,eh_skin(p,EH_RGB(110,243,255)));
  eh_box(f,tx+2,ty+12,(int)n*6-4,1,eh_skin(p,EH_RGB(23,128,206)));
 }
}
void eh7_render(uint16_t *f,const EhUI7 *s,const EhRegistry *r){
 if(!f||!s||!r)return;
 EhPalette p=eh_palette(r->appearance);eh_desktop_wallpaper(f,r->appearance);
 EhTile tiles[EH7_SLOT_COUNT];int count=eh_desktop_layout(r,tiles);
 for(int i=0;i<count;i++)draw_tile(f,&tiles[i],r,&p,tiles[i].slot==s->selected);
}
