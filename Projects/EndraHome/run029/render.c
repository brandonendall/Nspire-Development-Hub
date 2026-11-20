/* EndraHome Run034: minimal wordmark chrome over the known-working Run031
 * native RGB565 artwork. Static art is decoded only on UI redraw; no
 * device writes other than the unchanged framebuffer platform.
 * One coordinated theme, no duplicate badge, no fake battery meter. */
#include "render.h"
#include "desktop.h"
#include "display.h"
#include "background_asset.h"
#include <stddef.h>
#include <string.h>

#define SKY EH_RGB(53,219,252)
#define INK EH_RGB(239,250,255)
#define DEEP EH_RGB(4,13,29)

static uint16_t mix34(uint16_t a,uint16_t b,unsigned pc){
 unsigned q=100-pc;
 return (uint16_t)(((((a>>11)*q+(b>>11)*pc)/100)<<11) |
   (((((a>>5)&63)*q+((b>>5)&63)*pc)/100)<<5) |
   (((a&31)*q+(b&31)*pc)/100));
}
static int decode34(uint16_t *f){
 size_t src=0,pos=0;
 while(src<ENDRA_BG_WORDS && pos<320u*240u){
  uint16_t word=endra_bg_rle[src++];
  size_t count=(word&127u)+1u;
  if(count>320u*240u-pos)return 0;
  if(word&0x8000u){
   if(src>=ENDRA_BG_WORDS)return 0;
   uint16_t color=endra_bg_rle[src++];
   for(size_t n=0;n<count;n++)f[pos++]=color;
  }else{
   if(count>ENDRA_BG_WORDS-src)return 0;
   for(size_t n=0;n<count;n++)f[pos++]=endra_bg_rle[src++];
  }
 }
 return pos==320u*240u;
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
static void draw_tile34(uint16_t *f,const EhTile *t,const EhRegistry *r,
                        const EhPalette *p,int selected){
 const int w=66,h=63,x=t->center-w/2,y=t->top;
 uint16_t thin=EH_RGB(37,90,126);
 /* Matching glass panels: current selection is a thin cyan trim,
  * not an enormous light rectangle obscuring the original artwork. */
 glass34(f,x-3,y-3,w+6,h+6,DEEP,70);
 eh_box(f,x-1,y-1,w+2,h+2,selected?SKY:thin);
 eh_box(f,x,y,w,h,EH_RGB(6,21,43));
 if(selected){
  eh_box(f,x+2,y+2,w-4,2,EH_RGB(140,248,255));
  eh_box(f,x+2,y+h-4,w-4,2,SKY);
  eh_box(f,x+2,y+3,2,h-7,SKY);
 }
 eh_app_icon(f,t->center-16,y+6,2);
 /* Files uses the same clean E badge as other slots. Run033's tiny
  * top-left yellow folder overlay was unwanted and is removed. */
 const char *name=t->slot==EH_FILES_SLOT?"Files":r->slots[t->slot].label;
 int n=(int)strlen(name);
 if(n>10)n=10;
 eh_small_text(f,t->center-n*3,y+50,name,p->ink,1,10);
}
void eh7_render(uint16_t *f,const EhUI7 *s,const EhRegistry *r){
 if(!f||!s||!r)return;
 EhPalette p=eh_palette(r->appearance);
 if(!decode34(f))eh_box(f,0,0,320,240,DEEP);
 /* Slim wordmark replaces the oversize E cube + second title row. */
 /* Opaque chrome hides the image's baked-in former logo and 96% battery. */
 eh_box(f,0,0,320,43,DEEP);
 eh_box(f,0,41,320,1,SKY);
 eh_box(f,0,42,320,1,EH_RGB(23,96,154));
 eh_small_text(f,12,14,"ENDRAHOME",INK,2,9);
 /* The full-color artwork already carries the Explore / Organize motto. */
 eh_small_text(f,216,10,"TI-NSPIRE",EH_RGB(142,214,250),1,10);
 eh_small_text(f,267,26,"RUN 034",SKY,1,7);
 EhTile tiles[EH7_SLOT_COUNT];
 int count=eh_desktop_layout(r,tiles);
 /* Keep the real artwork visible between tiles; no full rectangular
  * overlay covering the centerpiece. */
 if(count==1)glass34(f,117,94,86,84,DEEP,22);
 for(int i=0;i<count;i++)draw_tile34(f,&tiles[i],r,&p,tiles[i].slot==s->selected);
 eh_box(f,0,217,320,23,DEEP);
 eh_box(f,0,217,320,1,SKY);
 eh_small_text(f,10,227,"ESC  BACK   ENTER  OPEN   MENU  MANAGE",INK,1,48);
}
