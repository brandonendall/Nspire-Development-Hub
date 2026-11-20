/* Native read-only file metadata and text preview; dark EndraHome UI. */
#include "files.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
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
   box(f,55,18,210,205,p.accent);box(f,57,20,206,201,p.background);box(f,60,23,200,195,p.panel);box(f,60,43,200,2,p.accent);
   txt(f,77,30,"FILE ACTIONS",p.accent,18);
   for(int i=0;i<8;i++){int y=53+i*19;
    if(i==v->menu_item){box(f,64,y-4,192,19,p.accent);box(f,66,y-3,188,17,p.selected);}
    txt(f,79,y,menu[i],p.ink,19);
   }
  }
  if(v->confirm_delete){
   box(f,24,75,272,91,p.accent);box(f,27,78,266,85,p.panel);
   txt(f,38,90,"DELETE FILE / FOLDER?",p.ink,27);
   txt(f,38,114,"INCLUDING ALL CONTENTS",p.ink,29);
   txt(f,38,142,"ENTER YES   ESC NO",p.ink,29);
  }
  return;
 }
 if(v->screen==3){eh18_image_render(f,&v->image);return;}
 if(v->screen==4){
  box(f,0,0,320,240,p.background);
  txt(f,12,16,v->edit_folder?"CREATE FOLDER":"RENAME FILE",p.ink,32);
  txt(f,12,61,"NAME:",p.ink,20);
  txt(f,12,82,v->edit_name,p.ink,36);
  txt(f,12,112,"TYPE NAME ON KEYBOARD",p.ink,36);
  if(v->edit_error)txt(f,12,151,v->edit_error==3?"NAME ALREADY EXISTS":v->edit_error==1?"INVALID NAME":"OPERATION FAILED",p.ink,36);
  txt(f,12,222,"ENTER SAVE  ESC CANCEL",p.ink,36);
  return;
 }
 uint16_t white=p.ink,dim=p.ink,cyan=p.ink;
 box(f,0,0,320,240,p.background);
 box(f,0,0,320,32,p.raised);
 txt(f,12,11,v->screen==1?"FILE INFORMATION":"TEXT PREVIEW",white,24);
 box(f,0,31,320,2,p.accent);
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
  box(f,0,222,320,18,p.raised);
  txt(f,6,227,v->preview_ok?"ENTER PREVIEW ESC BACK":
    eh18_is_image_name(v->path)?"ENTER RETRY ESC BACK":"ESC BACK TO FILES",cyan,36);
 }else{
  box(f,9,40,302,174,p.panel);
  if(!v->text_len)txt(f,14,62,"EMPTY TEXT FILE",dim,30);
  else show_lines(f,v,white);
  if(v->preview_truncated)txt(f,14,202,"FIRST 4096 BYTES ONLY",cyan,27);
  box(f,0,222,320,18,p.raised);
  txt(f,5,227,"UP/DOWN SCROLL ESC INFO MENU FILES",cyan,39);
 }
}
