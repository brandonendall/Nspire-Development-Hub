/* Run033: interactive launcher drawn over the Run031 art that succeeded on hardware.
 * The original screenshot has a fixed battery reading: replace its top/bottom
 * with live UI, never pass a static estimate off as calculator telemetry.
 * No platform/OS changes; reuse the already working native framebuffer path.
 */
#include "render.h"
#include "caption.h"
#include "desktop.h"
#include "display.h"
#include "asset031.h"
#include "background_asset.h"
#include <stddef.h>
#include <string.h>
static uint16_t blend33(uint16_t a,uint16_t b,unsigned t) {
 unsigned u=100-t;
 return (uint16_t)(((((a>>11)*u+(b>>11)*t)/100)<<11) |
    (((((a>>5)&63)*u+((b>>5)&63)*t)/100)<<5) |
    (((a&31)*u+(b&31)*t)/100));
}
static int decode33(uint16_t *f) {
 size_t src=0,dst=0;
 while(src<ENDRA_BG_WORDS && dst<320u*240u) {
  uint16_t tag=endra_bg_rle[src++];
  size_t n=(tag&127u)+1u;
  if(n>320u*240u-dst)return 0;
  if(tag&0x8000u) {
   if(src>=ENDRA_BG_WORDS)return 0;
   uint16_t color=endra_bg_rle[src++];
   for(size_t i=0;i<n;i++)f[dst++]=color;
  } else {
   if(n>ENDRA_BG_WORDS-src)return 0;
   for(size_t i=0;i<n;i++)f[dst++]=endra_bg_rle[src++];
  }
 }
 return dst==320u*240u;
}
static void glass33(uint16_t *f,int x,int y,int w,int h,uint16_t color,unsigned opacity) {
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x+w>320)w=320-x;if(y+h>240)h=240-y;
 if(w<=0||h<=0)return;
 for(int row=y;row<y+h;row++)
  for(int col=x;col<x+w;col++){
   size_t at=(size_t)row*320+col;
   f[at]=blend33(f[at],color,opacity);
  }
}
static void tile33(uint16_t *f,const EhTile *tile,const EhRegistry *apps,
                   const EhPalette *p,int selected) {
 int x=tile->center-35,y=tile->top;
 uint16_t cyan=EH_RGB(54,218,254),outline=EH_RGB(36,86,122);
 /* An identical dark panel for every tile: the highlight can never wash
    out the icon, text or artwork behind it as in the Run032 photo. */
 glass33(f,x-2,y-2,74,69,EH_RGB(1,8,21),73);
 eh_box(f,x-1,y-1,72,67,selected?cyan:outline);
 eh_box(f,x+1,y+1,68,63,EH_RGB(7,19,35));
 if(selected) {
  eh_box(f,x+2,y+2,66,2,EH_RGB(115,241,255));
  eh_box(f,x+2,y+61,66,2,cyan);
  eh_box(f,x+3,y+3,2,56,cyan);
 }
 eh_app_icon(f,tile->center-16,y+5,2);
 if(tile->slot==EH_FILES_SLOT)eh_folder_icon(f,x+6,y+6,1,p);
 const char *caption=tile->slot==EH_FILES_SLOT?"Files":apps->slots[tile->slot].label;
 char first[EH7_LABEL_CAP],second[EH7_LABEL_CAP];
 int n=(int)strlen(caption);
 if(n<=8)
  eh_text(f,tile->center-n*4,y+45,caption,p->ink,8,1);
 else {
  eh_caption(caption,8,first,second);
  eh_text(f,tile->center-(int)strlen(first)*4,y+40,first,p->ink,8,1);
  eh_text(f,tile->center-(int)strlen(second)*4,y+51,second,p->ink,8,1);
 }
}
void eh7_render(uint16_t *f,const EhUI7 *ui,const EhRegistry *apps) {
 if(!f||!ui||!apps)return;
 EhPalette p=eh_palette(apps->appearance);
 if(!decode33(f))eh_box(f,0,0,320,240,p.background);
 EhTile tiles[EH7_SLOT_COUNT];
 int count=eh_desktop_layout(apps,tiles);
 if(count>1) {
  glass33(f,7,64,306,148,EH_RGB(1,8,20),76);
  eh_box(f,7,64,306,1,EH_RGB(20,91,143));
  eh_box(f,7,211,306,1,EH_RGB(20,91,143));
 } else {
  glass33(f,100,78,120,107,EH_RGB(1,8,20),32);
 }
 /* Hide static '96%' and Run031 control labels embedded in the art. */
 eh_box(f,0,0,320,51,EH_RGB(6,17,31));
 eh_box(f,0,49,320,2,p.accent);
 eh031_cube(f,10,5);
 eh_text(f,57,9,"EndraHome",p.ink,9,1);
 eh_text(f,57,27,"TI-Nspire Edition",EH_RGB(139,212,246),18,1);
 eh_text(f,272,18,"R033",p.accent,4,1);
 for(int i=0;i<count;i++)tile33(f,&tiles[i],apps,&p,tiles[i].slot==ui->selected);
 eh_box(f,0,214,320,26,EH_RGB(6,17,31));
 eh_box(f,0,213,320,1,p.accent);
 eh_text(f,8,223,"Esc Back",p.ink,8,1);
 eh_text(f,108,223,"Enter Open",p.ink,10,1);
 eh_text(f,224,223,"Menu Apps",p.ink,9,1);
}
