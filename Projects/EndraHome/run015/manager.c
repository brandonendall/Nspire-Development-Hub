/* Native dark manager: no SDL or white nClock compatibility overlay. */
#include "manager.h"
#include <stdint.h>
#include <string.h>
extern char font8x8_basic[128][8];
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void box(uint16_t *f,int x,int y,int w,int h,uint16_t c){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=320||y>=240||w<=0||h<=0)return;
 if(x+w>320)w=320-x;
 if(y+h>240)h=240-y;
 for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)f[j*320+i]=c;
}
static void txt(uint16_t *f,int x,int y,const char *s,uint16_t c,int n){
 for(int k=0;s&&s[k]&&k<n;k++){
  unsigned ch=(unsigned char)s[k];if(ch>127)ch='?';
  for(int yy=0;yy<8;yy++){
   unsigned b=(unsigned char)font8x8_basic[ch][yy];
   for(int xx=0;xx<8;xx++)if(b&(1u<<xx))box(f,x+k*8+xx,y+yy,1,1,c);
  }
 }
}
static void tail(uint16_t *f,int x,int y,const char *s,uint16_t c,int n){
 int len=(int)strlen(s);if(len>n)s+=len-n;
 txt(f,x,y,s,c,n);
}
void eh15_manager_render(uint16_t *f,const EhUniversalUI *s){
 if(!f||!s||!s->apps)return;
 const EhAppSlot *app=&s->apps->slots[s->page.selected];
 const uint16_t white=RGB(239,246,255),muted=RGB(150,175,200),cyan=RGB(79,219,251);
 box(f,0,0,320,240,RGB(9,16,30));
 box(f,0,0,320,33,RGB(29,43,64));box(f,0,31,320,2,RGB(38,125,200));
 txt(f,12,11,"ENDRAHOME",white,10);
 tail(f,144,11,app->label,cyan,19);
 box(f,11,41,298,169,RGB(23,39,58));
 if(s->renaming){
  txt(f,19,48,"RENAME SHORTCUT",cyan,18);
  txt(f,19,64,"CURRENT:",muted,9);txt(f,101,64,app->label,white,15);
  box(f,17,78,286,21,RGB(12,30,48));
  txt(f,22,84,s->rename_input[0]?s->rename_input:"(NEW NAME)",s->rename_input[0]?white:muted,15);
  for(int i=0;i<42;i++){
   int x=16+(i%7)*41,y=104+(i/7)*17;
   if(i==s->rename_cursor)box(f,x-2,y-2,40,16,RGB(39,115,153));
   if(i==40)txt(f,x,y,"DEL",cyan,4);
   else if(i==41)txt(f,x,y,"SAVE",cyan,5);
   else{char c[2]={eh15_palette[i],0};txt(f,x+12,y,c,white,1);}
  }
  txt(f,8,226,"ENTER ADD   MENU SAVE   ESC BACK",cyan,38);
  return;
 }
 if(s->confirm_remove){
  txt(f,23,68,"REMOVE SHORTCUT?",cyan,25);
  txt(f,23,96,app->label,white,15);
  txt(f,23,123,"ONLY THE LINK WILL BE REMOVED",muted,35);
  txt(f,23,146,"THE APPLICATION FILE IS SAFE",muted,34);
  txt(f,12,226,"ENTER REMOVE   ESC CANCEL",cyan,35);
  return;
 }
 if(s->page.page==EH_PAGE_DETAIL){
  static const char *items[5]={"ASSIGN / CHANGE APP","RENAME SHORTCUT",
   "REMOVE SHORTCUT","APP INFORMATION","SETTINGS"};
  txt(f,19,47,"MANAGE SHORTCUT",cyan,20);
  for(int i=0;i<5;i++){
   int y=68+i*27;
   if(s->manager_item==i)box(f,15,y-4,287,24,RGB(31,99,139));
   txt(f,21,y,s->manager_item==i?">":" ",cyan,1);
   txt(f,37,y,items[i],white,30);
  }
  txt(f,10,226,"UP/DOWN ENTER SELECT ESC BACK",cyan,38);
 }else if(s->page.page==EH_PAGE_HELP&&!s->subview){
  txt(f,19,51,"APPLICATION INFORMATION",cyan,28);
  txt(f,19,79,"SHORTCUT:",muted,14);txt(f,116,79,app->label,white,15);
  txt(f,19,105,app->path[0]?"LINKED FILE:":"NOT CONFIGURED",cyan,22);
  if(app->path[0]){
   txt(f,19,127,app->path,white,35);
   if(strlen(app->path)>35)txt(f,19,148,app->path+35,muted,35);
  }
  txt(f,19,181,"UNASSIGN NEVER DELETES FILES",muted,35);
  txt(f,14,226,"ESC BACK TO MANAGER",cyan,30);
 }else{
  txt(f,19,51,"ENDRAHOME SETTINGS",cyan,26);
  txt(f,19,80,"7 configurable shortcuts",white,31);
  txt(f,19,105,"Fast exit: ON",white,27);
  txt(f,19,128,"Return to Home: ON",white,27);
  txt(f,19,157,"Clock HUD: future feature",cyan,33);
  txt(f,19,177,"No nClock hooks used yet",muted,33);
  txt(f,14,226,"ESC BACK TO MANAGER",cyan,30);
 }
}
