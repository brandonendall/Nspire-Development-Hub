/* Run033: EndraZIP-inspired crisp native RGB565 Files layout. */
#include "picker.h"
#include "display.h"
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
void eh13_picker_render(uint16_t *f,const Eh13Picker *p,const EhRegistry *apps){
 if(!f||!p||!p->active||!apps)return;
 EhPalette colors=eh_palette(apps->appearance);
 uint16_t bg=EH_RGB(6,16,29),light=colors.ink,dim=colors.ink,cyan=EH_RGB(52,217,251),highlight=EH_RGB(10,47,72);
 box(f,0,0,320,240,bg);
 box(f,0,0,320,29,EH_RGB(8,27,46));box(f,0,28,320,2,cyan);
 label(f,12,10,p->show_all?"ENDRA FILES":"ASSIGN APP",light,13);
 if(!p->show_all)label(f,118,10,apps->slots[p->slot].label,cyan,16);
 box(f,7,34,306,29,cyan);box(f,8,35,304,27,EH_RGB(8,26,43));
 label(f,12,42,"PATH:",cyan,8);
 /* Show path tail if very long; keeps last directory names visible. */
 const char *s=p->dir;size_t len=strlen(s);
 if(len>30)s+=len-30;
 label(f,61,42,s,light,31);
 if(p->show_all){box(f,0,64,320,15,EH_RGB(11,34,54));label(f,39,67,"NAME",cyan,6);label(f,272,67,"SIZE",cyan,5);}
 int bottom=p->scroll+EH13_VIEW_ROWS;
 if(bottom>p->count)bottom=p->count;
 for(int i=p->scroll;i<bottom;i++){
  int y=81+(i-p->scroll)*15;
  if(i==p->selected){box(f,7,y-3,306,17,cyan);box(f,9,y-2,302,15,highlight);box(f,9,y-2,3,15,EH_RGB(122,248,255));}else box(f,9,y+12,302,1,EH_RGB(18,42,61));
  const Eh13PickEntry *e=p->items+i;
  file_icon(f,17,y-3,e->name,e->directory,&colors);
  /* 23 glyphs for names; size column starts at x=255. */
  /* Never paint filenames into the right-aligned SIZE column. */
  int maxname=p->show_all?23:32;
  eh_fit_text(f,39,y,e->name,light,maxname);
  if(p->show_all){
   char sz[24];
   if(e->directory)snprintf(sz,sizeof(sz),"--");
   else size_label(sz,sizeof(sz),e->bytes);
   int chars=(int)strlen(sz);
   label(f,312-chars*8,y,sz,i==p->selected?light:dim,chars);
  }
 }
 if(!p->count)label(f,19,93,p->show_all?"EMPTY FOLDER":"NO APPS IN THIS FOLDER",dim,30);
 /* No redundant count strip. Keep this area for file-operation status. */
 box(f,0,221,320,19,EH_RGB(8,27,46));box(f,0,220,320,1,cyan);
 label(f,7,227,p->show_all?"ENTER OPEN/INFO ESC BACK":"ENTER OPEN/ASSIGN ESC BACK",cyan,32);
}
