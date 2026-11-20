/* EndraHome Run034: cohesive slim native UI for app manager, dialogs and
 * appearance settings. Input and persistent settings are unchanged. */
#include "manager.h"
#include "display.h"
#include <string.h>
#define box eh_box
#define SKY EH_RGB(55,219,252)
#define INK EH_RGB(238,249,255)
#define MUTED EH_RGB(157,196,225)
#define DEEP EH_RGB(5,16,31)
#define PANEL EH_RGB(8,25,44)
#define ROW EH_RGB(14,52,78)
static void txt(uint16_t *f,int x,int y,const char *s,uint16_t c,int cap){
 eh_text(f,x,y,s,c,cap,1);
}
static const char *rightpart(const char *s,int cap){
 size_t n=strlen(s);
 return n>(size_t)cap?s+n-cap:s;
}
static void head34(uint16_t *f,const char *right){
 box(f,0,0,320,240,DEEP);
 box(f,0,0,320,42,EH_RGB(9,28,48));
 box(f,0,40,320,2,SKY);
 eh_small_text(f,12,10,"ENDRAHOME",INK,2,9);
 eh_small_text(f,13,30,"TI-NSPIRE EDITION",MUTED,1,18);
 if(right)eh_small_text(f,207,21,right,SKY,1,18);
 box(f,8,48,304,163,EH_RGB(28,76,105));
 box(f,9,49,302,161,PANEL);
 box(f,0,218,320,22,EH_RGB(9,28,48));
 box(f,0,217,320,1,SKY);
}
static void title34(uint16_t *f,const char *s){
 eh_small_text(f,18,56,s,SKY,2,23);
}
static void row34(uint16_t *f,int y,int selected){
 if(!selected)return;
 box(f,16,y-4,288,20,ROW);
 box(f,16,y-4,3,20,SKY);
 box(f,19,y-4,285,1,EH_RGB(20,100,143));
}
void eh15_manager_render(uint16_t *f,const EhUniversalUI *s){
 if(!f||!s||!s->apps)return;
 const EhAppSlot *app=&s->apps->slots[s->page.selected];
 int settings=s->page.page==EH_PAGE_HELP&&s->subview==1;
 EhPalette p=eh_palette(settings?s->preview:s->apps->appearance);
 uint16_t custom=p.ink;
 head34(f,settings?"SETTINGS":rightpart(app->label,17));
 if(s->renaming){
  title34(f,"RENAME SHORTCUT");
  txt(f,20,86,"CURRENT NAME:",MUTED,20);
  txt(f,144,86,rightpart(app->label,17),custom,17);
  box(f,18,116,284,36,DEEP);
  box(f,18,116,284,2,SKY);
  txt(f,24,130,s->rename_input[0]?s->rename_input:"TYPE A NAME",s->rename_input[0]?custom:MUTED,17);
  int n=(int)strlen(s->rename_input);
  if(n<16)box(f,24+n*8,140,2,12,SKY);
  eh_small_text(f,20,176,"USE THE PHYSICAL KEYBOARD",MUTED,1,40);
  eh_small_text(f,20,192,"15 CHARACTERS MAXIMUM",MUTED,1,40);
  eh_small_text(f,15,225,"ENTER SAVE      ESC CANCEL",INK,1,48);
  return;
 }
 if(s->confirm_remove){
  title34(f,s->page.selected==3?"USE BUILT-IN FILES?":"REMOVE SHORTCUT?");
  txt(f,20,105,rightpart(app->label,30),custom,30);
  eh_small_text(f,20,143,"THIS DOES NOT DELETE FILES",MUTED,1,46);
  eh_small_text(f,20,165,"ONLY THE SHORTCUT CHANGES",MUTED,1,46);
  eh_small_text(f,15,225,"ENTER CONFIRM      ESC CANCEL",INK,1,48);
  return;
 }
 if(s->page.page==EH_PAGE_DETAIL){
  static const char *items[]={"ASSIGN / CHANGE APP","RENAME SHORTCUT",
    "REMOVE SHORTCUT","APP INFORMATION","APPEARANCE","OPEN ENDRA FILES"};
  title34(f,"MANAGE APPLICATIONS");
  for(int i=0;i<6;i++){
   int y=83+i*20;
   row34(f,y,s->manager_item==i);
   txt(f,25,y,(i==2&&s->page.selected==3)?"USE BUILT-IN FILES":items[i],custom,34);
  }
  eh_small_text(f,15,225,"ARROWS SELECT   ENTER OPEN   ESC BACK",INK,1,48);
 }else if(s->page.page==EH_PAGE_HELP&&!s->subview){
  title34(f,"APPLICATION INFO");
  txt(f,20,85,"SHORTCUT:",MUTED,16);
  txt(f,108,85,rightpart(app->label,22),custom,22);
  txt(f,20,113,app->path[0]?"LINKED FILE:":"NOT CONFIGURED",SKY,23);
  if(app->path[0]){
   txt(f,20,132,app->path,custom,34);
   if(strlen(app->path)>34)txt(f,20,149,app->path+34,MUTED,34);
  }
  eh_small_text(f,19,189,"REMOVING LINK DOES NOT DELETE APP",MUTED,1,48);
  eh_small_text(f,15,225,"ESC BACK TO APP MANAGER",INK,1,48);
 }else{
  static const char *const choices[]={"Interface","Folders","Text","Restore defaults"};
  const char *values[]={eh_theme_names[s->preview.theme],eh_folder_names[s->preview.folder],
    eh_font_names[s->preview.font],""};
  const uint16_t chips[]={p.accent,p.folder,p.ink,p.panel};
  title34(f,"APPEARANCE");
  for(int i=0;i<4;i++){
   int y=88+i*24;
   row34(f,y,s->setting_row==i);
   txt(f,26,y,choices[i],custom,18);
   if(i<3){
    box(f,166,y-1,13,13,SKY);
    box(f,168,y+1,9,9,chips[i]);
    txt(f,186,y,values[i],custom,14);
   }
  }
  eh_small_text(f,21,194,"LEFT / RIGHT  PREVIEW COLOR",MUTED,1,46);
  eh_small_text(f,15,225,"ENTER SAVE      ESC CANCEL",INK,1,48);
 }
}
