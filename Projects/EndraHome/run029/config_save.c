/* EndraHome Run014: transaction-style shortcut save for Ndless Documents.
 * Unlike Run013, NEVER rename a temp file over an existing .tns directly.
 * ndless/newlib may refuse replacing a destination file with rename().
 * Move the original to a safe sibling backup first; then move the validated
 * staged file into place. A failed second move restores the original.
 * Crash recovery is available at next startup when cfg is absent.
 */
#include "config_save.h"
#include "preferences.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static int failure(Eh14SaveError *e,Eh14SaveStage stage) {
 if(e){e->stage=stage;e->os_error=errno;}
 return 0;
}
static int exists(const char *path){struct stat st;return stat(path,&st)==0;}
static int sibling_paths(const char *cfg,char *temp,char *old,size_t cap){
 if(!cfg||!cfg[0])return 0;
 const char *slash=strrchr(cfg,'/');
 if(!slash||strcmp(slash+1,EH7_CONFIG_FILE))return 0;
 size_t prefix=(size_t)(slash-cfg)+1;
 if(prefix+sizeof("EH40New.cfg.tns")>cap)return 0;
 memcpy(temp,cfg,prefix);
 strcpy(temp+prefix,"EH40New.cfg.tns");
 memcpy(old,cfg,prefix);
 strcpy(old+prefix,"EH40Old.cfg.tns");
 return 1;
}
static int slot_line(FILE *out,int slot,const EhRegistry *apps,const char *path){
 return fprintf(out,"%d=%s|%s\n",slot,apps->slots[slot].label,path)>0;
}
const char *eh14_save_stage_name(Eh14SaveStage stage){
 switch(stage){
 case EH14_OK:return "OK";
 case EH14_INVALID:return "INVALID PATH";
 case EH14_PATHS:return "SAVE PATH ERROR";
 case EH14_TEMP_BUSY:return "STALE TEMP EXISTS";
 case EH14_TEMP_OPEN:return "CREATE TEMP FAILED";
 case EH14_TEMP_WRITE:return "WRITE TEMP FAILED";
 case EH14_TEMP_VERIFY:return "VERIFY TEMP FAILED";
 case EH14_BACKUP_BUSY:return "BACKUP EXISTS";
 case EH14_BACKUP_MOVE:return "MOVE OLD FAILED";
 case EH14_INSTALL_MOVE:return "INSTALL FAILED";
 case EH14_ROLLBACK:return "RESTORE OLD FAILED";
 case EH14_INSTALL_VERIFY:return "VERIFY NEW FAILED";
 case EH14_BACKUP_RETAINED:return "SAVED BACKUP KEPT";
 default:return "UNKNOWN SAVE ERROR";
 }
}
int eh14_recover_config(const char *cfg,Eh14SaveError *error){
 char temp[512],old[512];
 if(error){error->stage=EH14_OK;error->os_error=0;}
 if(!sibling_paths(cfg,temp,old,sizeof(temp)))return failure(error,EH14_PATHS);
 if(exists(cfg))return 1;
 if(!exists(old)){
  const char *slash=strrchr(old,'/');
  strcpy(old+(slash-old)+1,"EHOld.cfg.tns");
  if(!exists(old))return 1;
 }
 EhRegistry check;eh7_registry_init(&check);
 if(eh7_registry_load(&check,old)<0||check.errors){
  errno=EINVAL;
  return failure(error,EH14_ROLLBACK);
 }
 if(rename(old,cfg)!=0)return failure(error,EH14_ROLLBACK);
 return 1;
}
static int matches(const char *file,const EhRegistry *expected,int slot,const char *path){
 EhRegistry actual;eh7_registry_init(&actual);
 if(eh7_registry_load(&actual,file)<0||actual.errors||actual.theme!=expected->theme||
    actual.clock_offset_seconds!=expected->clock_offset_seconds||!actual.has_appearance)return 0;
 const EhAppearance a=actual.appearance,b=expected->appearance;
 if(a.theme!=b.theme||a.folder!=b.folder||a.font!=b.font||
    a.wallpaper!=b.wallpaper||a.icon!=b.icon||a.icon_color!=b.icon_color)return 0;
 for(int i=0;i<EH7_SLOT_COUNT;i++){
  if(strcmp(actual.slots[i].label,expected->slots[i].label)||
     strcmp(actual.slots[i].path,i==slot?path:expected->slots[i].path))return 0;
 }
 return 1;
}

