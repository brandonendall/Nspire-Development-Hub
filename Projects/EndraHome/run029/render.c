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
/* Run036 Hero style: the wallpaper remains exposed between, around and
 * INSIDE each icon. The only selection indicator belongs to its caption. */
void eh_wallpaper_paint(uint16_t *f) {
 if(!f)return;
 if(!decode34(f))eh_box(f,0,0,320,240,DEEP);
 restore_wallpaper35(f);
}
void eh_glass_panel(uint16_t *f,int x,int y,int w,int h,unsigned opacity) {
 if(!f)return;
 if(opacity>100)opacity=100;
 glass34(f,x,y,w,h,DEEP,opacity);
}
static void icon_pixel36(uint16_t *f,int x,int y,uint16_t color) {
 if((unsigned)x<320u && (unsigned)y<240u)f[(size_t)y*320+(unsigned)x]=color;
}
static void icon_line36(uint16_t *f,int ax,int ay,int bx,int by,uint16_t c) {
 int dx=bx>ax?bx-ax:ax-bx,dy=by>ay?by-ay:ay-by;
 int sx=ax<bx?1:-1,sy=ay<by?1:-1,err=dx-dy;
 for(;;) {
  icon_pixel36(f,ax,ay,c);
  if(ax==bx&&ay==by)break;
  int two=2*err;
  if(two>-dy){err-=dy;ax+=sx;}
  if(two<dx){err+=dx;ay+=sy;}
 }
}
static void hero_e36(uint16_t *f,int cx,int y){
 const int x=cx-18;
 const uint16_t ice=EH_RGB(147,241,255),edge=EH_RGB(25,168,231);
 const uint16_t silver=EH_RGB(230,250,255),steel=EH_RGB(71,177,236);
 /* Draw a miniature 3D E cube with no opaque background square. The side
  * and top are faintly tinted GLASS; original wallpaper is visible through
  * the front face and interior of the E. All pixels bounded in the frame. */
 eh_glass_panel(f,x+7,y+7,27,25,27);
 eh_glass_panel(f,x+1,y+5,6,25,32);
 icon_line36(f,x+1,y+5,x+8,y,ice);
 icon_line36(f,x+8,y,x+34,y+1,EH_RGB(74,190,255));
 icon_line36(f,x+34,y+1,x+34,y+28,ice);
 icon_line36(f,x+34,y+28,x+28,y+33,edge);
 icon_line36(f,x+28,y+33,x+7,y+33,ice);
 icon_line36(f,x+7,y+33,x+1,y+28,edge);
 icon_line36(f,x+1,y+28,x+1,y+5,edge);
 /* 3D ridge: front face begins where the sloping left side ends. */
 icon_line36(f,x+7,y+8,x+7,y+32,EH_RGB(62,205,253));
 icon_line36(f,x+7,y+8,x+34,y+1,EH_RGB(64,170,229));
 icon_line36(f,x+7,y+8,x+28,y+8,EH_RGB(38,126,192));
 icon_line36(f,x+2,y+6,x+6,y+9,EH_RGB(140,236,255));
 /* Beveled icy E (not a filled 32x32 flat tile). */
 eh_box(f,x+13,y+12,4,17,steel);
 eh_box(f,x+14,y+12,16,3,silver);
 eh_box(f,x+14,y+18,13,3,EH_RGB(143,225,255));
 eh_box(f,x+14,y+26,16,3,silver);
 icon_line36(f,x+17,y+15,x+28,y+15,EH_RGB(20,131,223));
 icon_line36(f,x+17,y+21,x+25,y+21,EH_RGB(23,122,213));
 icon_line36(f,x+17,y+29,x+29,y+29,EH_RGB(28,145,234));
 icon_pixel36(f,x+32,y+3,EH_RGB(244,252,255));
}
static void draw_tile36(uint16_t *f,const EhTile *tile,const EhRegistry *apps,
                        const EhPalette *p,int selected){
 int y=tile->top,cx=tile->center;
 hero_e36(f,cx,y);
 const char *name=tile->slot==EH_FILES_SLOT?"Files":apps->slots[tile->slot].label;
 int width=(int)strlen(name);
 if(width>10)width=10;
 const int text_x=cx-width*3, text_y=y+43;
 /* Only the LABEL reacts to navigation; the E never gets a tile highlight. */
 if(selected){
  eh_glass_panel(f,text_x-5,text_y-3,width*6+10,14,38);
  eh_small_text(f,text_x+1,text_y+1,name,EH_RGB(19,90,139),1,10);
  eh_small_text(f,text_x,text_y,name,EH_RGB(203,255,255),1,10);
  eh_box(f,text_x-2,text_y+10,width*6+4,1,EH_RGB(109,243,255));
  eh_box(f,text_x+2,text_y+12,width*6-4,1,EH_RGB(23,128,206));
 } else {
  eh_small_text(f,text_x+1,text_y+1,name,EH_RGB(5,22,45),1,10);
  eh_small_text(f,text_x,text_y,name,p->ink,1,10);
 }
}
void eh7_render(uint16_t *f,const EhUI7 *s,const EhRegistry *r) {
 if(!f||!s||!r)return;
 EhPalette p=eh_palette(r->appearance);
 eh_wallpaper_paint(f);
 EhTile tiles[EH7_SLOT_COUNT];
 int count=eh_desktop_layout(r,tiles);
 for(int i=0;i<count;i++)
  draw_tile36(f,&tiles[i],r,&p,tiles[i].slot==s->selected);
}
