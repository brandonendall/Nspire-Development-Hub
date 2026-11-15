#include "render.h"
#include "ui.h"
#include "overlay.h"
#include "picker.h"
#include "manager.h"
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
  fwrite(b,1,3,f);
 }
 fclose(f);return 0;
}
int main(void){
 EhRegistry reg;eh7_registry_init(&reg);
 eh7_registry_parse(&reg,"0=Game|/documents/Apps/test.tns");
 EhUniversalUI s;eh7_ui_init(&s,&reg);
 eh7_render(fb,&s.page,&reg);
 if(save("build/home.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_MENU);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/manager.ppm"))return 1;
 s.manager_item=3;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/info.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_ESC);s.manager_item=4;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/settings.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_ESC);s.manager_item=1;eh7_ui_handle(&s,EH_KEY_ENTER);
 eh16_ui_type(&s,'E');eh16_ui_type(&s,'n');eh16_ui_type(&s,'d');
 eh16_ui_type(&s,'r');eh16_ui_type(&s,'a');
 eh7_render(fb,&s.page,&reg);eh15_manager_render(fb,&s);
 if(save("build/rename.ppm"))return 1;
 Eh13Picker p;memset(&p,0,sizeof(p));
 /* Preview without scanning a host /documents path. */
 strcpy(p.dir,"/documents");strcpy(p.root,"/documents");
 p.slot=0;p.active=1;
 Eh13PickEntry entries[2];
 strcpy(entries[0].name,"GBA");entries[0].directory=1;
 strcpy(entries[1].name,"EndraHome_Test006.tns");entries[1].directory=0;
 p.items=entries;p.count=2;p.selected=1;
 eh13_picker_render(fb,&p,&reg);
 return save("build/picker.ppm");
}
