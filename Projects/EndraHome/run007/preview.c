#include "render.h"
#include "ui.h"
#include "overlay.h"
#include <stdio.h>
static uint16_t fb[320*240];
static int save(const char *name){
 FILE *f=fopen(name,"wb");
 if(!f)return 1;
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
 eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);
 if(save("build/detail.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh7_overlay(fb,&s);
 if(save("build/confirm.ppm"))return 1;
 eh7_ui_handle(&s,EH_KEY_ESC);
 eh7_ui_handle(&s,EH_KEY_ESC);
 eh7_ui_handle(&s,EH_KEY_RIGHT);
 eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_ui_handle(&s,EH_KEY_ENTER);
 eh7_render(fb,&s.page,&reg);eh7_overlay(fb,&s);
 return save("build/unconfigured.ppm");
}
