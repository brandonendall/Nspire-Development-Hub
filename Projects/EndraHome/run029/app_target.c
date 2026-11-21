#include "app_target.h"
#include "registry.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static unsigned char folded(unsigned char c){
    return c>='A'&&c<='Z'?(unsigned char)(c+('a'-'A')):c;
}

int eh_same_path(const char *left,const char *right){
    if(!left||!right)return 0;
    while(*left&&folded(*left)==folded(*right)){++left;++right;}
    return !*left&&!*right;
}

int eh_tns_suffix(const char *name){
    size_t n=strlen(name);
    return n>4&&eh_same_path(name+n-4,".tns");
}

EhAppStatus eh_app_file(const char *path){
    struct stat st;
    if(stat(path,&st)!=0)return errno==ENOENT||errno==ENOTDIR?EH_APP_MISSING:EH_APP_UNREADABLE;
    if(!S_ISREG(st.st_mode))return EH_APP_DOCUMENT;
    FILE *f=fopen(path,"rb");
    if(!f)return EH_APP_UNREADABLE;
    unsigned char sig[4];
    size_t count=fread(sig,1,sizeof(sig),f);
    int read_error=ferror(f);
    if(fclose(f)!=0||read_error)return EH_APP_UNREADABLE;
    if(count==sizeof(sig)&&(!memcmp(sig,"Zehn",4)||!memcmp(sig,"bFLT",4)||!memcmp(sig,"PRG",4)))
        return EH_APP_READY;
    return EH_APP_DOCUMENT;
}

EhAppStatus eh_app_target(const char *path,const char *launcher){
    if(!eh7_valid_path(path))return EH_APP_INVALID;
    if(eh_same_path(path,launcher))return EH_APP_SELF;
    return eh_app_file(path);
}

const char *eh_app_status(EhAppStatus status){
    switch(status){
        case EH_APP_INVALID:return "INVALID SHORTCUT PATH";
        case EH_APP_SELF:return "CANNOT ASSIGN ENDRAHOME ITSELF";
        case EH_APP_MISSING:return "APP FILE MOVED OR MISSING";
        case EH_APP_UNREADABLE:return "CANNOT READ APP FILE";
        case EH_APP_DOCUMENT:return "CHOOSE AN NDLESS APP";
        default:return "";
    }
}
