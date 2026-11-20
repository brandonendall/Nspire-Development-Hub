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
/* Run035: preserve the user-approved ORIGINAL illustrated header, including
 * its cube logo and "EndraHome / TI-Nspire Edition" lettering. Do not paint a
 * new header on top. Mask only the artwork's old fixed Run031/96% indicator:
 * reuse neutral background pixels to avoid representing a fake live battery.
 * The original wallpaper also embeds a bottom key legend. Replace only that
 * baked-in strip with a dim reflection of the 183..203 artwork, not a UI bar.
 * Input bindings and native display handling are completely unchanged. */
static void restore_wallpaper35(uint16_t *f) {
 for(int y=8;y<=26;y++) {
  uint16_t neutral=f[(size_t)y*320+211];
  for(int x=226;x<=317;x++) {
   unsigned strength=x<234?(unsigned)(x-226)*100u/8u:100u;
   size_t pos=(size_t)y*320+x;
   f[pos]=mix34(f[pos],neutral,strength);
  }
 }
 for(int y=205;y<240;y++) {
  int original=203-(y-205)*20/34;
  unsigned fade=(unsigned)(y-205)*55u/34u;
  for(int x=0;x<320;x++) {
   uint16_t reflected=f[(size_t)original*320+x];
   f[(size_t)y*320+x]=mix34(reflected,DEEP,fade);
  }
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
 /* Show the original illustrated logo/header. No duplicate text or title bar. */
 restore_wallpaper35(f);
 EhTile tiles[EH7_SLOT_COUNT];
 int count=eh_desktop_layout(r,tiles);
 /* Keep the real artwork visible between tiles; no full rectangular
  * overlay covering the centerpiece. */
 if(count==1)glass34(f,117,94,86,84,DEEP,22);
 for(int i=0;i<count;i++)draw_tile34(f,&tiles[i],r,&p,tiles[i].slot==s->selected);
 /* No instructional footer: Esc / Enter / Menu retain their native bindings. */
}
