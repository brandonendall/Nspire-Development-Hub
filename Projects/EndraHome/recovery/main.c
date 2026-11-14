/*
 * EndraHome Run002 Safe Diagnostic.
 * No SDL, no nl_exec, no filesystem I/O, no hooks, no settings mutation.
 * A deliberately time-limited input/display test for TI-Nspire CX II CAS.
 */
#include <libndls.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "font8x8_basic.h"
#include "state.h"

#define SW 320
#define SH 240
#define RGB(r,g,b) (uint16_t)((((r)&255)>>3)<<11|((((g)&255)>>2)<<5)|(((b)&255)>>3))
#define TICK_MS 25
#define MAX_TICKS 1200 /* roughly 30 seconds of polling before exit */
static uint16_t fb[SW*SH];
static EhState st;
static const char *labels[]={"ENDRAGBA","FILES","ZIP","APPS"};
/* Some newlib toolchain variants reference the legacy destructor hook. */
void __attribute__((weak)) _fini(void) {}

static void box(int x,int y,int w,int h,uint16_t color){
  if(x<0){w+=x;x=0;}
  if(y<0){h+=y;y=0;}
  if(x>=SW||y>=SH||w<=0||h<=0)return;
  if(x+w>SW)w=SW-x;
  if(y+h>SH)h=SH-y;
  for(int row=0;row<h;row++)
    for(int col=0;col<w;col++)
      fb[(y+row)*SW+x+col]=color;
}
static void glyph(int x,int y,unsigned char c,uint16_t color){
  if(c>=128)c='?';
  for(int row=0;row<8;row++){
    unsigned char bits=(unsigned char)font8x8_basic[c][row];
    for(int col=0;col<8;col++)
      if(bits&(1u<<col))box(x+col,y+row,1,1,color);
  }
}
static void line(int x,int y,const char *str,int max,uint16_t color){
  for(int i=0;str[i]&&i<max;i++)glyph(x+8*i,y,(unsigned char)str[i],color);
}
static void render(int ticks){
  const uint16_t bg=RGB(11,20,33),p=RGB(23,41,58),cyan=RGB(62,215,211),
       white=RGB(236,243,249),dim=RGB(145,166,184),active=RGB(33,78,94);
  box(0,0,SW,SH,bg);
  box(0,0,SW,30,p);
  box(0,0,5,30,cyan);
  line(14,10,"ENDRA HOME   /   SAFE 002",30,white);
  if(st.help){
    box(12,43,296,166,p);
    line(24,55,"SAFE INPUT DIAGNOSTIC",30,cyan);
    line(24,78,"NO SDL  /  NO APP LAUNCH",32,white);
    line(24,100,"MENU OR ENTER = BACK",32,white);
    line(24,118,"ESC = EXIT HOME",32,white);
    line(24,140,"NO FILES OR CONFIG USED",32,dim);
    line(24,165,"AUTO EXIT AFTER 30 SEC",32,dim);
  }else{
    box(12,44,296,63,p);
    line(24,56,"TI-NSPIRE CX II CAS",32,cyan);
    line(24,78,"Keyboard + screen safety test",36,white);
    line(14,114,"TEST YOUR NAVIGATION",32,cyan);
    for(int i=0;i<4;i++){
      int x=14+i*76,sel=(i==st.selected);
      box(x,133,70,69,sel?active:p);
      if(sel){box(x,133,70,2,cyan);box(x,200,70,2,cyan);}
      box(x+20,144,30,22,RGB(60+i*25,135+i*18,187-i*14));
      line(x+12,175,labels[i],6,sel?cyan:white);
      if(sel)box(x+31,194,8,3,cyan);
    }
    if(st.last_action)
      line(12,210,"ENTER REGISTERED - LAUNCH OFF",37,cyan);
    else
      line(12,210,"ARROWS MOVE | MENU HELP",37,dim);
  }
  box(0,222,320,18,p);
  char footer[42];
  int left=(MAX_TICKS-ticks+39)/40;
  if(left<0)left=0;
  snprintf(footer,sizeof(footer),"ESC EXIT | AUTO-EXIT %02ds",left);
  line(7,228,footer,38,dim);
  lcd_blit(fb,SCR_320x240_565);
}
static int scan(void){
  if(isKeyPressed(KEY_NSPIRE_ESC))return EH_ESC;
  if(isKeyPressed(KEY_NSPIRE_MENU))return EH_MENU;
  if(isKeyPressed(KEY_NSPIRE_ENTER)||isKeyPressed(KEY_NSPIRE_RET)||
     isKeyPressed(KEY_NSPIRE_CLICK))return EH_ENTER;
  if(isKeyPressed(KEY_NSPIRE_LEFT)||isKeyPressed(KEY_NSPIRE_UP))return EH_LEFT;
  if(isKeyPressed(KEY_NSPIRE_RIGHT)||isKeyPressed(KEY_NSPIRE_DOWN))return EH_RIGHT;
  return EH_NONE;
}
int main(void){
  eh_state_init(&st);
  if(!lcd_init(SCR_320x240_565))return 1;
  int last_second=-1;
  for(int ticks=0;ticks<MAX_TICKS && !st.quit;ticks++){
    int changed=eh_state_input(&st,scan());
    int sec=ticks/40;
    if(changed||sec!=last_second){
      render(ticks);
      last_second=sec;
    }
    if(st.quit)break;
    msleep(TICK_MS);
  }
  lcd_init(SCR_TYPE_INVALID);
  refresh_osscr();
  return 0;
}
