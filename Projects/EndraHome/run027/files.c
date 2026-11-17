/* Run018: built-in Files with on-demand bounded JPEG/PNG viewing. */
#include "files.h"
#include "file_ops.h"
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
static void refresh(Eh17Files *v){
 char root[EH7_PATH_CAP],dir[EH7_PATH_CAP];
 strcpy(root,v->nav.root);strcpy(dir,v->nav.dir);
 eh13_picker_close(&v->nav);
 if(!eh17_picker_open(&v->nav,3,root,1)){v->active=0;return;}
 if(!strcmp(dir,root))return;
 const char *part=dir+strlen(root);
 while(*part=='/')part++;
 while(*part){
  char name[EH13_NAME_MAX];size_t len=0;
  while(part[len]&&part[len]!='/')len++;
  if(!len||len>=sizeof(name))break;
  memcpy(name,part,len);name[len]=0;
  int found=-1;
  for(int i=0;i<v->nav.count;i++)if(v->nav.items[i].directory&&!strcmp(v->nav.items[i].name,name)){found=i;break;}
  if(found<0)break;
  v->nav.selected=found;
  char ignored[EH7_PATH_CAP];eh13_picker_handle(&v->nav,EH_KEY_ENTER,ignored,sizeof(ignored));
  part+=len;while(*part=='/')part++;
 }
}
static int selection(Eh17Files *v,char *out,size_t cap,int *dir){
 if(!v->nav.count||v->nav.selected<0||v->nav.selected>=v->nav.count)return 0;
 Eh13PickEntry *e=&v->nav.items[v->nav.selected];
 int n=snprintf(out,cap,"%s/%s",v->nav.dir,e->name);
 if(dir)*dir=e->directory;
 return n>0&&(size_t)n<cap;
}
static int within_root(const Eh17Files *v,const char *path){
 size_t n=strlen(v->nav.root);
 return !strncmp(path,v->nav.root,n)&&(path[n]=='/'||path[n]==0);
}
static void file_action(Eh17Files *v,int action){
 char src[EH7_PATH_CAP],dst[EH7_PATH_CAP];int dir=0;
 if(action==6){v->edit_folder=1;v->edit_name[0]=0;v->edit_error=0;v->screen=4;return;}
 if(action==3){ /* Paste into current folder. */
  if(!v->clip_mode||!within_root(v,v->clip_path)){strcpy(v->status,"CLIPBOARD EMPTY");return;}
  const char *name=strrchr(v->clip_path,'/');if(!name||!name[1])return;
  int n=snprintf(dst,sizeof(dst),"%s/%s",v->nav.dir,name+1);
  if(n<0||(size_t)n>=sizeof(dst)){strcpy(v->status,"PATH TOO LONG");return;}
  int ok=v->clip_mode==1?eh25_copy(v->clip_path,dst):eh25_move(v->clip_path,dst);
  if(ok){strcpy(v->status,"PASTE FAILED / EXISTS");return;}
  if(v->clip_mode==2)v->clip_mode=0;
  strcpy(v->status,"PASTE COMPLETE");refresh(v);return;
 }
 if(!selection(v,src,sizeof(src),&dir)||!within_root(v,src))return;
 if(action==1||action==2){
  strcpy(v->clip_path,src);v->clip_mode=action;v->clip_is_dir=dir;
  strcpy(v->status,action==1?"COPIED TO CLIPBOARD":"CUT TO CLIPBOARD");return;
 }
 if(action==4){v->edit_folder=0;v->edit_error=0;strcpy(v->path,src);
  const char *base=strrchr(src,'/');if(!base)return;
  strcpy(v->edit_name,base+1);v->screen=4;return;}
 if(action==5){strcpy(v->path,src);v->confirm_delete=1;return;}
 if(action==7){strcpy(v->path,src);v->file_size=dir?0:v->nav.items[v->nav.selected].bytes;v->screen=1;preview(v);return;}
 if(action==0){char chosen[EH7_PATH_CAP];eh13_picker_handle(&v->nav,EH_KEY_ENTER,chosen,sizeof(chosen));return;}
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
 if(v->confirm_delete){
  if(key==EH_KEY_ESC){v->confirm_delete=0;return 1;}
  if(key==EH_KEY_ENTER){
   v->confirm_delete=0;
   if(within_root(v,v->path)&&strcmp(v->path,v->nav.root)&&!eh25_remove(v->path)){
    strcpy(v->status,"DELETE COMPLETE");refresh(v);
   }else strcpy(v->status,"DELETE FAILED");
   return 1;
  }
  return 0;
 }
 if(v->menu_open){
  if(key==EH_KEY_ESC||key==EH_KEY_MENU){v->menu_open=0;return 1;}
  if(key==EH_KEY_UP){if(v->menu_item>0)--v->menu_item;return 1;}
  if(key==EH_KEY_DOWN){if(v->menu_item<7)++v->menu_item;return 1;}
  if(key==EH_KEY_ENTER){int action=v->menu_item;v->menu_open=0;file_action(v,action);return 1;}
  return 0;
 }
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
    if(found<0)break;
    v->nav.selected=found;
    char ignored[EH7_PATH_CAP];
    eh13_picker_handle(&v->nav,EH_KEY_ENTER,ignored,sizeof(ignored));
    part+=len;
    while(*part=='/')part++;
   }
   return 1;
  }
  return 0;
 }
 if(v->screen==0&&key==EH_KEY_MENU){v->menu_open=1;v->menu_item=0;return 1;}
 if(v->screen==1&&key==EH_KEY_MENU){v->edit_folder=0;v->edit_error=0;const char *name=strrchr(v->path,'/');const char *base=name?name+1:v->path;
  size_t length=strlen(base);
  if(length>=sizeof(v->edit_name))return 1;
  memcpy(v->edit_name,base,length+1);v->screen=4;return 1;}
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
