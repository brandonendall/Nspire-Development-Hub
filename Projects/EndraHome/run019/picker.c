/* Bounded, read-only Documents .tns browser. No SDL, hooks, or file edits. */
#include "picker.h"
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
static int name_ok(const char *name){
 size_t n;
 if(!name||!(n=strlen(name))||n>=EH13_NAME_MAX ||
    !strcmp(name,".")||!strcmp(name,"..")||strstr(name,".."))return 0;
 for(size_t i=0;i<n;i++){
  unsigned char c=(unsigned char)name[i];
  if(c<32||c>126||strchr("/\\|",c))return 0;
 }
 return 1;
}
static int join(char *out,size_t cap,const char *dir,const char *name){
 if(!name_ok(name))return 0;
 int n=snprintf(out,cap,"%s/%s",dir,name);
 return n>0&&(size_t)n<cap;
}
static int is_tns(const char *n){
 size_t l=strlen(n);
 return l>4&&!strcmp(n+l-4,".tns");
}
static int compare_entries(const void *a,const void *b){
 const Eh13PickEntry *x=(const Eh13PickEntry*)a,*y=(const Eh13PickEntry*)b;
 if(x->directory!=y->directory)return y->directory-x->directory;
 return strcmp(x->name,y->name);
}
static int scan(Eh13Picker *p){
 DIR *dir=opendir(p->dir);
 if(!dir)return 0;
 Eh13PickEntry *list=(Eh13PickEntry*)malloc(sizeof(Eh13PickEntry)*EH13_PICKER_MAX);
 if(!list){closedir(dir);return 0;}
 int n=0,overflow=0;
 struct dirent *de;
 while((de=readdir(dir))){
  char full[EH7_PATH_CAP];
  struct stat st;
  if(!join(full,sizeof(full),p->dir,de->d_name))continue;
  if(stat(full,&st)!=0)continue;
  int directory=S_ISDIR(st.st_mode);
  if(!directory&&(!S_ISREG(st.st_mode)||
        (!p->show_all&&(!is_tns(de->d_name)||
        !strcmp(de->d_name,EH7_CONFIG_FILE)))))continue;
  if(n==EH13_PICKER_MAX){overflow=1;continue;}
  strcpy(list[n].name,de->d_name);
  list[n].directory=(unsigned char)directory;
  n++;
 }
 closedir(dir);
 if(n>1)qsort(list,(size_t)n,sizeof(*list),compare_entries);
 free(p->items);
 p->items=list;p->count=n;p->selected=0;p->scroll=0;p->overflow=overflow;
 return 1;
}
int eh17_picker_open(Eh13Picker *p,int slot,const char *root,int show_all){
 if(!p||!root||slot<0||slot>=EH7_SLOT_COUNT)return 0;
 size_t len=strlen(root);
 if(!len||root[0]!='/'||len>=EH7_PATH_CAP||root[len-1]=='/')return 0;
 memset(p,0,sizeof(*p));
 strcpy(p->dir,root);strcpy(p->root,root);p->slot=slot;p->show_all=!!show_all;
 if(!scan(p)){eh13_picker_close(p);return 0;}
 p->active=1;return 1;
}
void eh13_picker_close(Eh13Picker *p){
 if(!p)return;
 free(p->items);
 memset(p,0,sizeof(*p));
}
static void align_view(Eh13Picker *p){
 if(p->selected<p->scroll)p->scroll=p->selected;
 if(p->selected>=p->scroll+EH13_VIEW_ROWS)
  p->scroll=p->selected-EH13_VIEW_ROWS+1;
}
int eh13_picker_handle(Eh13Picker *p,EhKey key,char *chosen,size_t cap){
 if(!p||!p->active||key==EH_KEY_NONE)return 0;
 if(key==EH_KEY_MENU)return -1; /* Cancel, return to selected icon info */
 if(key==EH_KEY_ESC||key==EH_KEY_LEFT){
  if(!strcmp(p->dir,p->root))return -1;
  char prev[EH7_PATH_CAP];strcpy(prev,p->dir);
  char *slash=strrchr(p->dir,'/');
  if(!slash||(size_t)(slash-p->dir)<strlen(p->root))strcpy(p->dir,p->root);
  else *slash=0;
  if(!scan(p)){strcpy(p->dir,prev);return 0;}
  return 1;
 }
 if(key==EH_KEY_UP||key==EH_KEY_DOWN||key==EH_KEY_RIGHT){
  if(!p->count)return 0;
  int delta=(key==EH_KEY_UP)?-1:(key==EH_KEY_DOWN?1:EH13_VIEW_ROWS);
  int next=p->selected+delta;
  if(next<0)next=0;
  if(next>=p->count)next=p->count-1;
  if(next==p->selected)return 0;
  p->selected=next;align_view(p);return 1;
 }
 if(key==EH_KEY_ENTER&&p->count){
  Eh13PickEntry *e=p->items+p->selected;
  char path[EH7_PATH_CAP];
  if(!join(path,sizeof(path),p->dir,e->name))return 0;
  if(e->directory){
   char prev[EH7_PATH_CAP];strcpy(prev,p->dir);
   strcpy(p->dir,path);
   if(!scan(p)){strcpy(p->dir,prev);return 0;}
   return 1;
  }
  if(!chosen||strlen(path)>=cap)return 0;
  strcpy(chosen,path);
  return 2;
 }
 return 0;
}

int eh13_picker_open(Eh13Picker *p,int slot,const char *root){
 return eh17_picker_open(p,slot,root,0);
}
