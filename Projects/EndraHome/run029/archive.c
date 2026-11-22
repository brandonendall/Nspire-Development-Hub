/* Archive creation adapted from EndraZIP Run007. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "miniz.h"
#include <dirent.h>
#include "archive.h"
#include "file_ops.h"
#include <errno.h>
#include <unistd.h>

#define MAXPATH 512
#define COMPRESSION_LEVEL MZ_UBER_COMPRESSION
static int exists(const char *p){struct stat st; return stat(p,&st)==0;}
static int safe(const char *p){
 if(!p[0]||p[0]=='/'||p[0]=='\\'||strchr(p,':'))return 0;
 const char *s=p; while(*s){const char *e=strchr(s,'/');size_t n=e?(size_t)(e-s):strlen(s);
 if(!n||(n==1&&s[0]=='.')||(n==2&&s[0]=='.'&&s[1]=='.'))return 0;
 for(size_t i=0;i<n;i++)if((unsigned char)s[i]<32||s[i]=='\\')return 0;
 if(!e||!e[1])break; /* accept one terminal / for ZIP directories */
 s=e+1;
 }return 1;
}
/*
 * EndraZIP 006: stream file contents directly into the ZIP writer and walk
 * folders recursively. MAX_ZIP_DEPTH bounds stack use and unexpected cycles;
 * each emitted member retains its path under the selected root folder.
 * Empty directories are explicit ZIP directory entries.
 */
/* Count the destination folder as one node when round-tripping. */
#define MAX_ZIP_DEPTH (EH25_MAX_DEPTH-1)
#define MAX_ZIP_FILES (EH25_MAX_ITEMS-1)
static int add(mz_zip_archive *z,const char *path,const char *name,int depth,
               unsigned int *file_count){
 struct stat st;
#ifdef __linux__
 if(lstat(path,&st)||S_ISLNK(st.st_mode))return 0;
#endif
 if(depth>MAX_ZIP_DEPTH||*file_count>=MAX_ZIP_FILES||!safe(name)||stat(path,&st))return 0;
 (*file_count)++;
 if(S_ISDIR(st.st_mode)){
  char folder[MAXPATH],child_path[MAXPATH],child_name[MAXPATH];
  if(snprintf(folder,sizeof(folder),"%s/",name)>=(int)sizeof(folder))return 0;
  /* miniz calls the write callback even for zero-length entries. */
  static const unsigned char empty_entry=0;
  if(!mz_zip_writer_add_mem(z,folder,&empty_entry,0,0))return 0;
  DIR *dir=opendir(path);
  if(!dir)return 0;
  int ok=1;
  struct dirent *e;
  while(errno=0,(e=readdir(dir))){
   if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;
   if(!safe(e->d_name)||
      snprintf(child_path,sizeof(child_path),"%s/%s",path,e->d_name)>=(int)sizeof(child_path)||
      snprintf(child_name,sizeof(child_name),"%s/%s",name,e->d_name)>=(int)sizeof(child_name)||
      !add(z,child_path,child_name,depth+1,file_count)){ok=0;break;}
  }
  if(errno)ok=0;
  closedir(dir);
  return ok;
 }
 if(!S_ISREG(st.st_mode))return 0;
 return mz_zip_writer_add_file(z,name,path,NULL,0,COMPRESSION_LEVEL);
}
int eh_zip_create(const char *source,const char *file){
 mz_zip_archive z={0};char name[MAXPATH];const char *base=strrchr(source,'/');
 base=base?base+1:source;
 if(!safe(base)||exists(file)||strlen(base)>=sizeof(name))return 0;
 strcpy(name,base);
 if(!mz_zip_writer_init_file(&z,file,0))return 0;
 unsigned int file_count=0;
 int ok=add(&z,source,name,0,&file_count)&&mz_zip_writer_finalize_archive(&z);
 mz_zip_writer_end(&z);if(!ok)remove(file);return ok;
}


