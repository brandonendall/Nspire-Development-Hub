#include "files.h"
#include "file_ops.h"
#include "archive.h"
#include "miniz.h"
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
static void action(Eh17Files *f,int which){
 assert(eh17_files_handle(f,EH_KEY_MENU)==1);
 for(int i=0;i<which;i++)assert(eh17_files_handle(f,EH_KEY_DOWN)==1);
 assert(f->menu_item==which);assert(eh17_files_handle(f,EH_KEY_ENTER)==1);
}
static void archive_workflow(Eh17Files *f,const char *dir){
 char path[256];
 assert(select_name(&f->nav,"Projects"));
 eh17_files_handle(f,EH_KEY_ENTER);
 action(f,EH_NEW_FILE);
 for(const char *p="Diary.txt";*p;p++)assert(eh21_files_text(f,*p));
 eh17_files_handle(f,EH_KEY_ENTER);assert(f->screen==EH_FILES_TEXT_EDIT);
 eh21_files_text(f,'A');eh17_files_handle(f,EH_KEY_MENU);
 snprintf(path,sizeof(path),"%s/Projects/Diary.txt.tns",dir);
 verify_text(path,"A");
 while(f->screen)eh17_files_handle(f,EH_KEY_ESC);
 action(f,EH_NEW_FILE);strcpy(f->edit_name,"Diary");eh17_files_handle(f,EH_KEY_ENTER);
 assert(f->edit_error==3);eh17_files_handle(f,EH_KEY_ESC);
 eh17_files_handle(f,EH_KEY_ESC);assert(select_name(&f->nav,"Projects"));
 action(f,EH_CREATE_ZIP);assert(select_name(&f->nav,"Projects.zip.tns"));
 eh17_files_handle(f,EH_KEY_ENTER);assert(f->screen==EH_FILES_ARCHIVE);
 assert(select_name(&f->archive,"Projects"));eh17_files_handle(f,EH_KEY_ENTER);
 assert(select_name(&f->archive,"Diary.txt.tns"));assert(f->archive.items[f->archive.selected].bytes==1);
 eh17_files_handle(f,EH_KEY_ESC);eh17_files_handle(f,EH_KEY_ESC);
 assert(!f->screen && select_name(&f->nav,"Projects.zip.tns"));
 action(f,EH_EXTRACT_ZIP);snprintf(path,sizeof(path),"%s/Projects_unzipped/Projects/Diary.txt.tns",dir);
 verify_text(path,"A");assert(select_name(&f->nav,"Projects.zip.tns"));
 action(f,EH_EXTRACT_ZIP);assert(strstr(f->status.text,"EXISTS"));
 snprintf(path,sizeof(path),"%s/Projects_unzipped",dir);assert(!eh25_remove(path));
 snprintf(path,sizeof(path),"%s/Projects.zip.tns",dir);assert(!remove(path));
 snprintf(path,sizeof(path),"%s/Projects/Diary.txt.tns",dir);assert(!remove(path));
 char zip[256];snprintf(zip,sizeof(zip),"%s/input.zip",dir);
 mz_zip_archive z={0};assert(mz_zip_writer_init_file(&z,zip,0));
 assert(mz_zip_writer_add_mem(&z,"outer/inner/a.txt","test",4,6));
 assert(mz_zip_writer_finalize_archive(&z));mz_zip_writer_end(&z);
 Eh13Picker list={0};assert(eh_zip_list(zip,"",&list)&&list.count==1&&list.items[0].directory);
 assert(eh_zip_list(zip,"outer/",&list)&&!strcmp(list.items[0].name,"inner"));eh13_picker_close(&list);
 memset(&z,0,sizeof(z));assert(mz_zip_writer_init_file(&z,zip,0));
 assert(mz_zip_writer_add_mem(&z,"ok.txt","ok",2,0));
 assert(mz_zip_writer_add_mem(&z,"../outside.txt","bad",3,0));
 assert(mz_zip_writer_finalize_archive(&z));mz_zip_writer_end(&z);
 snprintf(path,sizeof(path),"%s/failed",dir);assert(!eh_zip_extract(zip,path)&&access(path,F_OK));
 assert(!remove(zip));
}
int main(void){
 char dir[120],sub[150],file[190];
 snprintf(dir,sizeof(dir),"/tmp/eh017_%ld",(long)getpid());
 snprintf(sub,sizeof(sub),"%s/Projects",dir);
 assert(!mkdir(dir,0700));assert(!mkdir(sub,0700));
 const char *exts[]={"notes.txt","data.cfg","photo.jpg","slides.pdf",
    "program.tns","archive.zip","mystery.zzz","raw.bin","Save.JSON.TNS","small.json"};
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
 assert(f.active&&f.nav.show_all&&f.nav.count==11);
 assert(f.nav.items[0].directory);
 for(unsigned i=0;i<sizeof(exts)/sizeof(exts[0]);i++)has(&f.nav,exts[i],1);
 action(&f,EH_FIND_NAME);assert(f.screen==EH_FILES_FIND);
 eh21_files_text(&f,'s');eh21_files_text(&f,'A');eh21_files_text(&f,'v');
 eh17_files_handle(&f,EH_KEY_ENTER);
 assert(!f.screen&&!strcmp(f.nav.items[f.nav.selected].name,"Save.JSON.TNS"));
 assert(select_name(&f.nav,"Save.JSON.TNS"));
 snprintf(file,sizeof(file),"%s/Save.JSON.TNS",dir);
 FILE *large=fopen(file,"wb");assert(large);fputs("{\"data\":\"",large);
 for(int j=0;j<5000;++j)fputc(j==4085?'A':j==4086?'B':j==4087?'C':'x',large);
 fputs("\"}",large);assert(fclose(large)==0);
 eh17_files_handle(&f,EH_KEY_ENTER);assert(f.screen==2 && f.json_view && f.preview_truncated);
 eh17_files_handle(&f,EH_KEY_MENU);assert(f.screen==EH_FILES_FIND_JSON);
 eh21_files_text(&f,'a');eh21_files_text(&f,'b');eh21_files_text(&f,'c');
 eh17_files_handle(&f,EH_KEY_ENTER);assert(f.screen==2&&f.matched&&f.match==4094&&f.scroll>0);
 eh17_files_handle(&f,EH_KEY_RIGHT);assert(f.preview_offset==4096 && !f.preview_truncated && strstr(f.content,"\"}"));
 eh17_files_handle(&f,EH_KEY_ENTER);assert(f.screen==2);
 eh17_files_handle(&f,EH_KEY_LEFT);assert(!f.preview_offset && f.content[0]=='{');
 eh17_files_handle(&f,EH_KEY_ESC);assert(!f.screen);
 assert(select_name(&f.nav,"small.json"));eh17_files_handle(&f,EH_KEY_ENTER);assert(f.screen==2);
 eh17_files_handle(&f,EH_KEY_ENTER);assert(f.screen==EH_FILES_TEXT_EDIT);
 eh17_files_handle(&f,EH_KEY_ESC);eh17_files_handle(&f,EH_KEY_ESC);assert(!f.screen);
 assert(eh17_is_text_name("log.LOG.TNS"));
 assert(select_name(&f.nav,"notes.txt"));
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1);
 assert(f.screen==1&&f.preview_ok&&f.text_len>0);
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1&&f.screen==2);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1&&f.screen==1);
 assert(eh17_files_handle(&f,EH_KEY_ESC)==1&&f.screen==0);
 assert(select_name(&f.nav,"slides.pdf"));
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1&&!f.pdf_requested);
 assert(strstr(f.status.text,"PDF HEADER"));
 snprintf(file,sizeof(file),"%s/slides.pdf",dir);create(file,"%PDF-1.7");
 assert(eh17_files_handle(&f,EH_KEY_ENTER)==1&&f.pdf_requested&&!strcmp(file,f.path));
 f.pdf_requested=0;
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
 archive_workflow(&f,dir);
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
