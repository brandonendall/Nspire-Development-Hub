/* EndraHome Run010: strict, read-only configuration of 7 app shortcuts. */
#include "registry.h"
#include <stdio.h>
#include <string.h>
static const char *defaults[EH7_SLOT_COUNT]={
 "EndraGBA","EndraZIP","EndraCmd","Files","Images","Tools","Favorites"
};
void eh7_registry_init(EhRegistry *apps){
 if(!apps)return;
 memset(apps,0,sizeof(*apps));
 for(int i=0;i<EH7_SLOT_COUNT;i++)
  strcpy(apps->slots[i].label,defaults[i]);
}
int eh7_valid_path(const char *path){
 if(!path||strncmp(path,"/documents/",11))return 0;
 size_t len=strlen(path);
 if(len<=15||len>=EH7_PATH_CAP)return 0;
 if(strcmp(path+len-4,".tns"))return 0;
 if(strstr(path,"..")||strstr(path,"//")||strstr(path,"/./"))return 0;
 for(size_t i=0;i<len;i++)
  if((unsigned char)path[i]<32||(unsigned char)path[i]>126||path[i]=='|')return 0;
 return 1;
}
int eh7_config_path(char *dst,size_t cap,const char *app_executable){
 if(!dst||!cap)return 0;
 dst[0]=0;
 if(!eh7_valid_path(app_executable))return 0;
 const char *slash=strrchr(app_executable,'/');
 if(!slash||!slash[1])return 0;
 size_t prefix=(size_t)(slash-app_executable)+1;
 if(prefix+sizeof(EH7_CONFIG_FILE)>cap)return 0;
 memcpy(dst,app_executable,prefix);
 memcpy(dst+prefix,EH7_CONFIG_FILE,sizeof(EH7_CONFIG_FILE));
 return 1;
}
int eh7_registry_parse(EhRegistry *apps,const char *line){
 if(!apps||!line)return 0;
 while(*line==' '||*line=='\t')line++;
 if(!*line||*line=='#'||*line=='\n'||*line=='\r')return 0;
 if(line[0]=='T'&&line[1]=='='&&line[2]>='0'&&line[2]<='6'&&(line[3]=='\n'||line[3]=='\r'||line[3]==0)){
  apps->theme=line[2]-'0';
  apps->appearance.theme=(unsigned)apps->theme;return 1;
 }
 if(line[0]<'0'||line[0]>'6'||line[1]!='='){apps->errors++;return -1;}
 int slot=line[0]-'0';
 const char *label=line+2;
 const char *split=strchr(label,'|');
 if(!split){apps->errors++;return -1;}
 size_t n=(size_t)(split-label);
 if(n>=EH7_LABEL_CAP){apps->errors++;return -1;}
 const char *path=split+1;
 size_t pn=strcspn(path,"\r\n");
 if(pn>=EH7_PATH_CAP){apps->errors++;return -1;}
 char checked[EH7_PATH_CAP];
 memcpy(checked,path,pn);checked[pn]=0;
 if(pn&&!eh7_valid_path(checked)){apps->errors++;return -1;}
 for(size_t i=0;i<n;i++){
  unsigned char ch=(unsigned char)label[i];
  if(ch<32||ch>126||ch=='|'){apps->errors++;return -1;}
 }
 if(n){memcpy(apps->slots[slot].label,label,n);apps->slots[slot].label[n]=0;}
 strcpy(apps->slots[slot].path,checked);
 apps->valid_lines++;
 return 1;
}
int eh7_registry_load(EhRegistry *apps,const char *cfg){
 if(!apps||!cfg)return -1;
 FILE *f=fopen(cfg,"rb");
 if(!f)return -1;
 apps->file_loaded=1;
 char line[512];
 while(fgets(line,sizeof(line),f)){
  /* Reject truncated lines instead of using partial paths. */
  size_t n=strlen(line);
  if(n==sizeof(line)-1&&line[n-1]!='\n'&&!feof(f)){
   apps->errors++;
   int c;while((c=fgetc(f))!=EOF&&c!='\n'){}
   continue;
  }
  eh7_registry_parse(apps,line);
 }
 fclose(f);
 apps->enabled=0;
 for(int i=0;i<EH7_SLOT_COUNT;i++)
  if(apps->slots[i].path[0])apps->enabled++;
 return apps->enabled;
}
