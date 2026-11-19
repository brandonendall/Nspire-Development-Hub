/* EndraHome Run029 visual refresh: compact reusable RGB565 placeholder.
 * Pure drawing code; no files, heap, OS hooks, or hardware access.
 * The caller owns a 320x240 RGB565 framebuffer and may theme the border.
 */
#ifndef ENDRAHOME_UNIVERSAL_ICON_H
#define ENDRAHOME_UNIVERSAL_ICON_H
#include <stdint.h>
#define EH_ICON_RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static inline void eh_icon_rect(uint16_t *fb,int x,int y,int w,int h,uint16_t color){
 if(!fb)return;
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=320||y>=240||w<=0||h<=0)return;
 if(x+w>320)w=320-x;if(y+h>240)h=240-y;
 for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)fb[j*320+i]=color;
}
/* Deliberately generic E badge: identical for every app shortcut. */
static inline void eh_icon_placeholder(uint16_t *fb,int x,int y,int size,uint16_t accent,int selected){
 if(!fb||size<18||size>80)return;
 const uint16_t navy=EH_ICON_RGB(8,19,38);
 const uint16_t shade=EH_ICON_RGB(15,39,65);
 const uint16_t bright=EH_ICON_RGB(103,222,255);
 eh_icon_rect(fb,x,y,size,size,accent);
 eh_icon_rect(fb,x+2,y+2,size-4,size-4,navy);
 eh_icon_rect(fb,x+4,y+4,size-8,size-8,shade);
 /* Three bars and a spine form an E, scaled to the tile. */
 int unit=size/10;if(unit<2)unit=2;
 int left=x+size/4,top=y+size/4;
 int width=size/2,height=size/2;
 eh_icon_rect(fb,left,top,unit,height,bright);
 eh_icon_rect(fb,left,top,width,unit,bright);
 eh_icon_rect(fb,left,top+height/2-unit/2,width-unit,unit,bright);
 eh_icon_rect(fb,left,top+height-unit,width,unit,bright);
 if(selected){
  eh_icon_rect(fb,x-2,y-2,size+4,1,bright);
  eh_icon_rect(fb,x-2,y+size+1,size+4,1,bright);
  eh_icon_rect(fb,x-2,y-2,1,size+4,bright);
  eh_icon_rect(fb,x+size+1,y-2,1,size+4,bright);
 }
}
#endif
