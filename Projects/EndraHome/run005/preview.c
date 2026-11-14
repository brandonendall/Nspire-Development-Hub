#include "render.h"
#include <stdint.h>
#include <stdio.h>
static uint16_t fb[320*240];
static int write_ppm(const char *path){
 FILE *f=fopen(path,"wb");if(!f)return 1;
 fprintf(f,"P6\n320 240\n255\n");
 for(int i=0;i<320*240;i++){
  uint16_t c=fb[i];
  unsigned char rgb[3]={(unsigned char)(((c>>11)&31)*255/31),
                        (unsigned char)(((c>>5)&63)*255/63),
                        (unsigned char)((c&31)*255/31)};
  fwrite(rgb,1,3,f);
 }
 fclose(f);return 0;
}
int main(void){
 EhUI7 ui;eh7_init(&ui);eh5_render(fb,&ui);
 if(write_ppm("build/home.ppm"))return 1;
 for(int i=0;i<4;i++)eh7_dispatch(&ui,EH_KEY_RIGHT);
 eh5_render(fb,&ui);if(write_ppm("build/selected.ppm"))return 1;
 eh7_dispatch(&ui,EH_KEY_ENTER);eh5_render(fb,&ui);
 if(write_ppm("build/detail.ppm"))return 1;
 eh7_dispatch(&ui,EH_KEY_MENU);eh5_render(fb,&ui);
 return write_ppm("build/help.ppm");
}
