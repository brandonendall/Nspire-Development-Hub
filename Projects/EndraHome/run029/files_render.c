/* Run034 dark Files action sheets, metadata, bounded text and name editing. */
#include "files.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "render.h"
#define box eh_box
static void txt(uint16_t *f,int x,int y,const char *s,uint16_t ink,int n){
 eh_text(f,x,y,s,ink,n,1);
}
static const char *tail(const char *s,int cap){
 int len=(int)strlen(s);return len>cap?s+len-cap:s;
}
static void show_lines(uint16_t *f,const Eh17Files *v,uint16_t white){
 unsigned pos=0;int line=0;
 while(pos<v->text_len&&line<v->scroll+12){
  char b[36];int used=0;
  while(pos<v->text_len&&used<35){
   unsigned char c=(unsigned char)v->content[pos++];
   if(c=='\n')break;
   if(c=='\r'){
    if(pos<v->text_len&&v->content[pos]=='\n')++pos;
    break;
   }
   b[used++]=c=='\t'?' ':c<127?(char)c:'?';
  }
  b[used]=0;
  if(line>=v->scroll)txt(f,15,51+(line-v->scroll)*13,b,white,35);
  ++line;
 }
}
void eh17_files_render(uint16_t *f,const Eh17Files *v,const EhRegistry *apps){
 if(!f||!v||!v->active||!apps)return;
 EhPalette p=eh_palette(apps->appearance);
 if(!v->screen){
  eh13_picker_render(f,&v->nav,apps);
  if(v->status.text[0]){box(f,8,200,304,19,p.panel);eh_fit_text(f,12,205,v->status.text,p.ink,37);}
  if(v->clip_mode && !v->status.text[0]){box(f,8,200,304,19,p.panel);txt(f,12,205,v->clip_mode==1?"Clipboard: copy":"Clipboard: cut",p.ink,28);}
  if(v->menu_open){
   static const char *menu[8]={"OPEN","COPY","CUT","PASTE","RENAME","DELETE","NEW FOLDER","PROPERTIES"};
   /* Design-selected Run036 neon-glass actions popup. 84% tinted backdrop
    * preserves readability without filling the screen with solid gray. */
   eh_glass_panel(f,57,24,206,191,93);
   box(f,57,24,206,1,EH_RGB(67,220,253));
   box(f,57,213,206,1,EH_RGB(67,220,253));
   box(f,57,24,1,190,EH_RGB(67,220,253));
   box(f,262,24,1,190,EH_RGB(67,220,253));
   eh_small_text(f,77,36,"FILE ACTIONS",EH_RGB(225,248,255),2,12);
   box(f,69,59,181,1,EH_RGB(59,198,245));
   for(int i=0;i<8;i++){
    int y=72+i*17;
    if(i==v->menu_item){
     eh_glass_panel(f,67,y-3,187,16,57);
     box(f,67,y-3,3,16,EH_RGB(145,248,255));
     box(f,70,y+12,180,1,EH_RGB(49,165,224));
    }
    /* Tiny action glyphs match the luminous line icons on the approved
     * Neon File Browser concept; no extra sprites or unsafe allocations. */
    uint16_t icon=EH_RGB(111,230,250);
    int ix=77;
    if(i==0){box(f,ix,y+2,7,1,icon);box(f,ix+6,y,1,5,icon);box(f,ix+5,y+3,4,1,icon);}
    else if(i==1){box(f,ix+1,y,7,8,icon);box(f,ix+3,y+2,7,8,EH_RGB(30,116,176));box(f,ix+4,y+3,5,6,icon);}
    else if(i==2){box(f,ix+1,y+1,2,2,icon);box(f,ix+7,y+1,2,2,icon);box(f,ix+4,y+3,1,8,icon);}
    else if(i==3){box(f,ix+2,y+2,8,8,icon);box(f,ix+4,y,4,2,icon);}
    else if(i==4){box(f,ix+1,y+2,9,1,icon);box(f,ix+1,y+8,9,1,icon);box(f,ix+1,y+3,1,5,icon);}
    else if(i==5){box(f,ix+2,y+1,8,1,icon);box(f,ix+3,y+3,6,7,icon);}
    else if(i==6){box(f,ix+1,y+3,8,8,icon);box(f,ix+1,y+1,4,2,icon);box(f,ix+9,y+5,4,1,icon);}
    else {box(f,ix+3,y+1,6,1,icon);box(f,ix+3,y+9,6,1,icon);box(f,ix+3,y+1,1,9,icon);box(f,ix+9,y+1,1,9,icon);}
    eh_text(f,97,y,menu[i],EH_RGB(238,251,255),18,1);
   }
  }
  if(v->confirm_delete){
   box(f,24,75,272,91,EH_RGB(53,217,254));box(f,26,77,268,87,EH_RGB(5,21,37));
   eh_small_text(f,39,87,"DELETE FILE OR FOLDER?",EH_RGB(244,251,255),1,38);
   eh_small_text(f,39,109,"INCLUDING ALL CONTENTS",EH_RGB(254,180,144),1,38);
   eh_small_text(f,39,143,"ENTER CONFIRM   ESC CANCEL",EH_RGB(244,251,255),1,43);
  }
  return;
 }
 if(v->screen==3){eh18_image_render(f,&v->image);return;}
 if(v->screen==4){
  box(f,0,0,320,240,EH_RGB(5,17,31));
  box(f,0,0,320,38,EH_RGB(9,28,47));
  box(f,0,36,320,2,EH_RGB(53,217,254));
  eh_small_text(f,12,11,v->edit_folder?"CREATE FOLDER":"RENAME FILE",EH_RGB(245,251,255),2,21);
  eh_small_text(f,15,64,"NAME",EH_RGB(118,217,251),1,10);
  box(f,13,81,294,33,EH_RGB(52,216,253));
  box(f,15,83,290,29,EH_RGB(9,28,47));
  const char *shown=tail(v->edit_name,34);
  txt(f,21,94,shown,p.ink,34);
  int cursor=(int)strlen(shown);
  if(cursor<34)box(f,21+cursor*8,91,2,15,EH_RGB(135,248,255));
  eh_small_text(f,17,142,"USE YOUR PHYSICAL KEYBOARD",EH_RGB(151,208,236),1,46);
  if(v->edit_error)eh_small_text(f,17,172,v->edit_error==3?"NAME EXISTS":v->edit_error==1?"INVALID NAME":"OPERATION FAILED",EH_RGB(255,170,120),1,48);
  box(f,0,218,320,22,EH_RGB(9,28,47));
  box(f,0,217,320,1,EH_RGB(53,217,254));
  eh_small_text(f,10,226,"ENTER SAVE     ESC CANCEL",EH_RGB(242,249,255),1,46);
  return;
 }
 uint16_t white=p.ink,dim=p.ink,cyan=EH_RGB(53,217,254);
 box(f,0,0,320,240,EH_RGB(5,17,31));
 box(f,0,0,320,38,EH_RGB(9,28,47));
 eh_small_text(f,12,11,v->screen==1?"FILE INFORMATION":"TEXT PREVIEW",EH_RGB(245,251,255),2,21);
 box(f,0,36,320,2,cyan);
 if(v->screen==1){
  txt(f,13,52,"NAME:",cyan,10);
  const char *name=strrchr(v->path,'/');
  txt(f,13,69,tail(name?name+1:v->path,36),white,36);
  txt(f,13,96,"SIZE:",cyan,10);
  char size[42];unsigned long long n=v->file_size;const char *u[]={"B","KB","MB","GB"};int k=0;unsigned long long rem=0;
  while(n>=1024&&k<3){rem=n%1024;n/=1024;k++;}
  if(!k)snprintf(size,sizeof(size),"%llu B",n);
  else {unsigned d=(unsigned)((rem*10+512)/1024);if(d==10){n++;d=0;}if(d)snprintf(size,sizeof(size),"%llu.%u %s",n,d,u[k]);else snprintf(size,sizeof(size),"%llu %s",n,u[k]);}
  txt(f,74,96,size,white,27);
  txt(f,13,122,"PATH:",cyan,10);
  txt(f,13,138,tail(v->path,36),dim,36);
  const char *message=v->image.error[0]?v->image.error:
       v->preview_ok?"TEXT PREVIEW AVAILABLE":
       eh18_is_image_name(v->path)?"ENTER VIEW JPEG/PNG":
       eh17_is_text_name(v->path)?"NOT TEXT / CANNOT READ":
       "VIEWER NOT INSTALLED YET";
  txt(f,13,161,message,v->preview_ok?cyan:dim,36);
  txt(f,13,184,"READ-ONLY. FILE NOT CHANGED.",dim,36);
  box(f,0,218,320,22,EH_RGB(9,28,47));box(f,0,217,320,1,cyan);
  txt(f,6,227,v->preview_ok?"ENTER PREVIEW ESC BACK":
    eh18_is_image_name(v->path)?"ENTER RETRY ESC BACK":"ESC BACK TO FILES",cyan,36);
 }else{
  box(f,9,43,302,170,EH_RGB(9,28,47));
  if(!v->text_len)txt(f,14,62,"EMPTY TEXT FILE",dim,30);
  else show_lines(f,v,white);
  if(v->preview_truncated)txt(f,14,202,"FIRST 4096 BYTES ONLY",cyan,27);
  box(f,0,218,320,22,EH_RGB(9,28,47));box(f,0,217,320,1,cyan);
  txt(f,5,227,"UP/DOWN SCROLL ESC INFO MENU FILES",cyan,39);
 }
}
