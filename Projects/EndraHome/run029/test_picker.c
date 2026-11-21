#include "picker.h"
#include "config_save.h"
#include "edit.h"
#include "session.h"
#include "app_target.h"
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
 snprintf(file,sizeof(file),"%s/Example.TNS",sub);
 snprintf(cfg,sizeof(cfg),"%s/EndraHome_Apps.cfg.tns",folder);
 assert(!mkdir(folder,0700));
 assert(!mkdir(sub,0700));
 write_file(file,"Zehn test dummy bytes");
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
 assert(eh_app_file(chosen)==EH_APP_READY);
 assert(eh_app_file(other)==EH_APP_DOCUMENT);
 assert(eh_app_file(sub)==EH_APP_DOCUMENT);
 assert(eh_app_file("/missing-eh40.tns")==EH_APP_MISSING);
 assert(eh_app_target("/documents/self.TNS","/Documents/Self.tns")==EH_APP_SELF);
 assert(eh_app_target("/documents/../self.tns","")==EH_APP_INVALID);
 assert(eh13_picker_handle(&p,EH_KEY_ESC,chosen,sizeof(chosen))==1);
 assert(!strcmp(p.dir,folder));
 assert(eh13_picker_handle(&p,EH_KEY_ESC,chosen,sizeof(chosen))==-1);
 eh13_picker_close(&p);
 EhRegistry reg;eh7_registry_init(&reg);
 assert(eh7_registry_parse(&reg,"5=Tools|/documents/EndraHome_Test006.tns")==1);
 write_file(cfg,"# Keep my comments\n  5=Tools|/documents/EndraHome_Test006.tns\n0=MyGBA|/documents/GBA/Game.tns\n");
 /* Mirror the real launcher: load saved labels and other slots before edit. */
 assert(eh7_registry_load(&reg,cfg)>=0);
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
 snprintf(oldbak,sizeof(oldbak),"%s/EH40Old.cfg.tns",folder);
 snprintf(temp,sizeof(temp),"%s/EH40New.cfg.tns",folder);
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

 char legacy_backup[256];
 snprintf(legacy_backup,sizeof(legacy_backup),"%s/EHOld.cfg.tns",folder);
 assert(rename(cfg,legacy_backup)==0);
 assert(eh14_recover_config(cfg,&err));
 write_file(legacy_backup,"# Legacy evidence must be preserved\n");
 assert(eh_assign_app(&reg,2,"/documents/Apps/New Program.TNS",1,cfg,&err));
 assert(!access(legacy_backup,F_OK));
 assert(!strcmp(reg.slots[2].label,"New Program"));
 assert(!eh_assign_app(&reg,2,"/documents/Apps/Overwrite.tns",1,cfg,&err));
 assert(!eh_assign_app(&reg,EH_FILES_SLOT,"/documents/Apps/Wrong.tns",1,cfg,&err));
 EhUniversalUI ui;eh7_ui_init(&ui,&reg);
 eh_assignment_complete(&ui,2);
 assert(ui.page.selected==2&&ui.page.page==EH_PAGE_HOME);
 assert(eh7_ui_handle(&ui,EH_KEY_ENTER)&&ui.request==3);
 assert(eh_assign_app(&reg,2,"/documents/Other Folder/Replacement.tns",0,cfg,&err));
 assert(!strcmp(reg.slots[2].label,"New Program"));
 assert(!strcmp(reg.slots[2].path,"/documents/Other Folder/Replacement.tns"));
 remove(legacy_backup);

 assert(eh15_update_shortcut(&reg,5,"MY TOOL",NULL,cfg,&err));
 assert(!strcmp(reg.slots[5].label,"MY TOOL"));
 assert(eh15_update_shortcut(&reg,5,NULL,"",cfg,&err));
 assert(!reg.slots[5].path[0]);
 assert(!eh15_update_shortcut(&reg,5,"BAD|LABEL",NULL,cfg,&err));
 assert(err.stage==EH14_INVALID);
 EhRegistry verify;eh7_registry_init(&verify);
 assert(eh7_registry_load(&verify,cfg)>=0);
 assert(!strcmp(verify.slots[5].label,"MY TOOL"));
 assert(!verify.slots[5].path[0]);
 assert(!strcmp(verify.slots[0].label,"MyGBA"));
 assert(!strcmp(verify.slots[0].path,"/documents/GBA/Game.tns"));
 assert(!access(file,F_OK)); /* removing shortcut did not delete app file */
 remove(cfg);remove(file);remove(other);rmdir(sub);rmdir(folder);
 puts("PASS Add/Reassign, absolute picker paths, signatures, transactional save and recovery");
 return 0;
}
