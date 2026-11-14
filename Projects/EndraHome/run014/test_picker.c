#include "picker.h"
#include "config_save.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
static void write_file(const char *path,const char *content){
 FILE *f=fopen(path,"wb");assert(f);
 assert(fputs(content,f)>=0);assert(!fclose(f));
}
int main(void){
 char folder[128],sub[160],file[192],cfg[192];
 snprintf(folder,sizeof(folder),"/tmp/eh013_%ld",(long)getpid());
 snprintf(sub,sizeof(sub),"%s/Apps",folder);
 snprintf(file,sizeof(file),"%s/Example.tns",sub);
 snprintf(cfg,sizeof(cfg),"%s/EndraHome_Apps.cfg.tns",folder);
 assert(!mkdir(folder,0700));
 assert(!mkdir(sub,0700));
 write_file(file,"test dummy bytes");
 char other[192];
 snprintf(other,sizeof(other),"%s/not-an-app.txt",folder);
 write_file(other,"not an application");
 Eh13Picker p;
 assert(eh13_picker_open(&p,5,folder));
 assert(p.count==1&&p.items[0].directory);
 char chosen[EH7_PATH_CAP];
 assert(eh13_picker_handle(&p,EH_KEY_ENTER,chosen,sizeof(chosen))==1);
 assert(p.count==1&&!p.items[0].directory);
 assert(eh13_picker_handle(&p,EH_KEY_ENTER,chosen,sizeof(chosen))==2);
 assert(!strcmp(chosen,file));
 assert(eh13_picker_handle(&p,EH_KEY_ESC,chosen,sizeof(chosen))==1);
 assert(!strcmp(p.dir,folder));
 assert(eh13_picker_handle(&p,EH_KEY_ESC,chosen,sizeof(chosen))==-1);
 eh13_picker_close(&p);
 EhRegistry reg;eh7_registry_init(&reg);
 assert(eh7_registry_parse(&reg,"5=Tools|/documents/EndraHome_Test006.tns")==1);
 write_file(cfg,"# Keep my comments\n5=Tools|/documents/EndraHome_Test006.tns\n0=MyGBA|/documents/GBA/Game.tns\n");
 Eh14SaveError err={EH14_OK,0};
 assert(eh14_assign_save(&reg,5,"/documents/Apps/NewTool.tns",cfg,&err));
 assert(!strcmp(reg.slots[5].path,"/documents/Apps/NewTool.tns"));
 assert(!eh14_assign_save(&reg,5,"/documents/../danger.tns",cfg,&err));
 assert(err.stage==EH14_INVALID);
 assert(!strcmp(reg.slots[5].path,"/documents/Apps/NewTool.tns"));
 EhRegistry read;eh7_registry_init(&read);
 assert(eh7_registry_load(&read,cfg)>=0);
 assert(!strcmp(read.slots[0].label,"MyGBA"));
 assert(!strcmp(read.slots[0].path,"/documents/GBA/Game.tns"));
 assert(!strcmp(read.slots[5].path,"/documents/Apps/NewTool.tns"));
 FILE *f=fopen(cfg,"rb");assert(f);
 char buffer[512];size_t n=fread(buffer,1,sizeof(buffer)-1,f);buffer[n]=0;fclose(f);
 assert(strstr(buffer,"# Keep my comments"));
 assert(!strstr(buffer,"EndraHome_Test006"));
 assert(!eh14_assign_save(&reg,5,"/documents/Apps/test.tns","/tmp",&err));
 assert(err.stage==EH14_TEMP_OPEN||err.stage==EH14_PATHS);
 /* Safeguard: a stale backup aborts without touching live configuration. */
 char oldbak[256],temp[256];
 snprintf(oldbak,sizeof(oldbak),"%s/EHOld.cfg.tns",folder);
 snprintf(temp,sizeof(temp),"%s/EHNew.cfg.tns",folder);
 write_file(oldbak,"BACKUP MUST SURVIVE");
 assert(!eh14_assign_save(&reg,5,"/documents/Apps/Other.tns",cfg,&err));
 assert(err.stage==EH14_BACKUP_BUSY);
 assert(!strcmp(reg.slots[5].path,"/documents/Apps/NewTool.tns"));
 remove(oldbak);
 /* Safeguard: a stale staged file is never overwritten. */
 write_file(temp,"STAGE MUST SURVIVE");
 assert(!eh14_assign_save(&reg,5,"/documents/Apps/Other.tns",cfg,&err));
 assert(err.stage==EH14_TEMP_BUSY);
 remove(temp);
 /* Simulate a power loss after cfg -> backup, before stage -> cfg. */
 assert(rename(cfg,oldbak)==0);
 assert(!access(oldbak,F_OK));
 assert(access(cfg,F_OK)!=0);
 assert(eh14_recover_config(cfg,&err));
 assert(!access(cfg,F_OK));
 assert(access(oldbak,F_OK)!=0);
 assert(eh14_assign_save(&reg,5,"/documents/Apps/Another.tns",cfg,&err));
 assert(!strcmp(reg.slots[5].path,"/documents/Apps/Another.tns"));
 assert(!access(cfg,F_OK));
 assert(access(oldbak,F_OK)!=0);

 remove(cfg);remove(file);remove(other);rmdir(sub);rmdir(folder);
 puts("Run014 transaction save, recovery and picker tests passed");
 return 0;
}
