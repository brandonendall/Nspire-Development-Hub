/* Run016 dark manager with native QWERTY text-entry field. */
#include "manager.h"
#include <stdint.h>
#include <string.h>
#include "display.h"
#define box eh_box
static void txt(uint16_t *f,int x,int y,const char *s,uint16_t ink,int n){
 eh_text(f,x,y,s,ink,n,1);
}
static void tail(uint16_t *f,int x,int y,const char *s,uint16_t c,int n){
 int len=(int)strlen(s);if(len>n)s+=len-n;
 txt(f,x,y,s,c,n);
}
void eh15_manager_render(uint16_t *f,const EhUniversalUI *s){
 if(!f||!s||!s->apps)return;
 const EhAppSlot *app=&s->apps->slots[s->page.selected];
 int settings = s->page.page==EH_PAGE_HELP && s->subview==1;
 EhPalette p=eh_palette(settings?s->preview:s->apps->appearance);
 uint16_t white=p.ink,muted=p.ink,cyan=p.ink;
 box(f,0,0,320,240,p.background);
 box(f,0,0,320,33,p.raised);box(f,0,31,320,2,p.accent);
 txt(f,12,11,"ENDRAHOME",p.ink,10);
 tail(f,144,11,app->label,p.ink,19);
 box(f,11,41,298,169,p.panel);
 if(s->renaming){
  txt(f,19,50,"RENAME WITH QWERTY",cyan,24);
  txt(f,19,78,"CURRENT NAME:",muted,23);
  txt(f,19,95,app->label,white,15);
  box(f,16,119,288,35,p.background);
  box(f,16,119,288,2,p.accent);
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
   if(s->manager_item==i)box(f,15,y-3,287,21,p.selected);
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
  static const char *const rows[]={"Interface","Folders","Text","Restore defaults"};
  const char *values[]={eh_theme_names[s->preview.theme],eh_folder_names[s->preview.folder],
                        eh_font_names[s->preview.font],""};
  uint16_t chips[]={p.accent,p.folder,p.ink,p.panel};
  txt(f,19,50,"APPEARANCE",p.ink,10);
  for(int i=0;i<4;++i){
   int y=73+i*27;
   if(i==s->setting_row){
    box(f,15,y-5,287,22,p.selected);
    box(f,15,y-5,3,22,p.accent);
   }
   txt(f,26,y,rows[i],p.ink,18);
   if(i<3){
    box(f,166,y-1,12,10,chips[i]);
    txt(f,187,y,values[i],p.ink,11);
   }
  }
  txt(f,19,188,"Left/Right  Preview",p.ink,25);
  txt(f,13,225,"Enter  Save       Esc  Cancel",p.ink,35);
 }
}
