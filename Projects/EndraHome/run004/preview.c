/*
 * Host-only deterministic rendered preview, no calculator required.
 * Generates PPM images for all four screens (home, detail, about, help).
 */
#include "render.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static uint16_t pixels[320*240];
static int save(const char *name){
  FILE *f=fopen(name,"wb");
  if(!f)return 1;
  fprintf(f,"P6\n320 240\n255\n");
  for(int i=0;i<320*240;i++){
    uint16_t c=pixels[i];
    unsigned char rgb[3]={
      (unsigned char)(((c>>11)&31)*255/31),
      (unsigned char)(((c>>5)&63)*255/63),
      (unsigned char)((c&31)*255/31)
    };
    fwrite(rgb,1,3,f);
  }
  fclose(f);return 0;
}
int main(void){
  EhUI s;eh_ui_init(&s);
  eh_render004(pixels,&s);
  if(save("build/home.ppm"))return 1;
  eh_ui_dispatch(&s,EH_KEY_RIGHT);
  eh_ui_dispatch(&s,EH_KEY_ENTER);
  eh_render004(pixels,&s);
  if(save("build/detail.ppm"))return 1;
  eh_ui_dispatch(&s,EH_KEY_ENTER);
  eh_render004(pixels,&s);
  if(save("build/about.ppm"))return 1;
  eh_ui_dispatch(&s,EH_KEY_MENU);
  eh_render004(pixels,&s);
  return save("build/help.ppm");
}
