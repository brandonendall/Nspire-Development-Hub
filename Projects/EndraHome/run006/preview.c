#include "../run005/render.h"
#include "ui.h"
#include "overlay.h"
#include <stdio.h>
static uint16_t pixels[320*240];
static int save(const char *path){
 FILE *f=fopen(path,"wb");if(!f)return 1;
 fprintf(f,"P6\n320 240\n255\n");
 for(int i=0;i<320*240;i++){
  uint16_t c=pixels[i];unsigned char b[3]={
   (unsigned char)(((c>>11)&31)*255/31),
   (unsigned char)(((c>>5)&63)*255/63),
   (unsigned char)((c&31)*255/31)
  };fwrite(b,1,3,f);
 }fclose(f);return 0;
}
int main(void){
 EhUI6 s;eh6_init(&s);
 eh5_render(pixels,&s.page);
 if(save("build/home.ppm"))return 1;
 for(int j=0;j<5;j++)eh6_handle(&s,EH_KEY_RIGHT);
 eh6_handle(&s,EH_KEY_ENTER);
 eh5_render(pixels,&s.page);
 if(save("build/tools.ppm"))return 1;
 eh6_handle(&s,EH_KEY_ENTER);
 eh6_confirm_overlay(pixels);
 return save("build/confirm.ppm");
}
