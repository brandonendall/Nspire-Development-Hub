#include "../gba-ui/platform.h"
#include "registry.h"
#include "preferences.h"
#include "desktop.h"
#include "platform029.h"
#include "ui.h"
#include "render.h"
#include "overlay.h"
#include "picker.h"
#include "config_save.h"
#include "edit.h"
#include "manager.h"
#include "text_input.h"
#include "files.h"
#include "session.h"
#include "app_target.h"
#include <unistd.h>
#include <libndls.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
void __attribute__((weak)) _fini(void){}
/* BSS, not stack: 4KiB bounded text preview */
static Eh17Files built_files;
static void paint(const EhUniversalUI *s,const Eh13Picker *picker){
 uint16_t *fb=eh_framebuffer();
 if(built_files.active)eh17_files_render(fb,&built_files,s->apps);
 else if(picker->active){
  eh13_picker_render(fb,picker,s->apps);
  eh7_overlay(fb,s);
 }
 else{
  if(s->page.page==EH_PAGE_HOME)eh7_render(fb,&s->page,s->apps);
  else eh15_manager_render(fb,s);
  eh7_overlay(fb,s);
 }
 eh_platform_flip();
}
static void open_builtin(EhUniversalUI *ui){
 ui->open_files=0;
 if(!eh17_files_open(&built_files,"/documents"))
  eh_toast_set(&ui->notice,"Cannot open built-in Files");
}
/* Read-only probe for CX II CAS 6.4.0.74, using nTheme's OS49
 * fingerprint and Settings literal address. No foreign pointer dereference,
 * OS write, callback, hook or startup modification. */
