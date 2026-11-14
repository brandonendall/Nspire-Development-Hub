/* EndraHome Run013: assign .tns shortcut via bounded Documents picker.
 * Run012 immediate home launch, Run011 return-home and Run010 fast shutdown
 * remain preserved. Never transfer control while LCD hardware is owned.
 */
#include "../gba-ui/platform.h"
#include "registry.h"
#include "ui.h"
#include "render.h"
#include "overlay.h"
#include "picker.h"
#include "config_save.h"
#include "session.h"
#include <libndls.h>
#include <string.h>
#include <stdio.h>
void __attribute__((weak)) _fini(void){}
static void paint(const EhUniversalUI *s,const Eh13Picker *picker){
 uint16_t *fb=eh_framebuffer();
 if(picker->active)eh13_picker_render(fb,picker,s->apps);
 else{
  eh7_render(fb,&s->page,s->apps);
  eh7_overlay(fb,s);
 }
 eh_platform_flip();
}
static void launch_selected(const EhUniversalUI *ui,int argc,char **argv){
 int slot=ui->request-1;
 if(slot<0||slot>=EH7_SLOT_COUNT)return;
 const char *path=ui->apps->slots[slot].path;
 if(!eh7_valid_path(path)){
  show_msgbox("EndraHome 013","Invalid path. Reassign under Menu.");return;
 }
 if(argc>0&&argv&&argv[0]&&!strcmp(path,argv[0])){
  show_msgbox("EndraHome 013","Cannot launch EndraHome itself.");return;
 }
 int result=nl_exec(path,0,NULL);
 if(result<0)show_msgbox("EndraHome 013","App failed. Check assigned path and Ndless.");
}
int main(int argc,char **argv){
 EhRegistry apps;eh7_registry_init(&apps);
 char cfg[512]={0};
 if(argc>0&&argv&&eh7_config_path(cfg,sizeof(cfg),argv[0]))
  eh7_registry_load(&apps,cfg);
 EhUniversalUI ui;eh7_ui_init(&ui,&apps);
 Eh13Picker picker;memset(&picker,0,sizeof(picker));
 for(;;){
  if(!eh_platform_start())return 1;
  paint(&ui,&picker);
  while(!ui.page.should_exit&&!ui.request){
   EhKey key=eh_platform_readkey();
   if(picker.active){
    char chosen[EH7_PATH_CAP];
    int action=eh13_picker_handle(&picker,key,chosen,sizeof(chosen));
    if(action==2){
     int slot=picker.slot;
     if(argc>0&&argv&&argv[0]&&!strcmp(chosen,argv[0])){
      strcpy(ui.notice,"CANNOT ASSIGN ENDRAHOME ITSELF");
     }else if(eh13_assign_save(&apps,slot,chosen,cfg)){
      strcpy(ui.notice,"SHORTCUT SAVED - ENTER LAUNCH");
      ui.page.page=EH_PAGE_HOME;
     }else{
      strcpy(ui.notice,"SAVE FAILED - CONFIG UNCHANGED");
      ui.page.page=EH_PAGE_DETAIL;
     }
     eh13_picker_close(&picker);
     paint(&ui,&picker);
    }else if(action==-1){
     eh13_picker_close(&picker);
     paint(&ui,&picker);
    }else if(action==1)paint(&ui,&picker);
   }else{
    if(eh7_ui_handle(&ui,key)){
     if(ui.assign_slot){
      int slot=ui.assign_slot-1;ui.assign_slot=0;
      if(!cfg[0]){
       strcpy(ui.notice,"CONFIG PATH UNAVAILABLE");
      }else if(!eh13_picker_open(&picker,slot,"/documents")){
       strcpy(ui.notice,"CANNOT OPEN DOCUMENTS FOLDER");
      }
     }
     if(!ui.request&&!ui.page.should_exit)paint(&ui,&picker);
    }
   }
   if(!ui.page.should_exit&&!ui.request)eh_platform_sleep();
  }
  eh13_picker_close(&picker);
  eh_platform_stop(); /* preserve fast no-refresh LCD/timer restore */
  if(ui.page.should_exit||!ui.request)return 0;
  launch_selected(&ui,argc,argv);
  /* Child finished: return to selected icon; never relaunch automatically. */
  eh11_resume_home(&ui);
 }
}
