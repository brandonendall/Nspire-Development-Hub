/* Run033: EndraZIP-inspired crisp native RGB565 Files layout. */
#include "picker.h"
#include "display.h"
#include "render.h"
#include "files.h"
#include <stdio.h>
#include <string.h>
#define box eh_box
static void label(uint16_t *f,int x,int y,const char *s,uint16_t ink,int n){
 eh_text(f,x,y,s,ink,n,1);
}

static int suffix(const char *name,const char *ext){
 size_t n=strlen(name),length=strlen(ext);
 if(n<length)return 0;
 const char *p=name+n-length;
 for(size_t i=0;i<length;++i){
  char c=p[i];
  if(c>='A' && c<='Z')c=(char)(c+32);
  if(c!=ext[i])return 0;
 }
 return 1;
}
static uint16_t mix565(uint16_t a,uint16_t b,unsigned percent){
 unsigned inv=100-percent;
 return (uint16_t)(((((a>>11)*inv+(b>>11)*percent)/100)<<11)|
 (((((a>>5)&63)*inv+((b>>5)&63)*percent)/100)<<5)|
 (((a&31)*inv+(b&31)*percent)/100));
}
static void file_icon(uint16_t *f,int x,int y,const char *name,int dir,const EhPalette *p){
 if(dir){eh_folder_icon(f,x,y,1,p);return;}
 int image=suffix(name,".jpg")||suffix(name,".png")||suffix(name,".jpeg")||
   suffix(name,".jpg.tns")||suffix(name,".jpeg.tns")||suffix(name,".png.tns");
 int archive=suffix(name,".zip")||suffix(name,".zip.tns")||suffix(name,".7z")||suffix(name,".rar");
 int config=suffix(name,".cfg")||suffix(name,".cfg.tns")||suffix(name,".ini")||suffix(name,".theme");
 int pdf=suffix(name,".pdf")||suffix(name,".pdf.tns");
 int app=suffix(name,".tns")&&!image&&!archive&&!config&&!pdf&&!suffix(name,".txt.tns");
 int text=eh17_is_text_name(name);
 uint16_t tone=archive?EH_RGB(64,206,131):
  image?mix565(p->folder,EH_RGB(60,205,240),55):
  config?mix565(p->folder,EH_RGB(170,150,230),65):
  pdf?mix565(p->folder,EH_RGB(245,70,70),65):
  text?mix565(p->folder,EH_RGB(210,225,245),70):p->accent;
 if(app){eh_app_icon(f,x,y,1);return;}
 box(f,x+2,y+1,12,14,tone);
 box(f,x+4,y+3,8,10,p->panel);
 if(archive){
  box(f,x+7,y+3,2,9,tone);
  for(int a=0;a<4;a++)box(f,x+7+(a&1),y+4+a*2,1,1,p->ink);
 }else if(image){
  box(f,x+5,y+5,6,6,tone);
  box(f,x+9,y+6,2,2,p->ink);
  box(f,x+6,y+9,4,2,p->ink);
 }else if(config){
  box(f,x+5,y+6,6,2,tone);
  box(f,x+7,y+4,2,6,tone);
  box(f,x+6,y+10,4,2,p->ink);
 }else if(pdf){
  box(f,x+4,y+5,8,2,tone);
  box(f,x+4,y+8,6,2,tone);
  box(f,x+4,y+11,8,1,tone);
 }else{
  box(f,x+4,y+5,8,1,tone);
  box(f,x+4,y+8,8,1,tone);
  box(f,x+4,y+11,6,1,tone);
 }
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
 const uint16_t cyan=EH_RGB(52,219,253),pale=EH_RGB(228,249,255);
 const uint16_t steel=EH_RGB(126,210,247),line=EH_RGB(23,111,169);
 eh_wallpaper_paint(f);
 eh_glass_panel(f,0,0,320,37,94);
 eh_box(f,0,35,320,2,cyan);
 eh_small_text(f,11,10,p->show_all?"ENDRA FILES":"ASSIGN APP",pale,2,12);
 eh_small_text(f,255,21,p->show_all?"BROWSER":"PICKER",steel,1,12);
 eh_glass_panel(f,7,41,306,28,88);
 eh_box(f,7,41,306,1,cyan);
 eh_box(f,7,68,306,1,cyan);
 eh_small_text(f,13,51,"PATH",cyan,1,5);
 const char *shown=p->dir;
 size_t len=strlen(shown);
 if(len>30)shown+=len-30;
 label(f,54,49,shown,pale,30);
 eh_glass_panel(f,7,73,306,143,88);
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
   eh_glass_panel(f,10,y-3,299,16,63);
   eh_box(f,10,y-3,3,16,EH_RGB(139,247,255));
   eh_box(f,13,y+12,294,1,cyan);
  }else{
   eh_box(f,13,y+12,294,1,EH_RGB(13,54,84));
  }
  const Eh13PickEntry *e=p->items+i;
  file_icon(f,17,y-3,e->name,e->directory,&colors);
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
 eh_glass_panel(f,0,219,320,21,91);
 eh_box(f,0,218,320,1,cyan);
 eh_small_text(f,9,227,p->show_all?"ENTER OPEN / INFO   ESC BACK   MENU ACTIONS":"ENTER SELECT APP   ESC BACK",pale,1,50);
}
