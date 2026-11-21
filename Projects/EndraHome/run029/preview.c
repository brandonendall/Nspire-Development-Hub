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
 EhRegistry photo;eh7_registry_init(&photo);
 eh7_registry_parse(&photo,"0=GBA|/documents/GBA/EndraGBA.tns");
 photo.appearance.wallpaper=4;
 photo.appearance.icon=8;
 EhUniversalUI selected;eh7_ui_init(&selected,&photo);
 for(int scene=0;scene<2;scene++){
  photo.appearance.theme=scene?EH_BLACK:EH_BLUE;
  selected.page.selected=0;
  eh7_render(fb,&selected.page,&photo);
  if(save(scene?"build/ring_black_gba.ppm":"build/ring_blue_gba.ppm"))return 1;
  eh7_ui_handle(&selected,EH_KEY_RIGHT);
  eh7_render(fb,&selected.page,&photo);
  if(save(scene?"build/ring_black_files.ppm":"build/ring_blue_files.ppm"))return 1;
 }
 reg.appearance.theme=EH_RED;
 eh7_render(fb,&s.page,&reg);if(save("build/home_red.ppm"))return 1;
 for(unsigned i=0;i<EH_WALLPAPER_COUNT;i++){
  char name[40];
  empty.appearance.wallpaper=i;
  for(unsigned variant=0;variant<2;variant++){
   empty.appearance.theme=variant?EH_GREEN:EH_BLUE;
   eh7_render(fb,&only.page,&empty);
   snprintf(name,sizeof(name),"build/wallpaper_%u_%s.ppm",i+1,variant?"green":"blue");
   if(save(name))return 1;
  }
 }
 reg.appearance.theme=0;
 eh7_ui_handle(&s,EH_KEY_MENU);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/manager.ppm"))return 1;
 s.manager_item=EH_MANAGER_INFO;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/info.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_ESC);s.manager_item=EH_MANAGER_APPEARANCE;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/settings.ppm"))return 1;
 for(unsigned i=0;i<EH_THEME_COUNT;i++){
  char name[48];
  s.preview.theme=i;
  eh15_manager_render(fb,&s);
  snprintf(name,sizeof(name),"build/appearance_%s.ppm",eh_theme_names[i]);
  if(save(name))return 1;
  empty.appearance=(EhAppearance){.theme=i,.wallpaper=3,.icon=9};
  eh7_render(fb,&only.page,&empty);
  snprintf(name,sizeof(name),"build/theme_%s.ppm",eh_theme_names[i]);
  if(save(name))return 1;
 }
 s.setting_row=EH_LOOK_ICON;
 for(unsigned i=0;i<EH_ICON_COUNT;i++){
  char name[40];
  s.preview.theme=i&1?EH_GREEN:EH_BLUE;
  s.preview.wallpaper=i%EH_WALLPAPER_COUNT;
  s.preview.icon=i;
  eh15_manager_render(fb,&s);
  snprintf(name,sizeof(name),"build/icon_%02u.ppm",i+1);
  if(save(name))return 1;
 }

 eh7_ui_handle(&s,EH_KEY_ESC);s.manager_item=EH_MANAGER_RENAME;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh16_ui_type(&s,'E');eh16_ui_type(&s,'n');eh16_ui_type(&s,'d');
 eh16_ui_type(&s,'r');eh16_ui_type(&s,'a');
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/rename.ppm"))return 1;
 Eh13Picker p;memset(&p,0,sizeof(p));
 /* Preview without scanning a host /documents path. */
 strcpy(p.dir,"/documents");strcpy(p.root,"/documents");
 p.slot=0;p.active=1;
 Eh13PickEntry entries[]={
   {.name="Games",.directory=1},{.name="Notes.TXT.TNS"},{.name="Backup.ZIP.TNS"},
   {.name="Photo.JPG.TNS"},{.name="Manual.PDF.TNS"},{.name="Look.CFG.TNS"},
   {.name="Table.CSV.TNS"},{.name="Quest.GBZ.TNS"},{.name="Tool.Zehn.TNS"},
   {.name="Scratchpad.tns"},{.name="Data.bin"}
 };
 p.items=entries;p.count=(int)(sizeof(entries)/sizeof(entries[0]));p.selected=1;
 eh13_picker_render(fb,&p,&reg);
 if(save("build/picker.ppm"))return 1;
 p.adding_app=1;eh13_picker_render(fb,&p,&reg);
 if(save("build/picker_add.ppm"))return 1;
 p.adding_app=0;
 p.show_all=1;entries[1].bytes=111282;
 eh13_picker_render(fb,&p,&reg);
 if(save("build/files.ppm"))return 1;
 for(unsigned variant=0;variant<2;variant++){
  const char *theme=variant?"white":"black";
  reg.appearance.theme=variant?EH_WHITE:EH_BLACK;
  reg.appearance.font=variant?0:7;
  reg.appearance.folder=variant?5:7;
  for(int page=0;page<2;page++){
   char name[48];
   p.scroll=page?p.count-EH13_VIEW_ROWS:0;
   p.selected=p.scroll+1;
   eh13_picker_render(fb,&p,&reg);
   snprintf(name,sizeof(name),"build/files_%s_%d.ppm",theme,page+1);
   if(save(name))return 1;
  }
 }
 p.scroll=0;p.selected=1;
 reg.appearance=(EhAppearance){.theme=EH_BLUE};
 Eh17Files popup={0};popup.active=1;popup.nav=p;popup.menu_open=1;popup.menu_item=3;
 eh17_files_render(fb,&popup,&reg);
 if(save("build/files_menu.ppm"))return 1;
 Eh17Files editor={0};
 editor.active=1;editor.screen=EH_FILES_TEXT_EDIT;
 strcpy(editor.path,"/documents/Notes.txt.tns");
 strcpy(editor.content,"EndraHome Run043\n\nCheck the JPEG viewer and zoom.\nKeep notes here while testing.");
 editor.text_len=(unsigned)strlen(editor.content);
 eh_text_begin(&editor.text_edit,editor.content,editor.text_len);
 editor.text_edit.cursor=editor.text_len;
 eh21_files_text(&editor,'.');
 eh17_files_render(fb,&editor,&reg);if(save("build/text_editor.ppm"))return 1;
 eh17_files_handle(&editor,EH_KEY_ESC);
 eh17_files_render(fb,&editor,&reg);if(save("build/text_discard.ppm"))return 1;
 reg.appearance.theme=EH_RED;reg.appearance.wallpaper=2;eh17_files_render(fb,&popup,&reg);
 if(save("build/files_menu_red.ppm"))return 1;
 eh7_ui_init(&s,&reg);eh7_ui_handle(&s,EH_KEY_MENU);
 eh15_manager_render(fb,&s);if(save("build/manager_red.ppm"))return 1;
 eh7_render(fb,&only.page,&empty);
 empty.appearance.theme=EH_GREEN;empty.appearance.wallpaper=0;
 eh7_render(fb,&only.page,&empty);
 eh_desktop_clock(fb,11*3600+58*60+37);
 if(save("build/clock.ppm"))return 1;
 s.page.page=EH_PAGE_HELP;s.subview=2;s.clock_draft=(11*60+47)*60+26;s.clock_field=2;
 eh15_manager_render(fb,&s);
 if(save("build/set_clock.ppm"))return 1;
 photo.appearance.theme=EH_ORANGE;photo.appearance.icon_color=EH_GREEN+1;
 eh7_render(fb,&selected.page,&photo);eh_desktop_clock(fb,12*3600+206);
 if(save("build/orange_green_icons.ppm"))return 1;
 s.subview=1;s.preview=photo.appearance;s.setting_row=EH_LOOK_ICON_COLOR;
 eh15_manager_render(fb,&s);return save("build/icon_color_setting.ppm");
}
