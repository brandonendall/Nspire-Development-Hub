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
/* Run037: palette-aware glass and clean wallpaper under the original title. */
void eh_theme_glass(uint16_t *f,int x,int y,int w,int h,unsigned alpha,const EhPalette *p){
 if(f&&p)glass34(f,x,y,w,h,eh_skin(p,DEEP),alpha>100?100:alpha);
}
void eh_desktop_wallpaper(uint16_t *f,const EhPalette *p){
 eh_wallpaper_paint(f);eh_skin_frame(f,p);
 uint16_t soft[32];
 for(int i=0;i<32;i++)soft[i]=eh_skin(p,EH_RGB(3+i/4,9+i*3,22+i*6));
 for(int x=0;x<320;x++){
  int dx=x-160,curve=195-dx*dx/410,curve2=218-(x-260)*(x-260)/850,upper=52+x/6;
  for(int y=47;y<240;y++){
   int d=y-curve;if(d<0)d=-d;
   int e=y-curve2;if(e<0)e=-e;
   int q=y-upper;if(q<0)q=-q;
   int level=2+(y-47)/46;
   if(d<18)level+=(18-d)/2;
   if(d<3)level+=10;
   if(e<10)level+=(10-e)/2;
   if(q<14)level+=(14-q)/4;
   if((x<38||x>282)&&((x+y/2)%11==0||y%13==0))level+=2;
   if(level>31)level=31;
   f[y*320+x]=soft[level];
  }
 }
}
/* The actual shaded E cube, sampled from the original title at icon scale.
 * Mask its polygon; never fill the card or outline a wireframe. */
static void hero37(uint16_t *f,int cx,int y){
 for(int dy=0;dy<40;dy++)for(int dx=0;dx<40;dx++){
  int sx=11+dx*37/40,sy=3+dy*38/40;
  int top=sx<=33?13-(sx-11)*9/22:4+(sx-33)*6/14;
  int bottom=sx<=25?30+(sx-11)*10/14:40-(sx-25)*7/22;
  if(sy<top||sy>bottom||sx>47)continue;
  int px=cx-20+dx,py=y+dy;
  if((unsigned)px<320u&&(unsigned)py<240u)f[py*320+px]=f[sy*320+sx];
 }
}
static void draw_tile37(uint16_t *f,const EhTile *t,const EhRegistry *r,const EhPalette *p,int selected){
 int cx=t->center,y=t->top;hero37(f,cx,y);
 const char *name=t->slot==EH_FILES_SLOT?"Files":r->slots[t->slot].label;
 char label[13];size_t n=strlen(name);
 if(n>12){memcpy(label,name,9);memcpy(label+9,"...",4);n=12;}
 else {memcpy(label,name,n);label[n]=0;}
 int tx=cx-(int)n*3,ty=y+46;
 if(selected)eh_theme_glass(f,tx-4,ty-3,(int)n*6+8,15,55,p);
 eh_small_text(f,tx+1,ty+1,label,EH_RGB(2,5,10),1,12);
 /* Both states use the configured font colour; only decoration changes. */
 eh_small_text(f,tx,ty,label,p->ink,1,12);
 if(selected){
  eh_box(f,tx-2,ty+10,(int)n*6+4,1,eh_skin(p,EH_RGB(110,243,255)));
  eh_box(f,tx+2,ty+12,(int)n*6-4,1,eh_skin(p,EH_RGB(23,128,206)));
 }
}
void eh7_render(uint16_t *f,const EhUI7 *s,const EhRegistry *r){
 if(!f||!s||!r)return;
 EhPalette p=eh_palette(r->appearance);eh_desktop_wallpaper(f,&p);
 EhTile tiles[EH7_SLOT_COUNT];int count=eh_desktop_layout(r,tiles);
 for(int i=0;i<count;i++)draw_tile37(f,&tiles[i],r,&p,tiles[i].slot==s->selected);
}
