/* Bounded, read-only built-in Files mode; no JPEG decoder yet. */
#include "files.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
static int suffix(const char *p,const char *ending){
 size_t n=strlen(p),k=strlen(ending);
 if(n<k)return 0;
 for(size_t i=0;i<k;i++){
  char a=p[n-k+i],b=ending[i];
  if(a>='A'&&a<='Z')a=(char)(a+32);
  if(a!=b)return 0;
 }
 return 1;
}
int eh17_is_text_name(const char *p){
 if(!p)return 0;
 static const char *ext[]={".txt",".cfg",".csv",".md",".log",".ini",
  ".json",".xml",".py",".c",".h",".sh",".yaml",".yml",".txt.tns",
  ".cfg.tns",".py.tns"};
 for(unsigned i=0;i<sizeof(ext)/sizeof(ext[0]);i++)
  if(suffix(p,ext[i]))return 1;
 return 0;
}
static void tally(Eh17Files *v){
 unsigned pos=0;int lines=0;
 while(pos<v->text_len && lines<5000){
  int cols=0;
  while(pos<v->text_len && cols<35){
   unsigned char c=(unsigned char)v->content[pos++];
   if(c=='\n')break;
   if(c=='\r'){
    if(pos<v->text_len&&v->content[pos]=='\n')++pos;
    break;
   }
   ++cols;
  }
  ++lines;
 }
 v->line_count=lines;
}
static void preview(Eh17Files *v){
 v->preview_ok=0;v->preview_truncated=0;v->line_count=0;v->scroll=0;
 v->text_len=0;v->content[0]=0;
 if(!eh17_is_text_name(v->path))return;
 FILE *f=fopen(v->path,"rb");if(!f)return;
 size_t n=fread(v->content,1,EH17_TEXT_CAP,f);
 int failed=ferror(f);
 if(n==EH17_TEXT_CAP && fgetc(f)!=EOF)v->preview_truncated=1;
 fclose(f);
 if(failed)return;
 for(size_t i=0;i<n;i++){
  unsigned char c=(unsigned char)v->content[i];
  if(!c||(c<32&&c!='\n'&&c!='\r'&&c!='\t')){
   v->content[0]=0;return;
  }
 }
 v->content[n]=0;v->text_len=(unsigned)n;
 v->preview_ok=1;tally(v);
}
int eh17_files_open(Eh17Files *v,const char *root){
 if(!v||!root)return 0;
 memset(v,0,sizeof(*v));
 if(!eh17_picker_open(&v->nav,3,root,1))return 0;
 v->active=1;return 1;
}
void eh17_files_close(Eh17Files *v){
 if(!v)return;
 eh13_picker_close(&v->nav);
 v->active=v->screen=v->preview_ok=0;
 v->text_len=0;v->path[0]=0;
}
int eh17_files_handle(Eh17Files *v,EhKey key){
 if(!v||!v->active||key==EH_KEY_NONE)return 0;
 if(v->screen==2){
  if(key==EH_KEY_ESC||key==EH_KEY_LEFT){v->screen=1;return 1;}
  if(key==EH_KEY_MENU){v->screen=0;return 1;}
  if(key==EH_KEY_UP){if(v->scroll>0)--v->scroll;return 1;}
  if(key==EH_KEY_DOWN){if(v->scroll+12<v->line_count)++v->scroll;return 1;}
  if(key==EH_KEY_RIGHT){
   if(v->scroll+12<v->line_count){
    v->scroll+=10;
    if(v->scroll+12>v->line_count)
     v->scroll=v->line_count>12?v->line_count-12:0;
   }
   return 1;
  }
  return 0;
 }
 if(v->screen==1){
  if(key==EH_KEY_ESC||key==EH_KEY_LEFT||key==EH_KEY_MENU){v->screen=0;return 1;}
  if(key==EH_KEY_ENTER&&v->preview_ok){v->screen=2;return 1;}
  return 0;
 }
 char chosen[EH7_PATH_CAP];
 int code=eh13_picker_handle(&v->nav,key,chosen,sizeof(chosen));
 if(code==-1){eh17_files_close(v);return -1;}
 if(code==1)return 1;
 if(code==2){
  struct stat st;
  if(stat(chosen,&st)!=0)return 0;
  v->file_size=st.st_size>=0?(unsigned long)st.st_size:0;
  strcpy(v->path,chosen);
  v->screen=1;preview(v);return 1;
 }
 return 0;
}
