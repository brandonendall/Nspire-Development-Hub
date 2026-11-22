#include "app.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>


static char raw[SOURCE_CAP+1], staged[SOURCE_CAP+1], check[SOURCE_CAP+1];
static int fail(const char *msg){ snprintf(app.status,sizeof app.status,"%s",msg); return 0; }
static int join(char *p,const char *name) {
    return snprintf(p,PATH_CAP,"%s/%s",app.root,name)<PATH_CAP;
}
int valid_script(const char *s) {
    size_t n=strlen(s);
    if(!n || s[0]=='.' || n>=NAME_CAP)return 0;
    for(size_t i=0;i<n;i++){
        unsigned char c=s[i];
        if(c<32 || c==127 || c=='/' || c=='\\')return 0;
    }
    return (n>3&&!strcasecmp(s+n-3,".py"))||(n>7&&!strcasecmp(s+n-7,".py.tns"));
}
static int compare_entries(const void *a,const void *b){
    const struct FileEntry *x=a,*y=b;
    return x->folder!=y->folder ? y->folder-x->folder : strcasecmp(x->name,y->name);
}
static void enumerate(DIR *d){
    struct dirent *entry;
    app.count=app.selected=0;
    int skip=app.file_page*FILE_CAP;app.more_files=0;
    while((entry=readdir(d))){
        const char *name=entry->d_name;
        if(!*name || *name=='.' || strlen(name)>=NAME_CAP || strchr(name,'/') || strchr(name,'\\'))continue;
        char path[PATH_CAP];struct stat st;
        if(!join(path,name))continue;
#ifdef __arm__
        if(stat(path,&st))continue;
#else
        if(lstat(path,&st))continue;
#endif
        int folder=S_ISDIR(st.st_mode);
        if(!folder && !(S_ISREG(st.st_mode) && valid_script(name)))continue;
        if(skip>0){skip--;continue;}
        if(app.count==FILE_CAP){app.more_files=1;break;}
        struct FileEntry *item=&app.entries[app.count++];
        strcpy(item->name,name);item->folder=folder;
    }
    closedir(d);
    qsort(app.entries,app.count,sizeof app.entries[0],compare_entries);
}
void files_refresh(void){
    app.count=app.selected=app.more_files=0;
    DIR *d=opendir(app.root);
    if(d)enumerate(d);
    else if(errno!=ENOENT)fail("Cannot read folder");
}
int browse_folder(const char *path){
    if(strlen(path)>=sizeof app.folder)return fail("Folder path too long");
    DIR *d=opendir(path);
    if(!d)return fail("Cannot open folder");
    strcpy(app.folder,path);app.browsing=1;app.file_page=0;enumerate(d);
    if(app.count)app.selected=2;
    return 1;
}
void browse_parent(void){
    if(!strcmp(app.folder,app.documents)){
        strcpy(app.folder,app.home);app.browsing=0;app.file_page=0;files_refresh();return;
    }
    char parent[PATH_CAP];strcpy(parent,app.folder);
    char *slash=strrchr(parent,'/');
    if(!slash)return;
    const char *old=slash+1;*slash=0;
    if(browse_folder(parent)){
        for(int i=0;i<app.count;i++)if(!strcmp(app.entries[i].name,old)){app.selected=i+2;break;}
    }
}
static int read_bytes(const char *path,char *dst) {
    FILE *f=fopen(path,"rb");
    if(!f)return -1;
    size_t n=fread(dst,1,SOURCE_CAP+1,f);
    int error=ferror(f);
    if(fclose(f)||error||n>SOURCE_CAP||memchr(dst,0,n))return -1;
    dst[n]=0;
    return (int)n;
}
int load_script(const char *name) {
    char path[PATH_CAP];
    if(!valid_script(name)||!join(path,name))return fail("Invalid script name");
    int n=read_bytes(path,raw);
    if(n<0)return fail("Unreadable, binary or over 8 KiB");
    for(int i=0;i<n;){
        int step,c=utf_decode(raw+i,&step);i+=step;
        if((c<32 && c!='\r' && c!='\n' && c!='\t')||c==127)return fail("Invalid UTF-8 or binary source");
    }
    Editor *d=&app.doc;
    edit_reset_history();
    memset(d,0,sizeof *d);strcpy(d->name,name);strcpy(d->original,raw);
    d->existing=1;d->crlf=strstr(raw,"\r\n")!=NULL;
    int j=0;
    for(int i=0;i<n;i++){
        if(raw[i]=='\r'){if(raw[i+1]=='\n')i++; d->text[j++]='\n';}
        else d->text[j++]=raw[i];
    }
    return 1;
}
/* An existing scripts folder is reusable. Only the .saving lock below
 * requires exclusive creation. Verify the root by the operation we need. */
