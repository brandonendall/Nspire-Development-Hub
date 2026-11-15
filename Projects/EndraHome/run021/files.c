/* Run018: built-in Files with on-demand bounded JPEG/PNG viewing. */
#include "files.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
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
 eh18_image_close(&v->image);
 v->active=v->screen=v->preview_ok=0;
 v->text_len=0;v->path[0]=0;
}
int eh17_files_handle(Eh17Files *v,EhKey key){
 if(!v||!v->active||key==EH_KEY_NONE)return 0;
 if(v->screen==4){
  if(key==EH_KEY_ESC){v->screen=v->edit_folder?0:1;return 1;}
  if(key==EH_KEY_ENTER){
   char target[EH7_PATH_CAP];
   const char *dir=v->nav.dir;
   int n=snprintf(target,sizeof(target),"%s/%s",dir,v->edit_name);
   v->edit_error=0;
   if(!v->edit_name[0]||!strcmp(v->edit_name,".")||!strcmp(v->edit_name,"..")||strstr(v->edit_name,"..")||strchr(v->edit_name,'/')||strchr(v->edit_name,'\\')||n<0||(size_t)n>=sizeof(target))v->edit_error=1;
   else if(v->edit_folder){if(mkdir(target,0755)!=0)v->edit_error=2;}
   else if(!strcmp(target,v->path)||stat(target,&(struct stat){0})==0)v->edit_error=3;
   else if(rename(v->path,target)!=0)v->edit_error=2;
   if(v->edit_error)return 1;
   v->screen=0;
   char root[EH7_PATH_CAP],current[EH7_PATH_CAP];strcpy(root,v->nav.root);strcpy(current,v->nav.dir);
   eh13_picker_close(&v->nav);eh17_picker_open(&v->nav,3,root,1);
   /* Re-enter prior directory safely through normal picker navigation. */
   if(strcmp(current,root)==0)return 1;
   /* Directory navigation is restored below by traversing components. */
   const char *part=current+strlen(root);while(*part=='/')part++;
   while(*part){char name[EH13_NAME_MAX];size_t len=0;while(part[len]&&part[len]!='/')len++;if(!len||len>=sizeof(name))break;memcpy(name,part,len);name[len]=0;
    int found=-1;for(int i=0;i<v->nav.count;i++)if(v->nav.items[i].directory&&!strcmp(v->nav.items[i].name,name)){found=i;break;}
    if(found<0)break;v->nav.selected=found;char ignored[EH7_PATH_CAP];eh13_picker_handle(&v->nav,EH_KEY_ENTER,ignored,sizeof(ignored));part+=len;while(*part=='/')part++;
   }
   return 1;
  }
  return 0;
 }
 if(v->screen==0&&key==EH_KEY_MENU){v->edit_folder=1;v->edit_name[0]=0;v->edit_error=0;v->screen=4;return 1;}
 if(v->screen==1&&key==EH_KEY_MENU){v->edit_folder=0;v->edit_error=0;const char *name=strrchr(v->path,'/');snprintf(v->edit_name,sizeof(v->edit_name),"%s",name?name+1:v->path);v->screen=4;return 1;}
 if(v->screen==3){ /* image: Esc goes straight back to our Files list */
  if(key==EH_KEY_ESC||key==EH_KEY_MENU){
   eh18_image_close(&v->image);v->screen=0;return 1;
  }
  return eh18_image_key(&v->image,key);
 }
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
  if(key==EH_KEY_ENTER&&eh18_is_image_name(v->path)){
   if(eh18_image_open(&v->image,v->path)){v->screen=3;return 1;}
   return 1;
  }
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
  v->screen=1;preview(v);
  /* One-Enter JPEG/PNG viewing, like opening an app; no extra file menu. */
  if(eh18_is_image_name(v->path)&&eh18_image_open(&v->image,v->path))
   v->screen=3;
  return 1;
 }
 return 0;
}

int eh21_files_text(Eh17Files *v,int ch){
 if(!v||!v->active||v->screen!=4||!ch)return 0;
 size_t n=strlen(v->edit_name);
 if(ch==8){if(n)v->edit_name[n-1]=0;v->edit_error=0;return 1;}
 if(ch<32||ch>126||ch=='/'||ch=='\\'||ch=='|'||n>=EH13_NAME_MAX-1)return 0;
 v->edit_name[n]=(char)ch;v->edit_name[n+1]=0;v->edit_error=0;return 1;
}
