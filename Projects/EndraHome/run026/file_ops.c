/* EndraHome Run025: bounded, no-overwrite filesystem operations.
 * 8 KiB stream buffer, depth and item limits, safe partial-copy cleanup.
 * Ndless uses stat; symlinks must be excluded by filesystem policy. No recursive operations outside caller's root.
 */
#define _POSIX_C_SOURCE 200809L
#include "file_ops.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#define BUF 8192
#define PATHMAX 512
static int quota;
int eh25_safe_name(const char *n){
 if(!n||!n[0]||!strcmp(n,".")||!strcmp(n,"..")||strchr(n,'/')||strchr(n,'\\'))return 0;
 return 1;
}
int eh25_inside(const char *parent,const char *child){
 size_t n=strlen(parent);
 return !strncmp(parent,child,n)&&(child[n]=='/'||child[n]==0);
}
static int join(char *out,const char *dir,const char *name){
 int n=snprintf(out,PATHMAX,"%s/%s",dir,name);
 return n>0&&n<PATHMAX;
}
static int exists(const char *p){struct stat s;return stat(p,&s)==0;}
static int remove_rec(const char *path,int depth){
 if(depth>EH25_MAX_DEPTH||++quota>EH25_MAX_ITEMS)return -1;
 struct stat st;if(stat(path,&st))return -1;
 if(!S_ISDIR(st.st_mode))return unlink(path);
 DIR *d=opendir(path);if(!d)return -1;
 struct dirent *e;int rc=0;
 while((e=readdir(d))){
  if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;
  char next[PATHMAX];if(!join(next,path,e->d_name)||remove_rec(next,depth+1)){rc=-1;break;}
 }
 closedir(d);
 if(rc)return -1;
 return rmdir(path);
}
int eh25_remove(const char *path){quota=0;return path&&path[0]=='/'&&strcmp(path,"/")?remove_rec(path,0):-1;}
static int copy_rec(const char *src,const char *dst,int depth){
 if(depth>EH25_MAX_DEPTH||++quota>EH25_MAX_ITEMS||exists(dst))return -1;
 struct stat st;if(stat(src,&st))return -1;
 if(S_ISLNK(st.st_mode)||(!S_ISDIR(st.st_mode)&&!S_ISREG(st.st_mode)))return -1;
 if(S_ISDIR(st.st_mode)){
  if(mkdir(dst,0755))return -1;
  DIR *d=opendir(src);if(!d){rmdir(dst);return -1;}
  struct dirent *e;int rc=0;
  while((e=readdir(d))){
   if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;
   char a[PATHMAX],b[PATHMAX];
   if(!join(a,src,e->d_name)||!join(b,dst,e->d_name)||copy_rec(a,b,depth+1)){rc=-1;break;}
  }
  closedir(d);
  if(rc){int saved=quota;quota=0;remove_rec(dst,0);quota=saved;return -1;}
  return 0;
 }
 FILE *in=fopen(src,"rb");if(!in)return -1;
 FILE *out=fopen(dst,"wb");if(!out){fclose(in);return -1;}
 unsigned char buf[BUF];size_t n;int rc=0;
 while((n=fread(buf,1,sizeof(buf),in))!=0){
  if(fwrite(buf,1,n,out)!=n){rc=-1;break;}
 }
 if(ferror(in))rc=-1;
 if(fflush(out))rc=-1;
 if(fclose(out))rc=-1;
 fclose(in);
 if(rc)unlink(dst);
 return rc;
}
int eh25_copy(const char *src,const char *dst){
 if(!src||!dst||src[0]!='/'||dst[0]!='/'||!strcmp(src,dst)||eh25_inside(src,dst)||exists(dst))return -1;
 quota=0;return copy_rec(src,dst,0);
}
int eh25_move(const char *src,const char *dst){
 if(!src||!dst||src[0]!='/'||dst[0]!='/'||!strcmp(src,dst)||eh25_inside(src,dst)||exists(dst))return -1;
 if(rename(src,dst)==0)return 0;
 if(eh25_copy(src,dst))return -1;
 /* Preserve source on any deletion error; destination remains a complete copy. */
 return eh25_remove(src);
}