static int native_settings_probe(void){
 if(nl_ndless_rev()<2022||nl_osid()!=49||nl_hwsubtype()!=2)return 3;
 if(*(const volatile uint32_t *)0x10000020u!=0x1042aee0u)return 3;
 return *(const volatile uint32_t *)0x10a41a0cu==0x10c44fc4u?1:2;
}
int main(int argc,char **argv){
 EhRegistry apps;eh7_registry_init(&apps);
 char cfg[512]={0}, self[EH7_PATH_CAP]={0};
 if(argc>0&&argv&&argv[0]){
  if(argv[0][0]=='/'){
   if(eh7_valid_path(argv[0]))strcpy(self,argv[0]);
  }else{
   char cwd[EH7_PATH_CAP];
   if(getcwd(cwd,sizeof(cwd))){
    int n=snprintf(self,sizeof(self),"%s/%s",cwd,argv[0]);
    if(n<0||(size_t)n>=sizeof(self)||!eh7_valid_path(self))self[0]=0;
   }
  }
 }
 Eh14SaveError recovery={EH14_OK,0};
 int shortcuts_writable=0;
 if(eh7_config_path(cfg,sizeof(cfg),self)){
  /* Recover the prior known-good config if an interrupted swap left only
   * EHOld.cfg.tns; recovery never destroys or overwrites existing cfg. */
  shortcuts_writable=eh14_recover_config(cfg,&recovery);
  if(shortcuts_writable)eh7_registry_load(&apps,cfg);
 }
 if(!apps.has_appearance)eh_prefs_load(cfg,&apps.appearance);
 EhUniversalUI ui;eh7_ui_init(&ui,&apps);
 if(recovery.stage!=EH14_OK)eh_toast_set(&ui.notice,eh14_save_stage_name(recovery.stage));
 Eh13Picker picker;memset(&picker,0,sizeof(picker));
 for(;;){
  if(!eh_platform_start())return 1;
  paint(&ui,&picker);
  while(!ui.page.should_exit&&!ui.request){
   EhKey key=eh_platform_readkey();
   if(eh_platform_exit_chord()){ui.page.should_exit=1;break;}
   uint32_t now=eh_platform_ticks();
   int expired=eh_toast_tick(&ui.notice,now);
   expired|=eh_toast_tick(&built_files.status,now);
   if(expired)paint(&ui,&picker);
   int typed=eh16_textkey(); /* read each frame; one fresh QWERTY event */
   if(built_files.active){
    int action=0;
    int zoom=eh18_zoomkey();
    if(built_files.screen==3&&zoom)
     action=eh18_image_zoom(&built_files.image,zoom);
    if(built_files.screen==3){
     int dx=0,dy=0;
     int contact=eh_platform_touch_pan(&dx,&dy);
     if(contact){
      action|=eh18_image_pan(&built_files.image,-dx*2,dy*2);
      /* Touchpad movement is not a physical arrow press. */
      if(key==EH_KEY_UP||key==EH_KEY_DOWN||key==EH_KEY_LEFT||key==EH_KEY_RIGHT)key=EH_KEY_NONE;
     }
    }
    action|=eh21_files_text(&built_files,typed);
    action|=eh17_files_handle(&built_files,key);
    if(action)paint(&ui,&picker);
   }else if(picker.active){
    char chosen[EH7_PATH_CAP];
    int action=eh13_picker_handle(&picker,key,chosen,sizeof(chosen));
    if(action==2){
     int slot=picker.slot;
     EhAppStatus status=eh_app_target(chosen,self);
     if(status!=EH_APP_READY){
      eh_toast_set(&ui.notice,eh_app_status(status));
     }else{
      Eh14SaveError diagnostic={EH14_OK,0};
      if(eh_assign_app(&apps,slot,chosen,picker.adding_app,cfg,&diagnostic)){
       eh_assignment_complete(&ui,slot);
       eh13_picker_close(&picker);
       eh_toast_set(&ui.notice,diagnostic.stage==EH14_BACKUP_RETAINED
         ?"SAVED - BACKUP RETAINED":"SHORTCUT SAVED - ENTER LAUNCH");
      }else{
       char message[sizeof(ui.notice.text)];
       snprintf(message,sizeof(message),"%s (%d)",eh14_save_stage_name(diagnostic.stage),diagnostic.os_error);
       eh_toast_set(&ui.notice,message);
      }
     }
     paint(&ui,&picker);
    }else if(action==-1){
     ui.notice.text[0]=0;
     eh13_picker_close(&picker);
     paint(&ui,&picker);
    }else if(action==1){ui.notice.text[0]=0;paint(&ui,&picker);}
   }else{
    int redraw=eh7_ui_handle(&ui,key);
    /* Physical typing remains in our own dark UI, no OS text popup.
     * Calling after the key handler permits Enter-to-save immediately. */
    if(ui.renaming&&typed)redraw|=eh16_ui_type(&ui,typed);
    if(redraw){
     if(ui.open_files)open_builtin(&ui);
     if(ui.assign_slot){
      int slot=ui.assign_slot-1,adding=ui.adding_app;
      ui.assign_slot=ui.adding_app=0;
      if(!shortcuts_writable){
       eh_toast_set(&ui.notice,"CONFIG PATH UNAVAILABLE");
      }else if(!eh13_picker_open(&picker,slot,"/documents")){
       eh_toast_set(&ui.notice,"CANNOT OPEN DOCUMENTS FOLDER");
      }else{
       picker.adding_app=adding;
       ui.notice.text[0]=0;
      }
     }
     if(ui.edit_action){
      int action=ui.edit_action;ui.edit_action=EH15_NONE;
      Eh14SaveError diag={EH14_OK,0};
      int slot=ui.page.selected,ok=0;
      if(action==EH15_NATIVE_CHECK){
       ui.native_status=native_settings_probe();
      }else if(action==EH15_THEME){
       if(shortcuts_writable&&eh_save_appearance(&apps,ui.preview,cfg,&diag)){
        eh_toast_set(&ui.notice,"Appearance saved");
        ui.page.page=EH_PAGE_HOME;
        ui.page.selected=eh_desktop_select(&apps,slot,EH_KEY_NONE);
       }else eh_toast_set(&ui.notice,shortcuts_writable?eh14_save_stage_name(diag.stage):"Config path unavailable");
      }else{
       if(shortcuts_writable && action==EH15_RENAME)
        ok=eh15_update_shortcut(&apps,slot,ui.rename_input,NULL,cfg,&diag);
       else if(shortcuts_writable && action==EH15_REMOVE)
        ok=eh15_update_shortcut(&apps,slot,NULL,"",cfg,&diag);
       if(ok){
        const char *message=action==EH15_RENAME?"Shortcut renamed":
          slot==EH_FILES_SLOT?"Built-in Files enabled":"Shortcut unassigned";
        eh_toast_set(&ui.notice,message);
        ui.page.selected=eh_desktop_select(&apps,slot,EH_KEY_NONE);
        ui.page.page=EH_PAGE_HOME;
       }else eh_toast_set(&ui.notice,shortcuts_writable?eh14_save_stage_name(diag.stage):"Config path unavailable");
      }
     }
     if(ui.request){
      EhAppStatus status=eh_app_target(apps.slots[ui.request-1].path,self);
      if(status!=EH_APP_READY){
       ui.request=0;
       eh_toast_set(&ui.notice,eh_app_status(status));
      }
     }
     if(!ui.request&&!ui.page.should_exit)paint(&ui,&picker);
    }
   }
   if(!ui.page.should_exit&&!ui.request)eh_platform_sleep();
  }
  eh13_picker_close(&picker);
  eh17_files_close(&built_files);
  eh_platform_stop(); /* preserve fast no-refresh LCD/timer restore */
  if(ui.page.should_exit||!ui.request)return 0;
  int result=nl_exec(apps.slots[ui.request-1].path,0,NULL);
  eh_return_from_launch(&ui,result);
 }
}
