/*
 * EndraHome Run003: GBA-inspired navigation/graphics diagnostic.
 * Launching apps intentionally disabled until multi-screen + exit tests pass.
 */
#include "platform.h"
#include "logic.h"
#include <libndls.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "font8x8_basic.h"
#define RGB(r,g,b) (uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3))
static EhUI ui;
static const char *names[]={"ENDRAGBA","FILES","ZIP","APPS"};
static uint16_t *pixels;
void __attribute__((weak)) _fini(void) {}
static void fill(int x,int y,int w,int h,uint16_t color){
  if(x<0){w+=x;x=0;}
  if(y<0){h+=y;y=0;}
  if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
  if(x+w>EH_W)w=EH_W-x;
  if(y+h>EH_H)h=EH_H-y;
  for(int j=y;j<y+h;j++){
    uint16_t *row=pixels+j*EH_W;
    for(int i=x;i<x+w;i++)row[i]=color;
  }
}
static void text(int x,int y,const char *s,int max,uint16_t c){
  if(!s)return;
  for(int i=0;s[i]&&i<max;i++){
    unsigned char letter=(unsigned char)s[i];
    if(letter>127)letter='?';
    for(int row=0;row<8;row++){
      unsigned char glyph=font8x8_basic[letter][row];
      for(int col=0;col<8;col++)
        if(glyph&(1u<<col))fill(x+i*8+col,y+row,1,1,c);
    }
  }
}
static void card(int x,int y,int w,int h,uint16_t c,int selected){
  fill(x,y,w,h,c);
  if(selected){
    fill(x,y,w,2,RGB(61,227,213));
    fill(x,y+h-2,w,2,RGB(61,227,213));
    fill(x,y,2,h,RGB(61,227,213));
    fill(x+w-2,y,2,h,RGB(61,227,213));
  }
}
static void render(void){
  pixels=eh_framebuffer();
  uint16_t bg=RGB(9,17,29),panel=RGB(23,39,58),
      bright=RGB(227,238,246),dim=RGB(134,155,177),aqua=RGB(64,220,209);
  fill(0,0,320,240,bg);
  fill(0,0,320,31,panel);
  fill(0,0,4,31,aqua);
  text(12,10,"ENDRA",5,aqua);
  text(58,10,"HOME",4,bright);
  text(269,10,"003",3,dim);
  if(ui.page==EH_PAGE_HOME){
    card(12,42,296,70,panel,0);
    text(23,54,"YOUR CALCULATOR.",27,bright);
    text(23,71,"YOUR APPLICATIONS.",27,aqua);
    text(23,93,"GBA NAVIGATION ENGINE",27,dim);
    text(14,122,"LAUNCHER DOCK",20,aqua);
    for(int i=0;i<4;i++){
      int x=13+i*77,sel=(ui.selected==i);
      card(x,137,72,72,sel?RGB(26,60,73):panel,sel);
      fill(x+19,148,34,27,RGB(53+i*23,136+i*9,190-i*16));
      text(x+14,181,names[i],6,sel?aqua:bright);
      text(x+21,195,sel?"VIEW":"APP",4,dim);
    }
    text(7,216,"ARROWS SELECT | ENTER OPEN",38,dim);
  }else if(ui.page==EH_PAGE_DETAIL){
    card(12,43,296,169,panel,0);
    text(23,57,"APPLICATION DETAILS",34,aqua);
    text(23,89,names[ui.selected],30,bright);
    text(23,118,"STATUS: PREVIEW ONLY",34,bright);
    text(23,140,"NO APP LAUNCH IN RUN003",35,dim);
    text(23,170,"ENTER  MORE INFORMATION",35,aqua);
    text(23,191,"ESC BACK / MENU HOME",35,dim);
  }else if(ui.page==EH_PAGE_ABOUT){
    card(12,43,296,169,panel,0);
    text(23,57,"APP INFORMATION",34,aqua);
    text(23,87,names[ui.selected],34,bright);
    text(23,113,"GBA-STYLE VIDEO / INPUT",34,bright);
    text(23,137,"APP LAUNCH NOT ENABLED",34,dim);
    text(23,171,"ENTER OR ESC - BACK",34,aqua);
    text(23,191,"MENU - RETURN HOME",34,dim);
  }else{
    card(12,43,296,169,panel,0);
    text(23,57,"NAVIGATION HELP",34,aqua);
    text(23,85,"ARROWS   CHANGE ICON",34,bright);
    text(23,107,"ENTER    NEXT SCREEN",34,bright);
    text(23,129,"ESC      BACK OR EXIT",34,bright);
    text(23,151,"MENU     HOME / HELP",34,bright);
    text(23,181,"NO SDL / NO FILE ACCESS",34,dim);
    text(23,196,"NO APP LAUNCH IN RUN003",34,dim);
  }
  fill(0,225,320,15,panel);
  text(7,229,"ESC BACK/EXIT   MENU   ENTER",38,dim);
  eh_platform_flip();
}
int main(void){
  eh_ui_init(&ui);
  if(!eh_platform_start())return 1;
  render();
  while(!ui.should_exit){
    EhKey key=eh_platform_readkey();
    if(eh_ui_dispatch(&ui,key) && !ui.should_exit)render();
    if(!ui.should_exit)eh_platform_sleep();
  }
  eh_platform_stop();
  return 0;
}
