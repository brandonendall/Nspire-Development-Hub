/* Run033: EndraZIP-inspired crisp native RGB565 Files layout. */
#include "picker.h"
#include "display.h"
#include "render.h"
#include "art.h"
#include "file_icons.h"
#include "files.h"
#include <stdio.h>
#include <string.h>
#define box eh_box
static void label(uint16_t *f,int x,int y,const char *s,uint16_t ink,int n){
 eh_text(f,x,y,s,ink,n,1);
}

static void size_label(char *out,size_t cap,unsigned long long n){
 const char *units[]={"B","KB","MB","GB","TB"};
 unsigned unit=0;unsigned long long whole=n,rem=0;
 while(whole>=1024&&unit<4){rem=whole%1024;whole/=1024;unit++;}
 if(unit==0)snprintf(out,cap,"%lu B",(unsigned long)whole);
 else{
  unsigned tenth=(unsigned)((rem*10+512)/1024);
  if(tenth==10){whole++;tenth=0;}
  if(tenth)snprintf(out,cap,"%lu.%u %s",(unsigned long)whole,tenth,units[unit]);
  else snprintf(out,cap,"%lu %s",(unsigned long)whole,units[unit]);
 }
}
/* Run036: neon glass File Browser over the Run031 wallpaper.
 * Native directory enumeration, file icons, navigation and operations stay
 * unchanged. Dark alpha panels are calculated in the framebuffer only. */
void eh13_picker_render(uint16_t *f,const Eh13Picker *p,const EhRegistry *apps){
 if(!f||!p||!p->active||!apps)return;
 EhPalette colors=eh_palette(apps->appearance);
 const uint16_t cyan=eh_skin(&colors,EH_RGB(52,219,253)),pale=colors.ink;
 const uint16_t steel=eh_skin(&colors,EH_RGB(126,210,247)),line=eh_skin(&colors,EH_RGB(23,111,169));
 eh_wallpaper_paint(f,apps->appearance);
 /* Match the Files title to the browser label. */
 eh_box(f,0,0,320,37,eh_skin(&colors,EH_RGB(5,16,31)));
 eh_box(f,0,35,320,2,cyan);
 eh_small_text(f,11,p->show_all?15:10,p->show_all==2?"ZIP CONTENTS":p->show_all?"ENDRA FILES":p->adding_app?"ADD APP":"REASSIGN APP",pale,p->show_all?1:2,12);
 eh_small_text(f,255,p->show_all?15:21,p->show_all?"BROWSER":"PICKER",steel,1,12);
 eh_theme_glass(f,7,41,306,28,88,&colors);
 eh_box(f,7,41,306,1,cyan);
 eh_box(f,7,68,306,1,cyan);
 eh_small_text(f,13,51,"PATH",cyan,1,5);
 const char *shown=p->dir[0]?p->dir:"/";
 char abbreviated[31];
 size_t len=strlen(shown);
 if(len>30){
  snprintf(abbreviated,sizeof(abbreviated),"...%s",shown+len-27);
  shown=abbreviated;
 }
 label(f,54,49,shown,pale,30);
 eh_theme_glass(f,7,73,306,143,92,&colors);
 eh_box(f,7,73,306,1,line);
 eh_box(f,7,214,306,1,line);
 eh_small_text(f,38,77,"NAME",steel,1,5);
 if(p->show_all)eh_small_text(f,274,77,"SIZE",steel,1,5);
 eh_box(f,12,89,296,1,line);
 int bottom=p->scroll+EH13_VIEW_ROWS;
 if(bottom>p->count)bottom=p->count;
 for(int i=p->scroll;i<bottom;i++){
  int y=96+(i-p->scroll)*15;
  if(i==p->selected){
   eh_theme_glass(f,10,y-3,299,16,63,&colors);
   eh_box(f,10,y-3,3,16,eh_skin(&colors,EH_RGB(139,247,255)));
   eh_box(f,13,y+12,294,1,cyan);
  }else{
   eh_box(f,13,y+12,294,1,eh_skin(&colors,EH_RGB(13,54,84)));
  }
  const Eh13PickEntry *e=p->items+i;
  eh_file_kind_icon(f,17,y-3,eh_file_kind(e->name,e->directory),&colors);
  eh_fit_text(f,39,y,e->name,pale,p->show_all?22:31);
  if(p->show_all){
   char value[24];
   if(e->directory)snprintf(value,sizeof(value),"--");
   else size_label(value,sizeof(value),e->bytes);
   int n=(int)strlen(value);
   label(f,312-n*8,y,value,pale,n);
  }
 }
 if(!p->count)eh_small_text(f,18,103,p->show_all?"EMPTY FOLDER":"NO APPS HERE",pale,1,30);
 /* Files actions remain discoverable; the desktop alone hides its footer.
  * This narrow glass strip follows the selected Neon File Browser concept. */
 eh_theme_glass(f,0,219,320,21,91,&colors);
 eh_box(f,0,218,320,1,cyan);
 eh_small_text(f,9,227,p->show_all==2?"ENTER OPEN FOLDER   ESC BACK":p->show_all?"ENTER OPEN / INFO   ESC BACK   MENU ACTIONS":"ENTER SELECT APP   ESC BACK",pale,1,50);
}
