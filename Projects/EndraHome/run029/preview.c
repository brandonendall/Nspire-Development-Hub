#include "render.h"
#include "art.h"
#include "ui.h"
#include "overlay.h"
#include "picker.h"
#include "manager.h"
#include "files.h"
#include <stdio.h>
#include <string.h>
static uint16_t fb[320*240];
static int save(const char *name){
 FILE *f=fopen(name,"wb");if(!f)return 1;
 fprintf(f,"P6\n320 240\n255\n");
 for(int i=0;i<320*240;i++){
  uint16_t c=fb[i];
  unsigned char b[3]={
   (unsigned char)(((c>>11)&31)*255/31),
   (unsigned char)(((c>>5)&63)*255/63),
   (unsigned char)((c&31)*255/31)
  };
  if(fwrite(b,1,3,f)!=3){fclose(f);return 1;}
 }
 return fclose(f)!=0;
}
int main(void){
 EhRegistry empty;eh7_registry_init(&empty);
 EhUniversalUI only;eh7_ui_init(&only,&empty);
 eh7_render(fb,&only.page,&empty);
 if(save("build/home_default.ppm"))return 1;
 EhRegistry reg;eh7_registry_init(&reg);
 eh7_registry_parse(&reg,"0=EndraGBA|/documents/Apps/test.tns");
 eh7_registry_parse(&reg,"1=EndraZIP|/documents/Apps/Zip.tns");
 eh7_registry_parse(&reg,"2=EndraCmd|/documents/Apps/Cmd.tns");
 eh7_registry_parse(&reg,"4=Images|/documents/Apps/Images.tns");
 eh7_registry_parse(&reg,"5=Tools|/documents/Apps/Tools.tns");
 EhUniversalUI s;eh7_ui_init(&s,&reg);
 eh7_render(fb,&s.page,&reg);
 if(save("build/home.ppm"))return 1;
 reg.appearance.theme=14;
 eh7_render(fb,&s.page,&reg);if(save("build/home_red.ppm"))return 1;
 for(unsigned i=0;i<EH_WALLPAPER_COUNT;i++){
  char name[40];
  empty.appearance.wallpaper=i;
  for(unsigned variant=0;variant<2;variant++){
   empty.appearance.theme=variant?5:0;
   eh7_render(fb,&only.page,&empty);
   snprintf(name,sizeof(name),"build/wallpaper_%u_%s.ppm",i+1,variant?"green":"ocean");
   if(save(name))return 1;
  }
 }
 reg.appearance.theme=0;
 eh7_ui_handle(&s,EH_KEY_MENU);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/manager.ppm"))return 1;
 s.manager_item=3;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/info.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_ESC);s.manager_item=4;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/settings.ppm"))return 1;
 s.setting_row=EH_LOOK_ICON;
 for(unsigned i=0;i<EH_ICON_COUNT;i++){
  char name[40];
  s.preview.theme=i&1?5:0;
  s.preview.wallpaper=i%EH_WALLPAPER_COUNT;
  s.preview.icon=i;
  eh15_manager_render(fb,&s);
  snprintf(name,sizeof(name),"build/icon_%02u.ppm",i+1);
  if(save(name))return 1;
 }

 eh7_ui_handle(&s,EH_KEY_ESC);s.manager_item=1;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh16_ui_type(&s,'E');eh16_ui_type(&s,'n');eh16_ui_type(&s,'d');
 eh16_ui_type(&s,'r');eh16_ui_type(&s,'a');
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/rename.ppm"))return 1;
 Eh13Picker p;memset(&p,0,sizeof(p));
 /* Preview without scanning a host /documents path. */
 strcpy(p.dir,"/documents");strcpy(p.root,"/documents");
 p.slot=0;p.active=1;
 Eh13PickEntry entries[2];memset(entries,0,sizeof(entries));
 strcpy(entries[0].name,"GBA");entries[0].directory=1;
 strcpy(entries[1].name,"EndraHome_Test006.tns");entries[1].directory=0;
 p.items=entries;p.count=2;p.selected=1;
 eh13_picker_render(fb,&p,&reg);
 if(save("build/picker.ppm"))return 1;
 p.show_all=1;entries[1].bytes=111282;
 eh13_picker_render(fb,&p,&reg);
 if(save("build/files.ppm"))return 1;
 Eh17Files popup={0};popup.active=1;popup.nav=p;popup.menu_open=1;popup.menu_item=3;
 eh17_files_render(fb,&popup,&reg);
 if(save("build/files_menu.ppm"))return 1;
 reg.appearance.theme=14;reg.appearance.wallpaper=2;eh17_files_render(fb,&popup,&reg);
 if(save("build/files_menu_red.ppm"))return 1;
 eh7_ui_init(&s,&reg);eh7_ui_handle(&s,EH_KEY_MENU);
 eh15_manager_render(fb,&s);if(save("build/manager_red.ppm"))return 1;
 s.manager_item=6;eh7_ui_handle(&s,EH_KEY_ENTER);s.native_status=2;
 eh15_manager_render(fb,&s);
 return save("build/native_shortcut.ppm");
}
