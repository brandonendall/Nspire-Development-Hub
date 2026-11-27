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
static void render_editor(uint16_t *f,const Eh17Files *v,const EhPalette *p){
 const uint16_t bg=eh_skin(p,EH_RGB(5,17,31)),panel=eh_skin(p,EH_RGB(9,28,47));
 const uint16_t cyan=eh_skin(p,EH_RGB(53,217,254));
 box(f,0,0,320,240,bg);
 box(f,0,0,320,40,panel);
 box(f,0,39,320,1,cyan);
 eh_small_text(f,12,8,"TEXT EDITOR",p->ink,2,20);
 const char *name=strrchr(v->path,'/');
 eh_small_text(f,12,27,tail(name?name+1:v->path,40),cyan,1,40);
 if(v->text_edit.dirty)eh_small_text(f,255,11,"UNSAVED",cyan,1,7);
 unsigned start=0,focus=eh_text_cursor_row(v->content,v->text_len,v->text_edit.cursor);
 for(unsigned index=0;;++index){
  EhTextRow row=eh_text_row(v->content,v->text_len,start);
  if(index>=v->text_edit.top){
   unsigned visible=index-v->text_edit.top;
   if(visible>=EH_TEXT_ROWS)break;
   char line[EH_TEXT_COLUMNS+1];
   unsigned size=row.end-row.start;
   for(unsigned i=0;i<size;++i)line[i]=v->content[row.start+i]=='\t'?' ':v->content[row.start+i];
   line[size]=0;
   int y=47+(int)visible*12;
   txt(f,13,y,line,p->ink,EH_TEXT_COLUMNS);
   if(index==focus)box(f,12+(int)(v->text_edit.cursor-start)*8,y-1,1,10,cyan);
  }
  if(!row.more)break;
  start=row.next;
 }
 box(f,0,217,320,23,panel);
 box(f,0,217,320,1,cyan);
 eh_small_text(f,9,226,"MENU SAVE    ENTER NEW LINE    ESC BACK",p->ink,1,48);
 if(v->status.text[0])eh_small_text(f,10,202,v->status.text,cyan,1,49);
 if(v->text_edit.confirm_discard){
  box(f,25,82,270,70,cyan);box(f,27,84,266,66,panel);
  eh_small_text(f,40,98,"DISCARD UNSAVED EDITS?",p->ink,1,36);
  eh_small_text(f,40,128,"ENTER DISCARD    ESC KEEP EDITING",p->ink,1,40);
 }
}

