/* Preserve existing comments/other slot lines; replace only target slot. */
#include "config_save.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
static int slot_line(FILE *out,int slot,const EhRegistry *apps,const char *path){
 return fprintf(out,"%d=%s|%s\n",slot,apps->slots[slot].label,path)>0;
}
int eh13_assign_save(EhRegistry *apps,int slot,const char *path,const char *cfg){
 char temp[512];
 struct stat st;
 if(!apps||slot<0||slot>=EH7_SLOT_COUNT||!eh7_valid_path(path)||!cfg||!cfg[0])return 0;
 int k=snprintf(temp,sizeof(temp),"%s.pending.tns",cfg);
 if(k<0||(size_t)k>=sizeof(temp))return 0;
 FILE *in=fopen(cfg,"rb");
 if(!in&&stat(cfg,&st)==0)return 0; /* Exists but cannot be read. */
 FILE *out=fopen(temp,"wb");
 if(!out){if(in)fclose(in);return 0;}
 int ok=1,found=0;
 if(in){
  char line[512];
  while(fgets(line,sizeof(line),in)){
   if(line[0]==(char)('0'+slot)&&line[1]=='='){
    if(!found){if(!slot_line(out,slot,apps,path))ok=0;found=1;}
    /* Remove duplicate definitions for this same slot. */
   }else if(fputs(line,out)==EOF)ok=0;
   if(!ok)break;
  }
  if(ferror(in))ok=0;
  if(fclose(in)!=0)ok=0;
  if(!found&&ok){
   /* Appending to a file without a trailing newline must not join records. */
   if(fprintf(out,"\n")<0||!slot_line(out,slot,apps,path))ok=0;
  }
 }else{
  for(int i=0;i<EH7_SLOT_COUNT&&ok;i++)
   if(!slot_line(out,i,apps,i==slot?path:apps->slots[i].path))ok=0;
 }
 if(fflush(out)!=0)ok=0;
 if(fclose(out)!=0)ok=0;
 if(!ok){remove(temp);return 0;}
 /* Non-destructive: do not delete original even if replacement rename fails. */
 if(rename(temp,cfg)!=0){remove(temp);return 0;}
 int already=apps->slots[slot].path[0]!=0;
 strcpy(apps->slots[slot].path,path);
 if(!already)apps->enabled++;
 apps->file_loaded=1;
 return 1;
}
