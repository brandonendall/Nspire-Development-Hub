/* EndraHome Run010: strict, read-only configuration of 7 app shortcuts. */
#include "registry.h"
#include "app_target.h"
#include "preferences.h"
#include <strings.h>
#include <stdio.h>
#include <string.h>
static const char *defaults[EH7_SLOT_COUNT]={
 "EndraGBA","EndraZIP","EndraCmd","Files","Images","Tools","Favorites"
};
unsigned eh_legacy_theme(unsigned old) {
 static const unsigned map[EH_LEGACY_THEME_COUNT]={
  EH_BLUE, EH_BLUE, EH_BLACK, EH_BLACK, EH_RED, EH_GREEN, EH_PURPLE,
  EH_YELLOW, EH_ORANGE, EH_YELLOW, EH_WHITE, EH_BLACK, EH_WHITE, EH_WHITE,
  EH_RED, EH_GREEN, EH_PURPLE, EH_BLUE, EH_BLUE, EH_ORANGE
 };
 return old<EH_LEGACY_THEME_COUNT?map[old]:EH_BLUE;
}
void eh7_registry_init(EhRegistry *apps){
 if(!apps)return;
 memset(apps,0,sizeof(*apps));
 for(int i=0;i<EH7_SLOT_COUNT;i++)
  strcpy(apps->slots[i].label,defaults[i]);
}
int eh7_valid_path(const char *path){
 if(!path||strncasecmp(path,"/documents/",11))return 0;
 size_t len=strlen(path);
 if(len<=15||len>=EH7_PATH_CAP)return 0;
 if(!eh_tns_suffix(path))return 0;
 if(strstr(path,"..")||strstr(path,"//")||strstr(path,"/./"))return 0;
 for(size_t i=0;i<len;i++)
  if((unsigned char)path[i]<32||(unsigned char)path[i]>126||path[i]=='|'||path[i]=='\\')return 0;
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
 if(!strncmp(line,EH_HIDE_SECONDS,sizeof(EH_HIDE_SECONDS)-1)){
  const char *value=line+sizeof(EH_HIDE_SECONDS)-1;
  if((*value!='0'&&*value!='1')||value[1+strspn(value+1,"\r\n")])return 0;
  apps->hide_seconds=*value=='1';return 1;
 }
 int remainder=!strncmp(line,EH_CLOCK_REMAINDER,sizeof(EH_CLOCK_REMAINDER)-1);
 if(remainder||!strncmp(line,EH_CLOCK_LINE,sizeof(EH_CLOCK_LINE)-1)){
  const char *value=line+(remainder?sizeof(EH_CLOCK_REMAINDER):sizeof(EH_CLOCK_LINE))-1;
  size_t len=strspn(value,"0123456789");
  if(len==0 || len>4 || value[len+strspn(value+len,"\r\n")])return 0;
  int number=0;
  for(size_t i=0;i<len;i++)number=number*10+value[i]-'0';
  if(number>=(remainder?60:1440))return 0;
  int prior=apps->clock_offset_seconds;
  apps->clock_offset_seconds=remainder?prior-prior%60+number:number*60+prior%60;
  return 1;
 }
 if(!strncmp(line,EH_ICON_COLOR_LINE,sizeof(EH_ICON_COLOR_LINE)-1)){
  const char *value=line+sizeof(EH_ICON_COLOR_LINE)-1;
  if(value[0]<'0'||value[0]>'0'+EH_THEME_COUNT||value[1+strspn(value+1,"\r\n")])return 0;
  apps->appearance.icon_color=(unsigned)(value[0]-'0');
  return 1;
 }
 if(!strncmp(line,EH_APPEARANCE_LINE,sizeof(EH_APPEARANCE_LINE)-1)){
  EhAppearance parsed;
  if(eh_prefs_parse(line+sizeof(EH_APPEARANCE_LINE)-1,&parsed,4)){
   parsed.icon_color=apps->appearance.icon_color;apps->appearance=parsed;apps->has_appearance=1;return 1;
  }
  /* A damaged comment still allows shortcuts and the legacy look to load. */
  return 0;
 }
 if(!*line||*line=='#'||*line=='\n'||*line=='\r')return 0;
 if(line[0]=='T'&&line[1]=='='&&line[2]>='0'&&line[2]<='6'&&(line[3]=='\n'||line[3]=='\r'||line[3]==0)){
  apps->theme=line[2]-'0';
  if(!apps->has_appearance)apps->appearance.theme=eh_legacy_theme((unsigned)apps->theme);
  return 1;
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