static int save_config(EhRegistry *apps,int slot,const char *path,const char *cfg,Eh14SaveError *error){
 char temp[512],old[512];
 if(error){error->stage=EH14_OK;error->os_error=0;}
 if(!apps||!eh_prefs_valid(apps->appearance)||!cfg||!cfg[0])
  return failure(error,EH14_INVALID);
 if(!sibling_paths(cfg,temp,old,sizeof(temp)))return failure(error,EH14_PATHS);
 /* Never reuse or clobber a leftover backup/staged file. */
 if(exists(temp))return failure(error,EH14_TEMP_BUSY);
 if(exists(old))return failure(error,EH14_BACKUP_BUSY);

 FILE *in=fopen(cfg,"rb");
 int had_original=in!=NULL;
 if(!in&&exists(cfg))return failure(error,EH14_TEMP_OPEN);
 FILE *out=fopen(temp,"wb");
 if(!out){if(in)fclose(in);return failure(error,EH14_TEMP_OPEN);}
 int ok=1,found=0,theme_found=0;
 if(in){
  char line[512];int last='\n';
  while(fgets(line,sizeof(line),in)){
   size_t len=strlen(line);
   /* Detect truncated/corrupt source rather than dropping the remainder. */
   if(len==sizeof(line)-1&&line[len-1]!='\n'&&!feof(in)){ok=0;break;}
   const char *key=line+strspn(line," \t");
   if(!strncmp(key,EH_ICON_COLOR_LINE,sizeof(EH_ICON_COLOR_LINE)-1))continue;
   if(!strncmp(key,EH_CLOCK_REMAINDER,sizeof(EH_CLOCK_REMAINDER)-1))continue;
   if(!strncmp(key,EH_CLOCK_LINE,sizeof(EH_CLOCK_LINE)-1))continue;
   if(!strncmp(key,EH_APPEARANCE_LINE,sizeof(EH_APPEARANCE_LINE)-1))continue;
   last=len?(unsigned char)line[len-1]:last;
   if(key[0]=='T'&&key[1]=='='){
    if(!theme_found){if(fprintf(out,"T=%d\n",apps->theme)<0)ok=0;theme_found=1;}
   }else if(key[0]==(char)('0'+slot)&&key[1]=='='){
    if(!found){if(!slot_line(out,slot,apps,path))ok=0;found=1;}
   }else if(fputs(line,out)==EOF)ok=0;
   if(!ok)break;
  }
  if(ferror(in))ok=0;
  if(fclose(in)!=0)ok=0;
  if(!theme_found&&ok){if(last!='\n'&&fputc('\n',out)==EOF)ok=0;last='\n';if(fprintf(out,"T=%d\n",apps->theme)<0)ok=0;}
  if(slot>=0&&!found&&ok) {
   if(last!='\n'&&fputc('\n',out)==EOF)ok=0;
   if(ok&&!slot_line(out,slot,apps,path))ok=0;
   last='\n';
  }
  if(ok&&last!='\n'&&fputc('\n',out)==EOF)ok=0;
 }else{
  for(int i=0;i<EH7_SLOT_COUNT&&ok;i++)
   if(!slot_line(out,i,apps,i==slot?path:apps->slots[i].path))ok=0;
  if(ok&&fprintf(out,"T=%d\n",apps->theme)<0)ok=0;
 }
 if(ok && apps->clock_offset_seconds/60 && fprintf(out,EH_CLOCK_LINE "%d\n",apps->clock_offset_seconds/60)<0)ok=0;
 if(ok && apps->clock_offset_seconds%60 && fprintf(out,EH_CLOCK_REMAINDER "%d\n",apps->clock_offset_seconds%60)<0)ok=0;
 const EhAppearance a=apps->appearance;
 if(ok&&fprintf(out,EH_APPEARANCE_LINE "v4 %u %u %u %u %u\n",
                a.theme,a.folder,a.font,a.wallpaper,a.icon)<0)ok=0;
 if(ok && a.icon_color && fprintf(out,EH_ICON_COLOR_LINE "%u\n",a.icon_color)<0)ok=0;
 if(fflush(out)!=0)ok=0;
 if(fclose(out)!=0)ok=0;
 if(!ok){int err=errno;remove(temp);errno=err;return failure(error,EH14_TEMP_WRITE);}

 /* Read staged config back BEFORE replacing original; verify all slots. */
 if(!matches(temp,apps,slot,path)){
  remove(temp);
  return failure(error,EH14_TEMP_VERIFY);
 }

 /* Two explicit moves instead of a replace-existing rename() call. */
 if(had_original && rename(cfg,old)!=0){
  int err=errno;remove(temp);errno=err;return failure(error,EH14_BACKUP_MOVE);
 }
 if(rename(temp,cfg)!=0){
  int err=errno;
  /* Original still exists under EH40Old.cfg.tns until safely restored. */
  if(had_original && rename(old,cfg)!=0){
   return failure(error,EH14_ROLLBACK); /* Preserve both files for rescue. */
  }
  remove(temp);
  errno=err;
  return failure(error,EH14_INSTALL_MOVE);
 }

 if(!matches(cfg,apps,slot,path)){
  /* New content is in cfg; leave old backup in place for recovery. */
  return failure(error,EH14_INSTALL_VERIFY);
 }
 /* Only after the new config is verified may old backup be retired.
  * If remove() fails, leave it intact, but the assignment did succeed. */
 if(had_original&&remove(old)!=0){
  if(error){error->stage=EH14_BACKUP_RETAINED;error->os_error=errno;}
 }
 if(slot>=0){
  int was_set=apps->slots[slot].path[0]!=0;
  if(apps->slots[slot].path!=path)strcpy(apps->slots[slot].path,path);
  if(!was_set&&path[0])apps->enabled++;
  if(was_set&&!path[0])apps->enabled--;
 }
 apps->file_loaded=1;
 apps->has_appearance=1;
 return 1;
}

int eh14_assign_save(EhRegistry *apps,int slot,const char *path,const char *cfg,Eh14SaveError *error){
 if(!path||slot<0||slot>=EH7_SLOT_COUNT||(path[0]&&!eh7_valid_path(path))){
  errno=EINVAL;return failure(error,EH14_INVALID);
 }
 return save_config(apps,slot,path,cfg,error);
}

int eh_save_clock(EhRegistry *apps,int offset,const char *cfg,Eh14SaveError *error){
 if(!apps||offset<0||offset>=86400){errno=EINVAL;return failure(error,EH14_INVALID);}
 int previous=apps->clock_offset_seconds;
 apps->clock_offset_seconds=offset;
 if(save_config(apps,-1,NULL,cfg,error))return 1;
 apps->clock_offset_seconds=previous;
 return 0;
}

int eh_save_appearance(EhRegistry *apps,EhAppearance appearance,const char *cfg,Eh14SaveError *error){
 if(!apps||!eh_prefs_valid(appearance)){
  errno=EINVAL;return failure(error,EH14_INVALID);
 }
 EhAppearance previous=apps->appearance;
 apps->appearance=appearance;
 if(save_config(apps,-1,NULL,cfg,error))return 1;
 apps->appearance=previous;
 return 0;
}
