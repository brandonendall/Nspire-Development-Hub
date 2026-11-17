/* Native read-only file metadata and text preview; dark EndraHome UI. */
#include "files.h"
#include <stdint.h>
#include <stdio.h>
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
   unsigned bits=(unsigned char)font8x8_basic[ch][yy];
   for(int xx=0;xx<8;xx++)if(bits&(1u<<xx))box(f,x+k*8+xx,y+yy,1,1,c);
  }
 }
}
static uint16_t file_accent(int theme){
 static const int p[7][3]={{56,207,247},{25,129,239},{126,141,161},{156,169,189},{238,80,93},{69,211,121},{178,109,239}};
 if(theme<0||theme>6)theme=0;
 return RGB(p[theme][0],p[theme][1],p[theme][2]);
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
 if(!v->screen){
  eh13_picker_render(f,&v->nav,apps);
  if(v->status[0]){box(f,0,195,320,16,RGB(43,49,56));txt(f,8,199,v->status,RGB(112,225,204),38);}
  if(v->clip_mode){box(f,0,179,320,15,RGB(43,49,56));txt(f,8,182,v->clip_mode==1?"CLIPBOARD: COPY":"CLIPBOARD: CUT",RGB(112,225,204),28);}
  if(v->menu_open){
   static const char *menu[8]={"OPEN","COPY","CUT","PASTE","RENAME","DELETE","NEW FOLDER","PROPERTIES"};
   box(f,59,19,202,204,RGB(61,70,79));box(f,62,22,196,198,RGB(26,31,36));
   txt(f,77,29,"FILE ACTIONS",RGB(112,225,204),18);
   for(int i=0;i<8;i++){int y=53+i*19;
    if(i==v->menu_item)box(f,67,y-3,184,17,RGB(40,113,139));
    txt(f,79,y,menu[i],RGB(237,246,255),19);
   }
  }
  if(v->confirm_delete){
   box(f,24,75,272,91,RGB(40,67,87));box(f,27,78,266,85,RGB(15,29,48));
   txt(f,38,90,"DELETE FILE / FOLDER?",RGB(255,173,132),27);
   txt(f,38,114,"INCLUDING ALL CONTENTS",RGB(237,246,255),29);
   txt(f,38,142,"ENTER YES   ESC NO",RGB(112,225,204),29);
  }
  return;
 }
 if(v->screen==3){eh18_image_render(f,&v->image);return;}
 if(v->screen==4){
  box(f,0,0,320,240,RGB(25,29,34));
  txt(f,12,16,v->edit_folder?"CREATE FOLDER":"RENAME FILE",RGB(237,246,255),32);
  txt(f,12,61,"NAME:",file_accent(apps->theme),20);
  txt(f,12,82,v->edit_name,RGB(237,246,255),36);
  txt(f,12,112,"TYPE NAME ON KEYBOARD",RGB(155,181,204),36);
  if(v->edit_error)txt(f,12,151,v->edit_error==3?"NAME ALREADY EXISTS":v->edit_error==1?"INVALID NAME":"OPERATION FAILED",RGB(255,120,120),36);
  txt(f,12,222,"ENTER SAVE  ESC CANCEL",file_accent(apps->theme),36);
  return;
 }
 uint16_t white=RGB(237,246,255),dim=RGB(155,181,204),cyan=file_accent(apps->theme);
 box(f,0,0,320,240,RGB(10,17,31));
 box(f,0,0,320,32,RGB(48,55,63));
 txt(f,12,11,v->screen==1?"FILE INFORMATION":"TEXT PREVIEW",white,24);
 box(f,0,31,320,2,cyan);
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
  box(f,0,222,320,18,RGB(27,43,61));
  txt(f,6,227,v->preview_ok?"ENTER PREVIEW ESC BACK":
    eh18_is_image_name(v->path)?"ENTER RETRY ESC BACK":"ESC BACK TO FILES",cyan,36);
 }else{
  box(f,9,40,302,174,RGB(39,45,52));
  if(!v->text_len)txt(f,14,62,"EMPTY TEXT FILE",dim,30);
  else show_lines(f,v,white);
  if(v->preview_truncated)txt(f,14,202,"FIRST 4096 BYTES ONLY",cyan,27);
  box(f,0,222,320,18,RGB(27,43,61));
  txt(f,5,227,"UP/DOWN SCROLL ESC INFO MENU FILES",cyan,39);
 }
}
