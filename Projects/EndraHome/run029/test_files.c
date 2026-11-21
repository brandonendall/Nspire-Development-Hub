#include "files.h"
#include "font8x8_basic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
static void create(const char *p,const char *data){
 FILE *f=fopen(p,"wb");assert(f);assert(fputs(data,f)>=0);assert(!fclose(f));
}
static void has(Eh13Picker *p,const char *name,int expected){
 int found=0;
 for(int i=0;i<p->count;i++)if(!strcmp(p->items[i].name,name))found=1;
 assert(found==expected);
}
static int select_name(Eh13Picker *p,const char *name){
 for(int i=0;i<p->count;i++)if(!strcmp(p->items[i].name,name)){
  p->selected=i;p->scroll=i;return 1;
 }
 return 0;
}
static void verify_text(const char *path,const char *expected){
 char actual[128];
 FILE *in=fopen(path,"rb");assert(in);
 size_t n=fread(actual,1,sizeof(actual)-1,in);
 assert(feof(in)&&!ferror(in)&&!fclose(in));
 actual[n]=0;
 assert(!strcmp(actual,expected));
}
static void open_editor(Eh17Files *f){
 assert(select_name(&f->nav,"notes.txt"));
 for(int i=0;i<3;i++)assert(eh17_files_handle(f,EH_KEY_ENTER)==1);
 assert(f->screen==EH_FILES_TEXT_EDIT && !f->text_edit.dirty);
}
int main(void){
 char dir[120],sub[150],file[190];
 snprintf(dir,sizeof(dir),"/tmp/eh017_%ld",(long)getpid());
 snprintf(sub,sizeof(sub),"%s/Projects",dir);
 assert(!mkdir(dir,0700));assert(!mkdir(sub,0700));
 const char *exts[]={"notes.txt","data.cfg","photo.jpg","slides.pdf",
    "program.tns","archive.zip","mystery.zzz","raw.bin"};
 for(unsigned i=0;i<sizeof(exts)/sizeof(exts[0]);i++){
  snprintf(file,sizeof(file),"%s/%s",dir,exts[i]);
  create(file,i==0?"Hello Files!\nNext line\nThird line\n":"test");
 }
 Eh13Picker apps;
 assert(eh13_picker_open(&apps,3,dir));
 has(&apps,"program.tns",1);has(&apps,"notes.txt",0);
 has(&apps,"photo.jpg",0);has(&apps,"data.cfg",0);
 eh13_picker_close(&apps);
 Eh17Files f;
 assert(eh17_files_open(&f,dir));
 assert(f.active&&f.nav.show_all&&f.nav.count==9);
 assert(f.nav.items[0].directory);
 for(unsigned i=0;i<sizeof(exts)/sizeof(exts[0]);i++)has(&f.nav,exts[i],1);
 assert(select_name(&f.nav,"notes.txt"));
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1);
 assert(f.screen==1&&f.preview_ok&&f.text_len>0);
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1&&f.screen==2);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1&&f.screen==1);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1&&f.screen==0);
 assert(select_name(&f.nav,"photo.jpg"));
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1);
 assert(f.screen==1&&!f.preview_ok); /* malformed JPEG gracefully rejected */
 assert(f.image.error[0]);
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1); /* retry, no crash */
 assert(f.screen==1);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1);
 assert(select_name(&f.nav,"raw.bin"));
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1&&!f.preview_ok);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1);
 assert(select_name(&f.nav,"Projects"));
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1);
 assert(!strcmp(f.nav.dir,sub));
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1);
 assert(!strcmp(f.nav.dir,dir));
 snprintf(file,sizeof(file),"%s/notes.txt",dir);
 create(file,"One\r\nTwo");
 open_editor(&f);
 assert(eh21_files_text(&f,'X'));
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1 && f.text_edit.confirm_discard);
 assert(!eh21_files_text(&f,'Y'));
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1 && !f.text_edit.confirm_discard);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1 && f.text_edit.confirm_discard);
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1 && f.screen==2);
 verify_text(file,"One\r\nTwo");
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1 && f.screen==EH_FILES_TEXT_EDIT);
 assert(eh17_files_handle(&f,EH_KEY_DOWN)==1 && f.text_edit.cursor==5);
 assert(eh17_files_handle(&f,EH_KEY_LEFT)==1 && f.text_edit.cursor==3);
 assert(eh17_files_handle(&f,EH_KEY_RIGHT)==1 && f.text_edit.cursor==5);
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1);
 assert(!strcmp(f.content,"One\r\n\r\nTwo"));
 assert(eh21_files_text(&f,8));
 assert(eh21_files_text(&f,'T'));
 char backup[224],stage[224];
 snprintf(backup,sizeof(backup),"%s.old.tns",file);
 snprintf(stage,sizeof(stage),"%s.new.tns",file);
 create(backup,"keep me");
 assert(eh17_files_handle(&f,EH_KEY_MENU)==1 && f.text_edit.dirty);
 assert(strstr(f.status.text,"ALREADY EXISTS"));verify_text(backup,"keep me");
 assert(!remove(backup));
 create(file,"External");
 assert(eh17_files_handle(&f,EH_KEY_MENU)==1 && f.text_edit.dirty);
 assert(strstr(f.status.text,"CHANGED"));verify_text(file,"External");
 create(file,"One\r\nTwo");
 assert(eh17_files_handle(&f,EH_KEY_MENU)==1 && !f.text_edit.dirty);
 verify_text(file,"One\r\nTTwo");assert(access(backup,F_OK)&&access(stage,F_OK));
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1 && f.screen==2);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1 && f.screen==1);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1 && f.screen==0);
 assert(eh_text_editable("Notes.TXT.TNS","abc",3));
 assert(!eh_text_editable("EndraHome_Apps.cfg.tns","abc",3));
 assert(!eh_text_editable("Notes.txt","a\xC3\xA9",3));
 char full[EH_TEXT_CAP+1];memset(full,'x',EH_TEXT_CAP);full[EH_TEXT_CAP]=0;
 eh_text_begin(&f.text_edit,full,EH_TEXT_CAP);
 unsigned length=EH_TEXT_CAP;
 assert(!eh_text_type(&f.text_edit,full,&length,'w')&&length==EH_TEXT_CAP);
 eh_text_move(&f.text_edit,full,length,EH_KEY_DOWN);
 assert(f.text_edit.cursor==EH_TEXT_COLUMNS);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==-1);
 assert(!f.active);
 assert(eh17_is_text_name("EndraHome_Apps.cfg.tns"));
 assert(eh17_is_text_name("boot.txt.tns"));
 assert(!eh17_is_text_name("program.tns"));
 assert(!eh17_is_text_name("image.jpg"));
 for(unsigned i=0;i<sizeof(exts)/sizeof(exts[0]);i++){
  snprintf(file,sizeof(file),"%s/%s",dir,exts[i]);assert(remove(file)==0);
 }
 assert(!rmdir(sub));assert(!rmdir(dir));
 puts("PASS Files, text editing, CRLF, discard, backup preservation, save conflict and bounds");
 return 0;
}
