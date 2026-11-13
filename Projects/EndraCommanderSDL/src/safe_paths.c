/* Brand-new SDL baseline: no source imported from the frozen NX browser. */
#include "safe_paths.h"
#include <stdio.h>
#include <string.h>

int es_name_allowed(const char *name) {
    size_t i,n;
    if (!name) return 0;
    n=strlen(name);
    if (!n || n>=96 || (n==1&&name[0]=='.') ||
        (n==2&&name[0]=='.'&&name[1]=='.')) return 0;
    for(i=0;i<n;++i) {
        unsigned char c=(unsigned char)name[i];
        if(c<32||c==127||c=='/'||c=='\\') return 0;
    }
    return 1;
}

int es_path_allowed(const char *path) {
    size_t i,n;
    static const char base[]="/documents";
    if(!path) return 0;
    n=strlen(path);
    if(n<sizeof(base)-1||n>=ES_PATH_MAX) return 0;
    if(strncmp(path,base,sizeof(base)-1)) return 0;
    if(path[sizeof(base)-1] && path[sizeof(base)-1]!='/') return 0;
    if(path[sizeof(base)-1]=='/' && !path[sizeof(base)]) return 0;
    for(i=sizeof(base);i<n;++i){
        if(path[i]=='/' && (path[i-1]=='/' || i+1==n)) return 0;
        if(path[i]=='.' && path[i-1]=='/' &&
           (path[i+1]=='/' || !path[i+1] ||
            (path[i+1]=='.' && (path[i+2]=='/' || !path[i+2]))))return 0;
    }
    return 1;
}

int es_path_join(char *dst,size_t capacity,const char *parent,const char *name){
    char tmp[ES_PATH_MAX];
    int n;
    if(!dst||!capacity||!es_path_allowed(parent)||!es_name_allowed(name))return 0;
    n=snprintf(tmp,sizeof(tmp),"%s/%s",parent,name);
    if(n<0||(size_t)n>=sizeof(tmp)||(size_t)n>=capacity)return 0;
    if(!es_path_allowed(tmp))return 0;
    memcpy(dst,tmp,(size_t)n+1);return 1;
}

int es_path_parent(char *path){
    char *end;
    if(!es_path_allowed(path)||!strcmp(path,"/documents"))return 0;
    end=strrchr(path,'/');
    if(!end||end==path)return 0;
    *end=0;
    return es_path_allowed(path);
}
