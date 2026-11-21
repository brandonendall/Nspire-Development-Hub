/* EndraHome Run034: cohesive slim native UI for app manager, dialogs and
 * appearance settings. Input and persistent settings are unchanged. */
#include "manager.h"
#include "display.h"
#include "render.h"
#include "art.h"
#include <stdio.h>
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
static void head34(uint16_t *f,const char *right,EhAppearance appearance,const EhPalette *p){
 eh_wallpaper_paint(f,appearance);
 box(f,0,0,320,37,eh_skin(p,EH_RGB(5,16,31)));
 box(f,0,35,320,2,eh_skin(p,SKY));
 eh_small_text(f,11,10,"ENDRAHOME",p->ink,2,9);
 if(right)eh_small_text(f,201,22,right,eh_skin(p,MUTED),1,19);
 eh_theme_glass(f,8,43,304,169,93,p);
 box(f,8,43,304,1,eh_skin(p,SKY));
 box(f,8,211,304,1,eh_skin(p,SKY));
 box(f,8,43,1,169,eh_skin(p,SKY));
 box(f,311,43,1,169,eh_skin(p,SKY));
 eh_theme_glass(f,0,219,320,21,91,p);
 box(f,0,218,320,1,eh_skin(p,SKY));
}
static void title34(uint16_t *f,const char *s,const EhPalette *p){
 eh_small_text(f,18,51,s,eh_skin(p,SKY),2,23);
}
static void row34(uint16_t *f,int y,int selected,const EhPalette *p){
 if(!selected)return;
 eh_theme_glass(f,16,y-3,288,16,50,p);
 box(f,16,y-3,3,16,eh_skin(p,SKY));
 box(f,19,y+12,285,1,eh_skin(p,EH_RGB(20,100,143)));
}
void eh15_manager_render(uint16_t *f,const EhUniversalUI *s){
 if(!f||!s||!s->apps)return;
 const EhAppSlot *app=&s->apps->slots[s->page.selected];
 int settings=s->page.page==EH_PAGE_HELP&&s->subview==1;
 EhAppearance appearance=settings?s->preview:s->apps->appearance;
 EhPalette p=eh_palette(appearance);
 uint16_t custom=p.ink;
 head34(f,settings?"SETTINGS":s->subview==2&&s->page.page==EH_PAGE_HELP?"TI SETTINGS":rightpart(app->label,17),appearance,&p);
 if(s->renaming){
  title34(f,"RENAME SHORTCUT",&p);
  txt(f,20,86,"CURRENT NAME:",eh_skin(&p,MUTED),20);
  txt(f,144,86,rightpart(app->label,17),custom,17);
  box(f,18,116,284,36,eh_skin(&p,DEEP));
  box(f,18,116,284,2,eh_skin(&p,SKY));
  txt(f,24,130,s->rename_input[0]?s->rename_input:"TYPE A NAME",s->rename_input[0]?custom:eh_skin(&p,MUTED),17);
  int n=(int)strlen(s->rename_input);
  if(n<16)box(f,24+n*8,140,2,12,eh_skin(&p,SKY));
  eh_small_text(f,20,176,"USE THE PHYSICAL KEYBOARD",eh_skin(&p,MUTED),1,40);
  eh_small_text(f,20,192,"15 CHARACTERS MAXIMUM",eh_skin(&p,MUTED),1,40);
  eh_small_text(f,15,225,"ENTER SAVE      ESC CANCEL",p.ink,1,48);
  return;
 }
 if(s->confirm_remove){
  title34(f,s->page.selected==3?"USE BUILT-IN FILES?":"REMOVE SHORTCUT?",&p);
  txt(f,20,105,rightpart(app->label,30),custom,30);
  eh_small_text(f,20,143,"THIS DOES NOT DELETE FILES",eh_skin(&p,MUTED),1,46);
  eh_small_text(f,20,165,"ONLY THE SHORTCUT CHANGES",eh_skin(&p,MUTED),1,46);
  eh_small_text(f,15,225,"ENTER CONFIRM      ESC CANCEL",p.ink,1,48);
  return;
 }
 if(s->page.page==EH_PAGE_DETAIL){
  static const char *items[]={"ASSIGN / CHANGE APP","RENAME SHORTCUT",
    "REMOVE SHORTCUT","APP INFORMATION","APPEARANCE","OPEN ENDRA FILES","TI SETTINGS SHORTCUT"};
  title34(f,"MANAGE APPLICATIONS",&p);
  for(int i=0;i<EH15_MENU_COUNT;i++){
   int y=77+i*18;
   row34(f,y,s->manager_item==i,&p);
   eh_small_text(f,25,y,">",eh_skin(&p,SKY),1,1);
   txt(f,40,y,(i==2&&s->page.selected==3)?"USE BUILT-IN FILES":items[i],custom,32);
  }
  eh_small_text(f,15,225,"ARROWS SELECT   ENTER OPEN   ESC BACK",p.ink,1,48);
 }else if(s->page.page==EH_PAGE_HELP&&s->subview==2){
  title34(f,"NATIVE SHORTCUT",&p);
  txt(f,19,79,"HOME > 5 > ENDRAHOME",custom,34);
  txt(f,19,98,"STATUS: NOT INSTALLED",custom,34);
  eh_small_text(f,19,120,"READ-ONLY COMPATIBILITY CHECK",eh_skin(&p,SKY),1,45);
  const char *result=s->native_status==1?"STOCK MENU POINTER DETECTED":
    s->native_status==2?"MENU POINTER ALREADY MODIFIED":
    s->native_status==3?"UNSUPPORTED OS / NDLESS":"PRESS ENTER TO CHECK";
  eh_small_text(f,19,138,result,custom,1,46);
  eh_small_text(f,19,158,"PRESERVE EXISTING THEME EDITOR",eh_skin(&p,MUTED),1,45);
  eh_small_text(f,19,178,"NO OS WRITES OR HOOKS INSTALLED",custom,1,45);
  eh_small_text(f,15,225,"ENTER CHECK       ESC BACK",p.ink,1,48);
 }else if(s->page.page==EH_PAGE_HELP&&!s->subview){
  title34(f,"APPLICATION INFO",&p);
  txt(f,20,85,"SHORTCUT:",eh_skin(&p,MUTED),16);
  txt(f,108,85,rightpart(app->label,22),custom,22);
  txt(f,20,113,app->path[0]?"LINKED FILE:":"NOT CONFIGURED",eh_skin(&p,SKY),23);
  if(app->path[0]){
   txt(f,20,132,app->path,custom,34);
   if(strlen(app->path)>34)txt(f,20,149,app->path+34,eh_skin(&p,MUTED),34);
  }
  eh_small_text(f,19,189,"REMOVING LINK DOES NOT DELETE APP",eh_skin(&p,MUTED),1,48);
  eh_small_text(f,15,225,"ESC BACK TO APP MANAGER",p.ink,1,48);
 }else{
  static const char *const choices[]={"Interface","Folders","Text","Wallpaper","App Icon",
                                      "Restore defaults"};
  char wall[19], icon[19];
  snprintf(wall,sizeof(wall),"%u %s",appearance.wallpaper+1,
           eh_wallpaper_names[appearance.wallpaper]);
  snprintf(icon,sizeof(icon),"%u %s",appearance.icon+1,eh_icon_names[appearance.icon]);
  const char *values[]={eh_theme_names[appearance.theme],eh_folder_names[appearance.folder],
                        eh_font_names[appearance.font],wall,icon,""};
  title34(f,"APPEARANCE",&p);
  for(int i=0;i<=EH_LOOK_DEFAULTS;i++){
   int y=79+i*20;
   if(s->setting_row==i){
    eh_theme_glass(f,16,y-4,162,16,55,&p);
    box(f,16,y-4,2,16,eh_skin(&p,SKY));
   }
   eh_small_text(f,22,y,choices[i],custom,1,16);
   eh_small_text(f,83,y,values[i],eh_skin(&p,MUTED),1,15);
  }

  box(f,187,77,114,86,eh_skin(&p,SKY));
  eh_wallpaper_preview(f,appearance,188,78);
  eh_icon_paint(f,appearance,220,87);
  eh_icon_caption(f,229,141,"Files",&p);
  eh_folder_icon(f,200,173,1,&p);
  eh_icon_caption(f,226,178,"Folder",&p);
  eh_small_text(f,15,225,s->setting_row==EH_LOOK_DEFAULTS
                ?"ENTER PREVIEW DEFAULTS       ESC CANCEL"
                :"L / R PREVIEW   ENTER SAVE    ESC CANCEL",p.ink,1,48);
 }
}
