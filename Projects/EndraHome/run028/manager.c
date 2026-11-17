/* Run016 dark manager with native QWERTY text-entry field. */
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
static uint16_t accent(int theme){
 static const int p[7][3]={{79,219,251},{25,129,239},{126,141,161},{156,169,189},{238,80,93},{69,211,121},{178,109,239}};
 if(theme<0||theme>6)theme=0;
 return RGB(p[theme][0],p[theme][1],p[theme][2]);
}
static void tail(uint16_t *f,int x,int y,const char *s,uint16_t c,int n){
 int len=(int)strlen(s);if(len>n)s+=len-n;
 txt(f,x,y,s,c,n);
}
void eh15_manager_render(uint16_t *f,const EhUniversalUI *s){
 if(!f||!s||!s->apps)return;
 const EhAppSlot *app=&s->apps->slots[s->page.selected];
 const uint16_t white=RGB(239,246,255),muted=RGB(150,175,200),cyan=accent(s->apps->theme);
 box(f,0,0,320,240,RGB(9,16,30));
 box(f,0,0,320,33,RGB(29,43,64));box(f,0,31,320,2,cyan);
 txt(f,12,11,"ENDRAHOME",white,10);
 tail(f,144,11,app->label,cyan,19);
 box(f,11,41,298,169,RGB(23,39,58));
 if(s->renaming){
  txt(f,19,50,"RENAME WITH QWERTY",cyan,24);
  txt(f,19,78,"CURRENT NAME:",muted,23);
  txt(f,19,95,app->label,white,15);
  box(f,16,119,288,35,RGB(12,30,48));
  box(f,16,119,288,2,RGB(58,190,225));
  txt(f,22,131,s->rename_input[0]?s->rename_input:"TYPE NEW NAME...",s->rename_input[0]?white:muted,17);
  int len=(int)strlen(s->rename_input);
  if(len<15)box(f,22+len*8,143,7,2,cyan);
  txt(f,19,174,"USE YOUR PHYSICAL KEYBOARD",muted,33);
  txt(f,19,191,"MAXIMUM 15 CHARACTERS",muted,30);
  txt(f,9,225,"ENTER SAVE DEL DELETE ESC CANCEL",cyan,37);
  return;
 }
 if(s->confirm_remove){
  txt(f,23,68,s->page.selected==3?"USE BUILT-IN FILES?":"REMOVE SHORTCUT?",cyan,25);
  txt(f,23,96,app->label,white,15);
  txt(f,23,123,"ONLY THE LINK WILL BE REMOVED",muted,35);
  txt(f,23,146,"THE APPLICATION FILE IS SAFE",muted,34);
  txt(f,12,226,"ENTER REMOVE   ESC CANCEL",cyan,35);
  return;
 }
 if(s->page.page==EH_PAGE_DETAIL){
  static const char *items[6]={"ASSIGN / CHANGE APP","RENAME SHORTCUT",
   "REMOVE SHORTCUT","APP INFORMATION","SETTINGS","OPEN ENDRA FILES"};
  txt(f,19,45,"MANAGE APPS: LEFT/RIGHT SLOT",cyan,33);
  for(int i=0;i<6;i++){
   int y=63+i*23;
   if(s->manager_item==i)box(f,15,y-3,287,21,accent(s->apps->theme));
   txt(f,21,y,s->manager_item==i?">":" ",cyan,1);
   txt(f,37,y,(i==2&&s->page.selected==3)?"USE BUILT-IN FILES":items[i],white,30);
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
  static const char *themes[7]={"ORIGINAL","BLUE","BLACK","GRAY","RED","GREEN","PURPLE"};
  
  txt(f,19,51,"THEMES & APPEARANCE",cyan,26);
  txt(f,19,78,"CURRENT THEME:",muted,30);
  txt(f,161,78,themes[s->apps->theme>=0&&s->apps->theme<=6?s->apps->theme:0],white,15);
  txt(f,19,98,"PREVIEW:",muted,15);
  box(f,18,110,284,35,RGB(12,30,48));
  box(f,191,116,28,24,accent(s->preview_theme));
  box(f,195,120,20,16,RGB(241,248,255));
  txt(f,28,122,"<",cyan,1);
  txt(f,61,122,themes[s->preview_theme],white,15);
  txt(f,280,122,">",cyan,1);
  txt(f,19,165,"LEFT / RIGHT CHANGE COLOR",muted,34);
  txt(f,19,187,"ENTER APPLY  ESC CANCEL",cyan,34);
  txt(f,14,226,"ESC BACK TO MANAGER",cyan,30);
 }
}