void eh17_files_render(uint16_t *f,const Eh17Files *v,const EhRegistry *apps){
 if(!f||!v||!v->active||!apps)return;
 EhPalette p=eh_palette(apps->appearance);
 if(v->screen==EH_FILES_TEXT_EDIT){render_editor(f,v,&p);return;}
 if(v->screen==EH_FILES_ARCHIVE){
  eh13_picker_render(f,&v->archive,apps);
  if(v->status.text[0]){box(f,8,200,304,19,p.panel);eh_fit_text(f,12,205,v->status.text,p.ink,37);}
  return;
 }
 if(!v->screen){
  eh13_picker_render(f,&v->nav,apps);
  if(v->status.text[0]){box(f,8,200,304,19,p.panel);eh_fit_text(f,12,205,v->status.text,p.ink,37);}
  if(v->clip_mode && !v->status.text[0]){box(f,8,200,304,19,p.panel);txt(f,12,205,v->clip_mode==1?"Clipboard: copy":"Clipboard: cut",p.ink,28);}
  if(v->menu_open){
   static const char *menu[EH_ACTION_COUNT]={"OPEN","COPY","CUT","PASTE","RENAME","DELETE","NEW FOLDER","PROPERTIES","NEW FILE / NOTE","CREATE ZIP","EXTRACT ZIP"};
   /* Keep the neon frame while masking file names behind the popup. */
   eh_theme_glass(f,57,24,206,191,99,&p);
   box(f,57,24,206,1,eh_skin(&p,EH_RGB(67,220,253)));
   box(f,57,213,206,1,eh_skin(&p,EH_RGB(67,220,253)));
   box(f,57,24,1,190,eh_skin(&p,EH_RGB(67,220,253)));
   box(f,262,24,1,190,eh_skin(&p,EH_RGB(67,220,253)));
   eh_small_text(f,77,36,"FILE ACTIONS",p.ink,2,12);
   box(f,69,59,181,1,eh_skin(&p,EH_RGB(59,198,245)));
   int first=v->menu_item<8?0:v->menu_item-7;
   eh_small_text(f,219,39,first?"UP":"DOWN",p.ink,1,8);
   for(int i=first;i<first+8;i++){
    int y=72+(i-first)*17;
    if(i==v->menu_item){
     eh_theme_glass(f,67,y-3,187,16,57,&p);
     box(f,67,y-3,3,16,eh_skin(&p,EH_RGB(145,248,255)));
     box(f,70,y+12,180,1,eh_skin(&p,EH_RGB(49,165,224)));
    }
    /* Tiny action glyphs match the luminous line icons on the approved
     * Neon File Browser concept; no extra sprites or unsafe allocations. */
    uint16_t icon=eh_skin(&p,EH_RGB(111,230,250));
    int ix=77;
    if(i==0){box(f,ix,y+2,7,1,icon);box(f,ix+6,y,1,5,icon);box(f,ix+5,y+3,4,1,icon);}
    else if(i==1){box(f,ix+1,y,7,8,icon);box(f,ix+3,y+2,7,8,eh_skin(&p,EH_RGB(30,116,176)));box(f,ix+4,y+3,5,6,icon);}
    else if(i==2){box(f,ix+1,y+1,2,2,icon);box(f,ix+7,y+1,2,2,icon);box(f,ix+4,y+3,1,8,icon);}
    else if(i==3){box(f,ix+2,y+2,8,8,icon);box(f,ix+4,y,4,2,icon);}
    else if(i==4){box(f,ix+1,y+2,9,1,icon);box(f,ix+1,y+8,9,1,icon);box(f,ix+1,y+3,1,5,icon);}
    else if(i==5){box(f,ix+2,y+1,8,1,icon);box(f,ix+3,y+3,6,7,icon);}
    else if(i==6){box(f,ix+1,y+3,8,8,icon);box(f,ix+1,y+1,4,2,icon);box(f,ix+9,y+5,4,1,icon);}
    else {box(f,ix+3,y+1,6,1,icon);box(f,ix+3,y+9,6,1,icon);box(f,ix+3,y+1,1,9,icon);box(f,ix+9,y+1,1,9,icon);}
    eh_text(f,97,y,menu[i],p.ink,18,1);
   }
  }
  if(v->confirm_delete){
   box(f,24,75,272,91,eh_skin(&p,EH_RGB(53,217,254)));box(f,26,77,268,87,eh_skin(&p,EH_RGB(5,21,37)));
   eh_small_text(f,39,87,"DELETE FILE OR FOLDER?",p.ink,1,38);
   eh_small_text(f,39,109,"INCLUDING ALL CONTENTS",EH_RGB(254,180,144),1,38);
   eh_small_text(f,39,143,"ENTER CONFIRM   ESC CANCEL",p.ink,1,43);
  }
  return;
 }
 if(v->screen==3){eh18_image_render(f,&v->image);return;}
 if(v->screen==4){
  box(f,0,0,320,240,eh_skin(&p,EH_RGB(5,17,31)));
  box(f,0,0,320,38,eh_skin(&p,EH_RGB(9,28,47)));
  box(f,0,36,320,2,eh_skin(&p,EH_RGB(53,217,254)));
  eh_small_text(f,12,11,v->edit_folder==2?"CREATE FILE":v->edit_folder?"CREATE FOLDER":"RENAME FILE",p.ink,2,21);
  eh_small_text(f,15,64,"NAME",eh_skin(&p,EH_RGB(118,217,251)),1,10);
  box(f,13,81,294,33,eh_skin(&p,EH_RGB(52,216,253)));
  box(f,15,83,290,29,eh_skin(&p,EH_RGB(9,28,47)));
  const char *shown=tail(v->edit_name,34);
  txt(f,21,94,shown,p.ink,34);
  int cursor=(int)strlen(shown);
  if(cursor<34)box(f,21+cursor*8,91,2,15,eh_skin(&p,EH_RGB(135,248,255)));
  eh_small_text(f,17,142,v->edit_folder==2?"PLAIN NAMES GET .TXT.TNS":"USE YOUR PHYSICAL KEYBOARD",eh_skin(&p,EH_RGB(151,208,236)),1,46);
  if(v->edit_error)eh_small_text(f,17,172,v->edit_error==3?"NAME EXISTS":v->edit_error==1?"INVALID NAME":"OPERATION FAILED",EH_RGB(255,170,120),1,48);
  box(f,0,218,320,22,eh_skin(&p,EH_RGB(9,28,47)));
  box(f,0,217,320,1,eh_skin(&p,EH_RGB(53,217,254)));
  eh_small_text(f,10,226,"ENTER SAVE     ESC CANCEL",p.ink,1,46);
  return;
 }
 uint16_t white=p.ink,dim=p.ink,cyan=eh_skin(&p,EH_RGB(53,217,254));
 box(f,0,0,320,240,eh_skin(&p,EH_RGB(5,17,31)));
 box(f,0,0,320,38,eh_skin(&p,EH_RGB(9,28,47)));
 eh_small_text(f,12,11,v->screen==1?"FILE INFORMATION":v->json_view?"JSON READER":"TEXT PREVIEW",p.ink,2,21);
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
  box(f,0,218,320,22,eh_skin(&p,EH_RGB(9,28,47)));box(f,0,217,320,1,cyan);
  txt(f,6,227,v->preview_ok?"ENTER PREVIEW ESC BACK":
    eh18_is_image_name(v->path)?"ENTER RETRY ESC BACK":"ESC BACK TO FILES",cyan,36);
 }else{
  box(f,9,43,302,170,eh_skin(&p,EH_RGB(9,28,47)));
  if(!v->text_len)txt(f,14,62,"EMPTY TEXT FILE",dim,30);
  else show_lines(f,v,white);
  if(v->json_view && (v->preview_offset||v->preview_truncated)){
   char range[48];snprintf(range,sizeof(range),"BYTES %lu-%lu / %lu",v->preview_offset+1,v->preview_offset+v->text_len,v->file_size);
   eh_small_text(f,14,203,range,cyan,1,48);
  }else if(v->preview_truncated)txt(f,14,202,"FIRST 4096 BYTES ONLY",cyan,27);
  box(f,0,218,320,22,eh_skin(&p,EH_RGB(9,28,47)));box(f,0,217,320,1,cyan);
  if(v->json_view && (v->preview_offset||v->preview_truncated))
   eh_small_text(f,8,226,"UP/DOWN SCROLL   L/R SECTION   ESC FILES",cyan,1,48);
  else if(v->json_view)
   eh_small_text(f,8,226,eh_text_editable(v->path,v->content,v->text_len)?"UP/DOWN SCROLL   ENTER EDIT   ESC FILES":"UP/DOWN SCROLL   ESC FILES",cyan,1,48);
  else if(v->preview_ok&&!v->preview_truncated&&eh_text_editable(v->path,v->content,v->text_len))
   eh_small_text(f,8,226,"UP/DOWN SCROLL   ENTER EDIT   ESC INFO",cyan,1,48);
  else txt(f,5,227,"UP/DOWN SCROLL ESC INFO MENU FILES",cyan,39);
 }
}
