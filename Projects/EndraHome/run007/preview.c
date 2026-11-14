#include "../run005/render.h"
#include "ui.h"
#include "overlay.h"
#include <stdint.h>
#include <stdio.h>
static uint16_t fb[320*240];
static int image(const char *path){
 FILE *f=fopen(path,"wb");if(!f)return 1;
 fprintf(f,"P6\n320 240\n255\n");
 for(int i=0;i<320*240;i++){
  uint16_t c=fb[i];
  unsigned char rgb[3]={
   (unsigned char)((((c>>11)&31)*255)/31),
   (unsigned char)((((c>>5)&63)*255)/63),
   (unsigned char)(((c&31)*255)/31)
  };
  fwrite(rgb,1,3,f);
 }
 fclose(f);return 0;
}
int main(void){
 EhUI7Launch s;eh7launch_init(&s);eh5_render(fb,&s.page);
 if(image("build/home.ppm"))return 1;
 eh7launch_handle(&s,EH_KEY_RIGHT);
 eh7launch_handle(&s,EH_KEY_ENTER);
 eh5_render(fb,&s.page);
 if(image("build/zip-details.ppm"))return 1;
 eh7launch_handle(&s,EH_KEY_ENTER);
 eh5_render(fb,&s.page);
 eh7launch_dialog(fb,s.confirm_target);
 return image("build/zip-confirm.ppm");
}
