#include "app.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PATH_CAP 384
static char raw[SOURCE_CAP+1], staged[SOURCE_CAP+1], check[SOURCE_CAP+1];
static int fail(const char *msg){ snprintf(app.status,sizeof app.status,"%s",msg); return 0; }
static int join(char *p,const char *name) {
    return snprintf(p,PATH_CAP,"%s/%s",app.root,name)<PATH_CAP;
}
int valid_script(const char *s) {
    size_t n=strlen(s);
    if(!n || s[0]=='.' || n>=NAME_CAP || strstr(s,".."))return 0;
    for(size_t i=0;i<n;i++){
        char c=s[i];
        if(!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||strchr("_-. ",c)))return 0;
    }
    return (n>3&&!strcmp(s+n-3,".py"))||(n>7&&!strcmp(s+n-7,".py.tns"));
}
static int compare_names(const void *a,const void *b){return strcmp(a,b);}
void files_refresh(void) {
    app.count=app.selected=0;
    DIR *d=opendir(app.root);
    if(!d){if(errno!=ENOENT)fail("Cannot read scripts folder");return;}
    struct dirent *entry;
    while((entry=readdir(d))) {
        if(!valid_script(entry->d_name))continue;
        char path[PATH_CAP]; struct stat st;
        if(!join(path,entry->d_name)||stat(path,&st)||!S_ISREG(st.st_mode))continue;
        if(app.count==FILE_CAP){fail("Showing first 96 scripts");break;}
        strcpy(app.names[app.count++],entry->d_name);
    }
    closedir(d);
    qsort(app.names,app.count,sizeof app.names[0],compare_names);
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
    for(int i=0;i<n;i++){
        unsigned char c=raw[i];
        if((c<32 && c!='\r' && c!='\n' && c!='\t')||c>=127)return fail("Run001 editor requires ASCII source");
    }
    Editor *d=&app.doc;
    memset(d,0,sizeof *d);strcpy(d->name,name);strcpy(d->original,raw);
    d->existing=1;d->crlf=strstr(raw,"\r\n")!=NULL;
    int j=0;
    for(int i=0;i<n;i++){
        if(raw[i]=='\r'){if(raw[i+1]=='\n')i++; d->text[j++]='\n';}
        else d->text[j++]=raw[i];
    }
    return 1;
}
int save_script(void) {
    Editor *d=&app.doc;
    char dst[PATH_CAP], lock[PATH_CAP], tmp[PATH_CAP], backup[PATH_CAP];
    if(!valid_script(d->name)||!join(dst,d->name)||!join(lock,".saving")||!join(tmp,".saving/source")||!join(backup,".saving/original.py.tns"))
        return fail("Invalid save path");
    if(mkdir(app.root,0755) && errno!=EEXIST)return fail("Cannot create scripts folder");
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
