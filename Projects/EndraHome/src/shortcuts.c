#include "shortcuts.h"
#include <stdio.h>
#include <string.h>
void eh_defaults(EhShortcut slots[EH_SLOTS]) {
    static const char *names[EH_SLOTS]={"EndraGBA","Files","ZIP","Apps"};
    for(int i=0;i<EH_SLOTS;i++) {
        memset(&slots[i],0,sizeof(slots[i]));
        strcpy(slots[i].name,names[i]);
    }
}
/* Read-only configuration: a shortcut can only launch an explicit Documents app. */
int eh_safe_path(const char *p) {
    if(!p || strncmp(p,"/documents/",11)) return 0;
    size_t n=strlen(p);
    if(n<=15 || n>=EH_PATH_CAP || strcmp(p+n-4,".tns")) return 0;
    if(strstr(p,"..") || strstr(p,"//") || strstr(p,"/./")) return 0;
    for(size_t i=0;i<n;i++)
        if((unsigned char)p[i]<32 || (unsigned char)p[i]>126 || p[i]=='|') return 0;
    return 1;
}
/* Format: 0=Name|/documents/folder/app.tns. Empty path disables shortcut. */
int eh_parse_line(EhShortcut slots[EH_SLOTS],const char *src) {
    if(!src) return 0;
    while(*src==' ' || *src=='\t') src++;
    if(src[0]<'0' || src[0]>'3' || src[1]!='=') return 0;
    const char *name=src+2, *sep=strchr(name,'|');
    if(!sep) return 0;
    size_t nn=(size_t)(sep-name);
    if(nn==0 || nn>=EH_NAME_CAP) return 0;
    const char *p=sep+1;
    size_t pn=strcspn(p,"\r\n");
    if(pn>=EH_PATH_CAP) return 0;
    char checked[EH_PATH_CAP];
    memcpy(checked,p,pn);checked[pn]=0;
    if(pn && !eh_safe_path(checked)) return 0;
    for(size_t i=0;i<nn;i++)
        if((unsigned char)name[i]<32 || (unsigned char)name[i]>126) return 0;
    int i=src[0]-'0';
    memcpy(slots[i].name,name,nn);slots[i].name[nn]=0;
    strcpy(slots[i].path,checked);
    return 1;
}
int eh_load(EhShortcut slots[EH_SLOTS],const char *path) {
    FILE *f=fopen(path,"rb");
    if(!f) return -1;
    char line[512];int count=0;
    while(fgets(line,sizeof(line),f)) count+=eh_parse_line(slots,line);
    fclose(f);return count;
}