/* Reject truncated names and embedded NULs before using member paths. */
static int member(mz_zip_archive *z,unsigned index,mz_zip_archive_file_stat *st){
 if(!mz_zip_reader_file_stat(z,index,st))return 0;
 size_t n=strlen(st->m_filename);
 return safe(st->m_filename) && mz_zip_reader_get_filename(z,index,NULL,0)==n+1;
}
static int parents(char *path,size_t base,int *nodes){
 for(char *p=path+base+1;*p;p++){
  if(*p!='/')continue;
  *p=0;
  struct stat st;
  int ok=stat(path,&st)==0?S_ISDIR(st.st_mode):
      (*nodes<EH25_MAX_ITEMS && mkdir(path,0755)==0 && ++*nodes);
  *p='/';
  if(!ok)return 0;
 }
 return 1;
}
int eh_zip_extract(const char *source,const char *output){
 mz_zip_archive z={0};
 if(!mz_zip_reader_init_file(&z,source,0))return 0;
 unsigned count=mz_zip_reader_get_num_files(&z);
 int ok=count<EH25_MAX_ITEMS && mkdir(output,0755)==0;
 int owned=ok,nodes=1;
 for(unsigned i=0;ok&&i<count;i++){
  mz_zip_archive_file_stat st;
  char path[MAXPATH];
  if(!member(&z,i,&st)||st.m_is_encrypted||!st.m_is_supported){ok=0;break;}
  int n=snprintf(path,sizeof(path),"%s/%s",output,st.m_filename);
  if(n<0||(size_t)n>=sizeof(path)){ok=0;break;}
  if(path[n-1]=='/')path[--n]=0;
  int depth=1;
  for(const char *p=path+strlen(output)+1;*p;p++)if(*p=='/')depth++;
  if(depth>EH25_MAX_DEPTH||!parents(path,strlen(output),&nodes)){ok=0;break;}
  if(st.m_is_directory){
   struct stat existing;
   if(stat(path,&existing)==0)ok=S_ISDIR(existing.st_mode);
   else ok=nodes<EH25_MAX_ITEMS && !mkdir(path,0755) && ++nodes;
  }else{
   ok=nodes<EH25_MAX_ITEMS && !exists(path);
   if(ok){nodes++;ok=mz_zip_reader_extract_to_file(&z,i,path,0);}
  }
 }
 mz_zip_reader_end(&z);
 if(!ok&&owned)eh25_remove(output);
 return ok;
}
static int by_name(const void *a,const void *b){
 const Eh13PickEntry *x=a,*y=b;
 return x->directory==y->directory?strcmp(x->name,y->name):y->directory-x->directory;
}
int eh_zip_list(const char *source,const char *prefix,Eh13Picker *view){
 mz_zip_archive z={0};
 size_t len=strlen(prefix);
 if(len>=sizeof(view->dir)||!mz_zip_reader_init_file(&z,source,0))return 0;
 Eh13PickEntry *items=calloc(EH13_PICKER_MAX,sizeof(*items));
 int count=0,ok=items!=NULL;
 for(unsigned i=0;ok&&i<mz_zip_reader_get_num_files(&z);i++){
  mz_zip_archive_file_stat st;
  if(!member(&z,i,&st)){ok=0;break;}
  if(strncmp(st.m_filename,prefix,len))continue;
  const char *name=st.m_filename+len;
  if(!*name)continue;
  size_t n=strcspn(name,"/");
  if(n>=EH13_NAME_MAX){ok=0;break;}
  int folder=name[n]=='/'||st.m_is_directory;
  int j=0;
  while(j<count && (strlen(items[j].name)!=n||memcmp(items[j].name,name,n)))j++;
  if(j<count){if((int)items[j].directory!=folder)ok=0;continue;}
  if(count==EH13_PICKER_MAX){ok=0;break;}
  memcpy(items[count].name,name,n);
  items[count].directory=folder;
  items[count++].bytes=folder?0:st.m_uncomp_size;
 }
 mz_zip_reader_end(&z);
 if(!ok){free(items);return 0;}
 qsort(items,(size_t)count,sizeof(*items),by_name);
 eh13_picker_close(view);
 view->items=items;view->count=count;view->active=1;view->show_all=2;
 strcpy(view->dir,prefix);
 return 1;
}
