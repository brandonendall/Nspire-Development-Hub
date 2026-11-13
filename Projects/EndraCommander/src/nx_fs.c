/* EndraCommander NX Run 001. Original implementation, 2026-11-12. */
#include "nx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

char nx_error[160];
char nx_write_root[NX_PATH] = "/documents";
int (*nx_progress)(const char *, uint64_t);
int nx_fail(const char *message) { snprintf(nx_error,sizeof(nx_error),"%s",message); return 0; }
static int symlink_path(const char *path) {
#ifdef _TINSPIRE
    (void)path; return 0; /* Nucleus FAT has no POSIX symlinks. */
#else
    struct stat st; return lstat(path,&st)==0 && S_ISLNK(st.st_mode);
#endif
}
/* Run006: only one path-size scratch buffer, no 256-element stack table.
 * This reduces nested GUI/navigation stack pressure on the CX II.
 * Aliasing (out == path) is supported by copying input before writing. */
int nx_normalize(char *out,const char *path) {
    char tmp[NX_PATH];
    const char *p;
    size_t used=1;
    if(!out||!path||path[0]!='/'||strlen(path)>=sizeof(tmp))
        return nx_fail("Invalid or overlong path");
    strcpy(tmp,path);
    p=tmp;
    out[0]='/';out[1]=0;
    while(*p){
        const char *s;size_t n;
        while(*p=='/')++p;
        if(!*p)break;
        s=p;
        while(*p&&*p!='/')++p;
        n=(size_t)(p-s);
        if(n==1&&s[0]=='.')continue;
        if(n==2&&s[0]=='.'&&s[1]=='.'){
            if(used==1)return nx_fail("Path goes above root");
            char *slash=strrchr(out,'/');
            if(!slash)return nx_fail("Path normalization failed");
            used=(size_t)(slash-out);
            if(!used)used=1;
            out[used]=0;
            continue;
        }
        if(n>=NX_NAME||used+(used>1?1u:0u)+n>=NX_PATH)
            return nx_fail("Path too long");
        if(used>1)out[used++]='/';
        memcpy(out+used,s,n);
        used+=n;out[used]=0;
    }
    return 1;
}
int nx_name_valid(const char *name) {
    const unsigned char *p=(const unsigned char*)name;
    if (!name || !*name || strlen(name)>=NX_NAME || !strcmp(name,".") || !strcmp(name,"..")) return 0;
    for (;*p;++p) if (*p<32 || strchr("/\\:*?\"<>|",*p)) return 0;
    return name[strlen(name)-1]!=' ' && name[strlen(name)-1]!='.';
}
int nx_join(char *out,const char *dir,const char *name) {
    char tmp[NX_PATH]; int n;
    if (!nx_name_valid(name)) return nx_fail("Invalid file name");
    n=snprintf(tmp,sizeof(tmp),"%s%s%s",dir,strcmp(dir,"/")?"/":"",name);
    if (n<0 || (size_t)n>=sizeof(tmp)) return nx_fail("Path too long");
    return nx_normalize(out,tmp);
}
int nx_parent(char *path) { char *p=strrchr(path,'/'); if (!p) return 0; if(p==path) path[1]=0; else *p=0; return 1; }
int nx_exists(const char *p) { struct stat st; return stat(p,&st)==0 || symlink_path(p); }
int nx_is_dir(const char *p) { struct stat st; return !symlink_path(p) && stat(p,&st)==0 && S_ISDIR(st.st_mode); }
static int within(const char *p,const char *root) { size_t n=strlen(root); return !strncmp(p,root,n) && (p[n]==0 || p[n]=='/' || !strcmp(root,"/")); }
static int path_no_links(const char *path) {
    char tmp[NX_PATH]; size_t i;
    if (!nx_normalize(tmp,path)) return 0;
    for(i=1;;++i) if(tmp[i]=='/' || tmp[i]==0) {
        char c=tmp[i]; tmp[i]=0;
        if(symlink_path(tmp)) return nx_fail("Symbolic links are not supported");
        tmp[i]=c; if(!c) break;
    }
    return 1;
}
int nx_writable(const char *path) {
    char p[NX_PATH],root[NX_PATH];
    if(!nx_normalize(p,path) || !nx_normalize(root,nx_write_root)) return 0;
    if(!within(p,root) || !strcmp(p,root)) return nx_fail("Writes are limited to files inside Documents");
    return path_no_links(p);
}
static int sort_mode;
static int compare(const void *a,const void *b) {
    const NxEntry *x=a,*y=b;
    if(x->directory!=y->directory) return y->directory-x->directory;
    if(sort_mode==1 && !x->directory && x->size!=y->size) return x->size>y->size?-1:1;
    if(sort_mode==2 && x->modified!=y->modified) return x->modified>y->modified?-1:1;
    int c=strcasecmp(x->name,y->name); return c?c:strcmp(x->name,y->name);
}
void nx_sort(NxPane *p) { sort_mode=p->sort; if(p->count>1)qsort(p->items,p->count,sizeof(*p->items),compare); }
static int matches(const char *s,const char *q) { size_t n=strlen(q); if(!n)return 1; for(;*s;++s)if(!strncasecmp(s,q,n))return 1; return 0; }
void nx_release(NxPane *p) { free(p->items); p->items=NULL;p->count=p->cursor=p->scroll=0; }
int nx_scan(NxPane *p) {
    DIR *dir; struct dirent *d; NxEntry *list=NULL; size_t count=0,capacity=0;
    char safe_path[NX_PATH], safe_root[NX_PATH];
    /* Run005: hard-freeze observed while traversing the OS root. Refuse
     * out-of-Documents directory scans before any stat/opendir. */
    if(!nx_normalize(safe_path,p->path) ||
       !nx_normalize(safe_root,nx_write_root))return 0;
    if(!within(safe_path,safe_root))
        return nx_fail("Run005 safe mode: Documents folder only");
    if(!path_no_links(safe_path))return 0;
    dir=opendir(safe_path); if(!dir)return nx_fail("Cannot open this folder");
    while((d=readdir(dir))) {
        char path[NX_PATH]; struct stat st; NxEntry *e;
        if(!strcmp(d->d_name,".") || !strcmp(d->d_name,".."))continue;
        if(!nx_join(path,p->path,d->d_name) || symlink_path(path) || stat(path,&st)!=0)continue;
        if(p->folders_only && !S_ISDIR(st.st_mode))continue;
        if(!matches(d->d_name,p->filter))continue;
        if(count==NX_MAX_ENTRIES){closedir(dir);free(list);return nx_fail("Folder exceeds 4096 displayed entries; use a filter");}
        if(count==capacity) { size_t cap=capacity?capacity*2:32; void *next=realloc(list,cap*sizeof(*list)); if(!next){closedir(dir);free(list);return nx_fail("Not enough memory for folder");}list=next;capacity=cap; }
        e=list+count++; memset(e,0,sizeof(*e));snprintf(e->name,sizeof(e->name),"%s",d->d_name);
        e->directory=S_ISDIR(st.st_mode);e->size=e->directory?0:(uint64_t)st.st_size;
#ifdef _TINSPIRE
        e->modified=st.st_mtim.tv_sec;
#else
        e->modified=st.st_mtime;
#endif
    }
    closedir(dir);free(p->items);p->items=list;p->count=count;
    if(p->cursor>=count)p->cursor=count?count-1:0;
    if(p->scroll>p->cursor)p->scroll=p->cursor;
    nx_sort(p);return 1;
}
int nx_mkdir(const char *path) {
    char canonical[NX_PATH];
    if(!nx_normalize(canonical,path)||!nx_writable(canonical))return 0;
    if(nx_exists(canonical))return nx_fail("Destination already exists");
    return mkdir(canonical,0777)==0?1:nx_fail("Cannot create folder");
}
static int copy_file(const char *src,const char *dst) {
    FILE *in=NULL,*out=NULL; char temp[NX_PATH]; unsigned char *buf; size_t n; uint64_t total=0; int ok=0;
    if(snprintf(temp,sizeof(temp),"%s.nxpart",dst)>=(int)sizeof(temp))return nx_fail("Copy destination too long");
    if(nx_exists(temp))return nx_fail("A partial copy already exists; choose a new name");
    buf=malloc(32768);if(!buf)return nx_fail("Not enough memory to copy file");
    in=fopen(src,"rb");if(!in){free(buf);return nx_fail("Cannot read source file");}
    out=fopen(temp,"wb");if(!out){fclose(in);free(buf);return nx_fail("Cannot create destination file");}
    while((n=fread(buf,1,32768,in))) {
        if(fwrite(buf,1,n,out)!=n){nx_fail("Write failed; check free storage");goto done;}
        total+=n;if(nx_progress && !nx_progress(src,total)){nx_fail("Copy cancelled");goto done;}
    }
    if(ferror(in)){nx_fail("Source read failed");goto done;}
    if(fclose(out)!=0){out=NULL;nx_fail("Cannot finish destination file");goto done;}out=NULL;
    if(nx_exists(dst)){nx_fail("Destination already exists");goto done;}
    if(rename(temp,dst)!=0){nx_fail("Cannot commit copied file");goto done;}
    ok=1;
done:
    fclose(in);if(out)fclose(out);if(!ok)remove(temp);free(buf);return ok;
}
static void cleanup_created(const char *path,unsigned depth) {
    if(depth>32)return;
    if(nx_is_dir(path)){DIR *d=opendir(path);struct dirent *e;if(d){while((e=readdir(d))){char p[NX_PATH];if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;if(nx_join(p,path,e->d_name))cleanup_created(p,depth+1);}closedir(d);}rmdir(path);}else remove(path);
}
static int copy_tree(const char *src,const char *dst,unsigned depth) {
    struct stat st;DIR *dir;struct dirent *e;int ok=1;
    if(depth>32)return nx_fail("Folder nesting exceeds 32 levels");
    if(symlink_path(src))return nx_fail("Symbolic links are not supported");
    if(stat(src,&st)!=0)return nx_fail("Source no longer exists");
    if(nx_exists(dst))return nx_fail("Destination already exists");
    if(!S_ISDIR(st.st_mode)){if(!S_ISREG(st.st_mode))return nx_fail("Unsupported source file type");return copy_file(src,dst);}
    dir=opendir(src);if(!dir)return nx_fail("Cannot read source folder");
    if(mkdir(dst,0777)!=0){closedir(dir);return nx_fail("Cannot create destination folder");}
    while((e=readdir(dir))){char a[NX_PATH],b[NX_PATH];if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;if(!nx_join(a,src,e->d_name)||!nx_join(b,dst,e->d_name)||!copy_tree(a,b,depth+1)){ok=0;break;}}
    closedir(dir);
    /* Only remove a destination after this call successfully created it. */
    if(!ok)cleanup_created(dst,0);
    return ok;
}
int nx_copy(const char *src,const char *dst) {
    char a[NX_PATH],b[NX_PATH];
    if(!nx_normalize(a,src)||!nx_normalize(b,dst)||!path_no_links(a)||!nx_writable(b))return 0;
    if(within(b,a))return nx_fail("Cannot copy an item into itself");
    if(nx_exists(b))return nx_fail("Destination already exists; no overwrite");
    return copy_tree(a,b,0);
}
int nx_move(const char *src,const char *dst) {
    char a[NX_PATH],b[NX_PATH];
    if(!nx_normalize(a,src)||!nx_normalize(b,dst)||!nx_writable(a)||!nx_writable(b))return 0;
    if(within(b,a))return nx_fail("Cannot move an item into itself");
    if(nx_exists(b))return nx_fail("Destination already exists; no overwrite");
    return rename(a,b)==0?1:nx_fail("Move failed; source was kept");
}
int nx_delete(const char *path) {
    char canonical[NX_PATH];
    if(!nx_normalize(canonical,path)||!nx_writable(canonical))return 0;
    if(nx_is_dir(canonical))return rmdir(canonical)==0?1:nx_fail("Only empty folders can be deleted in Run 001");
    return remove(canonical)==0?1:nx_fail("Cannot delete this file");
}
static int ends(const char *s,const char *x) {size_t a=strlen(s),b=strlen(x);return a>=b&&!strcasecmp(s+a-b,x);}
int nx_kind(const char *path) {
    char name[NX_PATH];size_t n;FILE *f;unsigned char magic[4]={0};
    snprintf(name,sizeof(name),"%s",path);n=strlen(name);if(ends(name,".tns"))name[n-4]=0;
    if(ends(name,".zip"))return 3;
    if(ends(name,".png")||ends(name,".jpg")||ends(name,".jpeg")||ends(name,".bmp"))return 4;
    if(ends(name,".txt")||ends(name,".md")||ends(name,".c")||ends(name,".h")||ends(name,".py")||ends(name,".lua")||ends(name,".csv")||ends(name,".json")||ends(name,".cfg")||ends(name,".log"))return 1;
    f=fopen(path,"rb");if(f){fread(magic,1,4,f);fclose(f);}
    if(!memcmp(magic,"PRG\0",4)||!memcmp(magic,"\x7f" "ELF",4))return 2;
    if(!memcmp(magic,"PK\3\4",4)||!memcmp(magic,"PK\5\6",4))return 3;
    return 0;
}
void nx_size(char *out,size_t len,uint64_t n) {
    if(n<1024)snprintf(out,len,"%lu B",(unsigned long)n);
    else if(n<1024*1024)snprintf(out,len,"%lu.%lu KB",(unsigned long)(n/1024),(unsigned long)((n%1024)*10/1024));
    else snprintf(out,len,"%lu.%lu MB",(unsigned long)(n/1048576),(unsigned long)((n%1048576)*10/1048576));
}
