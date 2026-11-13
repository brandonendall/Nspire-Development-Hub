/* SDL restart Run002: deliberately small read-only text preview.
 * The new implementation does NOT link with EndraCommander NX's file viewer.
 */
#include "preview.h"
#include "safe_paths.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static unsigned char read_buffer[ES_PREVIEW_BYTES+1];

static int ends_ascii_icase(const char *str,const char *ext){
    size_t len=strlen(str),suffix=strlen(ext),i;
    if(len<suffix)return 0;
    for(i=0;i<suffix;++i){
        unsigned char a=(unsigned char)str[len-suffix+i];
        unsigned char b=(unsigned char)ext[i];
        if(a>='A'&&a<='Z')a=(unsigned char)(a+32);
        if(b>='A'&&b<='Z')b=(unsigned char)(b+32);
        if(a!=b)return 0;
    }
    return 1;
}
int es_preview_supported_name(const char *name){
    static const char *const extensions[]={
        ".txt",".cfg",".ini",".log",".md",
        ".txt.tns",".cfg.tns",".ini.tns",".log.tns",".md.tns"
    };
    size_t i;
    if(!es_name_allowed(name))return 0;
    for(i=0;i<sizeof(extensions)/sizeof(extensions[0]);++i){
        if(ends_ascii_icase(name,extensions[i]))return 1;
    }
    return 0;
}
int es_preview_decode(const unsigned char *data,size_t len,EsPreview *out){
    size_t i;
    unsigned row=0,col=0;
    if(!out||(!data&&len)||len>ES_PREVIEW_BYTES)return 0;
    memset(out,0,sizeof(*out));
    out->count=1;
    out->bytes=len;
    for(i=0;i<len;++i){
        unsigned char c=data[i];
        if(c==0)return 0; /* A binary document is not a text preview. */
        if(c=='\r')continue;
        if(c=='\n'){
            if(row+1>=ES_PREVIEW_ROWS){out->truncated=1;break;}
            ++row;col=0;out->count=row+1;continue;
        }
        if(col>=ES_PREVIEW_COLS){
            if(row+1>=ES_PREVIEW_ROWS){out->truncated=1;break;}
            ++row;col=0;out->count=row+1;
        }
        if(c=='\t')c=' ';
        else if(c<32||c>=127)c='.';
        out->lines[row][col++]=(char)c;
        out->lines[row][col]=0;
    }
    return 1;
}
int es_preview_read(const char *path,EsPreview *out){
    struct stat st;
    FILE *f;
    size_t n;
    int read_failed,was_truncated;
    if(!path||!out||!es_path_allowed(path))return 0;
    if(stat(path,&st)!=0||!S_ISREG(st.st_mode))return 0;
    f=fopen(path,"rb");
    if(!f)return 0;
    n=fread(read_buffer,1,sizeof(read_buffer),f);
    read_failed=ferror(f);
    fclose(f);
    if(read_failed)return 0;
    was_truncated=n>ES_PREVIEW_BYTES;
    if(was_truncated)n=ES_PREVIEW_BYTES;
    if(!es_preview_decode(read_buffer,n,out))return 0;
    if(was_truncated)out->truncated=1;
    return 1;
}
