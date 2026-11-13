/* Original ZIP integration using miniz 3.0.2 (MIT). */
#include "nx.h"
#include "miniz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#define ZIP_TOTAL_LIMIT (128ULL*1024*1024)
#define ZIP_FILE_LIMIT (64ULL*1024*1024)
#define ZIP_ENTRY_LIMIT 8192
int nx_zip_entry_valid(const char *name) {
    const char *p=name;char component[NX_NAME];size_t n;
    if(!name || !*name || *name=='/' || strlen(name)>=NX_PATH)return 0;
    while(*p){const char *start=p;while(*p && *p!='/')++p;n=(size_t)(p-start);if(!n||n>=sizeof(component))return 0;memcpy(component,start,n);component[n]=0;if(!nx_name_valid(component))return 0;if(*p)++p;}
    return 1;
}
static int has_link(const char *p) {
#ifdef _TINSPIRE
    (void)p;return 0;
#else
    struct stat s;return lstat(p,&s)==0&&S_ISLNK(s.st_mode);
#endif
}
static int pack(mz_zip_archive *z,const char *path,const char *entry,unsigned depth,uint64_t *total,unsigned *count) {
    struct stat st;DIR *d;struct dirent *e;int ok=1;
    if(depth>32 || ++*count>ZIP_ENTRY_LIMIT)return nx_fail("ZIP has too many items or nesting levels");
    if(has_link(path) || stat(path,&st)!=0)return nx_fail("ZIP source is unavailable or a symbolic link");
    if(nx_progress&&!nx_progress(path,*total))return nx_fail("ZIP creation cancelled");
    if(!S_ISDIR(st.st_mode)){
        if(!S_ISREG(st.st_mode))return nx_fail("ZIP source is not a regular file");
        *total+=(uint64_t)st.st_size;
        if((uint64_t)st.st_size>ZIP_FILE_LIMIT||*total>ZIP_TOTAL_LIMIT)return nx_fail("ZIP exceeds Run 001 size limits");
        return mz_zip_writer_add_file(z,entry,path,NULL,0,MZ_DEFAULT_LEVEL)?1:nx_fail("ZIP write failed; check free storage");
    }
    char folder[NX_PATH];if(snprintf(folder,sizeof(folder),"%s/",entry)>=(int)sizeof(folder))return nx_fail("ZIP entry path too long");
    if(!mz_zip_writer_add_mem(z,folder,"",0,0))return nx_fail("ZIP folder write failed");
    d=opendir(path);if(!d)return nx_fail("Cannot read ZIP source folder");
    while((e=readdir(d))){char p[NX_PATH],n[NX_PATH];if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;if(!nx_join(p,path,e->d_name)||snprintf(n,sizeof(n),"%s/%s",entry,e->d_name)>=(int)sizeof(n)||!pack(z,p,n,depth+1,total,count)){ok=0;break;}}
    closedir(d);return ok;
}
int nx_zip_create(const char *source,const char *archive) {
    mz_zip_archive z={0};char temp[NX_PATH],src[NX_PATH],dst[NX_PATH];uint64_t total=0;unsigned count=0;int ok;
    if(!nx_normalize(src,source)||!nx_normalize(dst,archive)||!nx_writable(dst))return 0;
    size_t n=strlen(src);if(!strncmp(src,dst,n)&&(dst[n]==0||dst[n]=='/'))return nx_fail("Put the ZIP outside its source folder");
    if(nx_exists(dst))return nx_fail("ZIP destination already exists");
    if(snprintf(temp,sizeof(temp),"%s.nxpart",dst)>=(int)sizeof(temp)||nx_exists(temp))return nx_fail("ZIP temporary path is unavailable");
    if(!mz_zip_writer_init_file(&z,temp,0))return nx_fail("Cannot create ZIP file");
    const char *name=strrchr(src,'/');name=name?name+1:src;
    ok=*name&&pack(&z,src,name,0,&total,&count);
    if(ok&&!mz_zip_writer_finalize_archive(&z))ok=nx_fail("Cannot finish ZIP file");
    mz_zip_writer_end(&z);
    if(ok&&(nx_exists(dst)||rename(temp,dst)!=0))ok=nx_fail("Cannot save completed ZIP");
    if(!ok)remove(temp);return ok;
}
static int parents(const char *path) {
    char p[NX_PATH];size_t i;snprintf(p,sizeof(p),"%s",path);
    for(i=1;p[i];++i)if(p[i]=='/'){p[i]=0;if(!nx_is_dir(p)&&mkdir(p,0777)!=0)return 0;p[i]='/';}
    return 1;
}
static void cleanup(const char *path,unsigned depth) {
    if(depth>32)return;
    if(nx_is_dir(path)){DIR *d=opendir(path);struct dirent *e;if(d){while((e=readdir(d))){char p[NX_PATH];if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;if(nx_join(p,path,e->d_name))cleanup(p,depth+1);}closedir(d);}rmdir(path);}else remove(path);
}
typedef struct {FILE *f;const char *name;uint64_t count;} ExtractState;
static size_t write_chunk(void *opaque,mz_uint64 offset,const void *buf,size_t length) {
    ExtractState *s=opaque;(void)offset;
    if(nx_progress&&!nx_progress(s->name,s->count))return 0;
    size_t n=fwrite(buf,1,length,s->f);s->count+=n;return n;
}
int nx_zip_extract(const char *archive,const char *destination) {
    mz_zip_archive z={0};mz_uint i,count;char stage[NX_PATH],canonical[NX_PATH];uint64_t total=0;int ok=1;
    if(!nx_normalize(canonical,destination))return 0;
    destination=canonical;
    if(!nx_writable(destination)||nx_exists(destination))return nx_fail("Choose a new extraction folder inside Documents");
    if(snprintf(stage,sizeof(stage),"%s.nxpart",destination)>=(int)sizeof(stage)||nx_exists(stage))return nx_fail("Extraction temporary folder is unavailable");
    if(!mz_zip_reader_init_file(&z,archive,0))return nx_fail("Cannot read this ZIP");
    count=mz_zip_reader_get_num_files(&z);
    if(count>ZIP_ENTRY_LIMIT){ok=nx_fail("ZIP exceeds 8192 entries");goto finish;}
    /* Validate the entire archive before creating a destination. */
    for(i=0;i<count;++i){mz_zip_archive_file_stat st;unsigned depth=0;const char *p;
        if(!mz_zip_reader_file_stat(&z,i,&st)||mz_zip_reader_get_filename(&z,i,NULL,0)>sizeof(st.m_filename)||!nx_zip_entry_valid(st.m_filename)){ok=nx_fail("ZIP contains an unsafe or overlong path");goto finish;}
        for(p=st.m_filename;*p;++p)if(*p=='/')++depth;
        if(depth>30||strlen(stage)+strlen(st.m_filename)+2>=NX_PATH){ok=nx_fail("ZIP paths exceed supported length or depth");goto finish;}
        if((st.m_bit_flag&1)||((st.m_external_attr>>16)&0170000)==0120000){ok=nx_fail("Encrypted ZIPs and symbolic links are unsupported");goto finish;}
        if(st.m_method!=0&&st.m_method!=8){ok=nx_fail("Only stored and deflated ZIPs are supported");goto finish;}
        if(st.m_uncomp_size>ZIP_FILE_LIMIT||total>ZIP_TOTAL_LIMIT-st.m_uncomp_size){ok=nx_fail("ZIP exceeds 64 MB per file or 128 MB total");goto finish;}
        total+=st.m_uncomp_size;
    }
    if(mkdir(stage,0777)!=0){ok=nx_fail("Cannot create extraction folder");goto finish;}
    for(i=0;i<count;++i){mz_zip_archive_file_stat st;char path[NX_PATH];
        if(!mz_zip_reader_file_stat(&z,i,&st)){ok=nx_fail("ZIP entry is unreadable");break;}
        if(snprintf(path,sizeof(path),"%s/%s",stage,st.m_filename)>=(int)sizeof(path)){ok=nx_fail("ZIP path became too long");break;}
        if(!parents(path)){ok=nx_fail("Cannot create extracted folders");break;}
        if(mz_zip_reader_is_file_a_directory(&z,i)){size_t n=strlen(path);if(n&&path[n-1]=='/')path[n-1]=0;if(!nx_is_dir(path)&&mkdir(path,0777)!=0){ok=nx_fail("Cannot create extracted folder");break;}}
        else {ExtractState state={0};int extracted,closed;
            if(nx_exists(path)){ok=nx_fail("ZIP contains duplicate or conflicting paths");break;}
            state.f=fopen(path,"wb");state.name=st.m_filename;
            if(!state.f){ok=nx_fail("Cannot write extracted file");break;}
            extracted=mz_zip_reader_extract_to_callback(&z,i,write_chunk,&state,0);closed=fclose(state.f);
            if(!extracted||closed){ok=nx_fail("Extraction failed or cancelled; check ZIP and storage");break;}
        }
    }
    if(ok&&(nx_exists(destination)||rename(stage,destination)!=0))ok=nx_fail("Cannot commit extracted folder");
    if(!ok)cleanup(stage,0);
finish:
    mz_zip_reader_end(&z);return ok;
}
int nx_zip_list(const char *archive,char *out,size_t length) {
    mz_zip_archive z={0};mz_uint i,count;size_t used;uint64_t total=0;
    if(!mz_zip_reader_init_file(&z,archive,0))return nx_fail("Cannot read this ZIP");
    count=mz_zip_reader_get_num_files(&z);used=(size_t)snprintf(out,length,"ZIP contents: %u entries\n\n",count);
    for(i=0;i<count;++i){mz_zip_archive_file_stat st;if(!mz_zip_reader_file_stat(&z,i,&st))continue;total+=st.m_uncomp_size;
        if(used+100<length){char size[32];nx_size(size,sizeof(size),st.m_uncomp_size);int n=snprintf(out+used,length-used,"%.60s [%s]\n",st.m_filename,size);if(n>0)used+=(size_t)n;}
    }
    if(used+90<length){char size[32];nx_size(size,sizeof(size),total);snprintf(out+used,length-used,"\nExpanded total: %s\nLong lists may be truncated.\n",size);}
    mz_zip_reader_end(&z);return 1;
}