static int ensure_scripts_folder(void) {
    DIR *dir=opendir(app.root);
    if(!dir){
        if(strcmp(app.root,app.home))return fail("Original folder is unavailable");
        struct stat st;
        if(stat(app.root,&st)==0)
            return fail(S_ISDIR(st.st_mode) ? "Cannot open scripts folder" : "Scripts path is not a folder");
        int created=mkdir(app.root,0755);
        dir=opendir(app.root);
        if(!dir)return fail(created==0 ? "Cannot open scripts folder" : "Cannot create scripts folder");
    }
    closedir(dir);
    return 1;
}
int save_script(void) {
    Editor *d=&app.doc;
    char dst[PATH_CAP], lock[PATH_CAP], tmp[PATH_CAP], backup[PATH_CAP];
    if(!valid_script(d->name)||!join(dst,d->name)||!join(lock,".saving")||!join(tmp,".saving/source")||!join(backup,".saving/original.py.tns"))
        return fail("Invalid save path");
    if(!ensure_scripts_folder())return 0;
    if(mkdir(lock,0700))return fail("Save busy - original preserved");
    const char *error=NULL;
    struct stat st;
    if(d->existing){
        if(read_bytes(dst,check)<0||strcmp(d->original,check))error="File changed - save cancelled";
    }else if(!stat(dst,&st)||errno!=ENOENT)error="Name in use or inaccessible";
    int j=0;
    for(int i=0;!error && d->text[i];i++){
        int cr=d->crlf && d->text[i]=='\n';
        if(j+1+cr>SOURCE_CAP){error="Saved file would exceed 8 KiB";break;}
        if(cr)staged[j++]='\r';
        staged[j++]=d->text[i];
    }
    staged[j]=0;
    if(!error){
        FILE *f=fopen(tmp,"wb");
        if(!f)error="Cannot create temporary file";
        else {
            int short_write=fwrite(staged,1,j,f)!=(size_t)j;
            if(fclose(f)||short_write||read_bytes(tmp,check)!=j||memcmp(staged,check,j))
                error="Write verification failed";
            else if(d->existing && (read_bytes(dst,check)<0 || strcmp(check,d->original)))
                error="File changed - save cancelled";
            else if(!d->existing && (!stat(dst,&st) || errno!=ENOENT))
                error="Name in use or inaccessible";
            else if(rename(tmp,dst)){
                /* Some TI filesystems refuse rename over an existing file. */
                if(!d->existing || rename(dst,backup))error="Replace failed - original preserved";
                else if(read_bytes(backup,check)<0 || strcmp(check,d->original) || rename(tmp,dst)){
                    if(rename(backup,dst))return fail("Recovery: original in .saving");
                    error="Save failed - original restored";
                }else if(read_bytes(dst,check)!=j || memcmp(staged,check,j))
                    return fail("Recovery: original in .saving");
                else if(remove(backup))return fail("Saved; backup remains in .saving");
            }
        }
    }
    remove(tmp); rmdir(lock);
    if(error)return fail(error);
    strcpy(d->original,staged);d->existing=1;d->dirty=0;
    strcpy(app.status,"Saved");return 1;
}
