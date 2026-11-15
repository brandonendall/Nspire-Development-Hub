#include "files.h"
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
 assert(eh17_files_handle(&f,EH_KEY_ESC)==-1);
 assert(!f.active);
 assert(eh17_is_text_name("EndraHome_Apps.cfg.tns"));
 assert(eh17_is_text_name("boot.txt.tns"));
 assert(!eh17_is_text_name("program.tns"));
 assert(!eh17_is_text_name("image.jpg"));
 snprintf(file,sizeof(file),"%s/notes.txt",dir);
 char check[48];FILE *in=fopen(file,"rb");assert(in);
 size_t n=fread(check,1,sizeof(check)-1,in);check[n]=0;fclose(in);
 assert(!strcmp(check,"Hello Files!\nNext line\nThird line\n"));
 for(unsigned i=0;i<sizeof(exts)/sizeof(exts[0]);i++){
  snprintf(file,sizeof(file),"%s/%s",dir,exts[i]);assert(remove(file)==0);
 }
 assert(!rmdir(sub));assert(!rmdir(dir));
 puts("Run017 read-only browse, info, preview and Esc tests passed");
 return 0;
}
