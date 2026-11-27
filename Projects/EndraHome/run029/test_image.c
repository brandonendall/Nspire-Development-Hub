#include "image_viewer.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "font8x8_basic.h"
/* Exact real 4x4 baseline grayscale JPEG (342 B); generated with Pillow. */
static const char jpg_b64[]="/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAoHBwgHBgoICAgLCgoLDhgQDg0NDh0VFhEYIx8lJCIfIiEmKzcvJik0KSEiMEExNDk7Pj4+JS5ESUM8SDc9Pjv/wAALCAAEAAQBAREA/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/9oACAEBAAA/AEZmdy7sWZjkknJJr//Z";
static int dec(char c){
 if(c>='A'&&c<='Z')return c-'A';
 if(c>='a'&&c<='z')return c-'a'+26;
 if(c>='0'&&c<='9')return c-'0'+52;
 if(c=='+')return 62;
 if(c=='/')return 63;
 return -1;
}
static size_t decode(const char *p,unsigned char *out,size_t cap){
 unsigned data=0,bits=0;size_t n=0;
 for(;*p&&*p!='=';p++){
  int b=dec(*p);if(b<0)continue;
  data=(data<<6)|(unsigned)b;bits+=6;
  if(bits>=8){
   bits-=8;
   if(n>=cap)return 0;
   out[n++]=(unsigned char)(data>>bits);
  }
 }
 return n;
}
int main(void){
 unsigned char bytes[400];
 size_t n=decode(jpg_b64,bytes,sizeof(bytes));
 assert(n==342&&bytes[0]==0xff&&bytes[1]==0xd8);
 char path[128];snprintf(path,sizeof(path),"/tmp/eh18_%ld.jpg.tns",(long)getpid());
 FILE *f=fopen(path,"wb");assert(f);
 assert(fwrite(bytes,1,n,f)==n);assert(!fclose(f));
 assert(eh18_is_image_name(path));
 assert(eh18_is_image_name("landscape.PNG"));
 assert(!eh18_is_image_name("file.pdf"));
 Eh18Image img;memset(&img,0,sizeof(img));
 if(!eh18_image_open(&img,path))fprintf(stderr,"JPEG loading failure: %s\n",img.error);
 assert(img.pixels);
 assert(img.width==4&&img.height==4&&img.zoom==img.fit_zoom);
 uint16_t fb[320*240];memset(fb,0,sizeof(fb));
 eh18_image_render(fb,&img);
 int nonzero=0;
 for(int i=0;i<320*240;i++)if(fb[i])nonzero++;
 assert(nonzero>500);
 int old=img.zoom;
 assert(eh18_image_zoom(&img,1)&&img.zoom>old);
 assert(eh18_image_key(&img,EH_KEY_ENTER)&&img.zoom==img.fit_zoom);
 eh18_image_close(&img);assert(!img.pixels);
 assert(remove(path)==0);

 /* Verify full-color PNG decoding and RGB565 conversion in the same run. */
 static const char png_b64[]="iVBORw0KGgoAAAANSUhEUgAAAAQAAAAECAIAAAAmkwkpAAAAIUlEQVR4nGP8qWMvVTaHgYGBgYGBUXjRbQYYYGJAAigcALa5BCc6eQsfAAAAAElFTkSuQmCC";
 n=decode(png_b64,bytes,sizeof(bytes));
 assert(n==90);
 snprintf(path,sizeof(path),"/tmp/eh18_%ld.png.tns",(long)getpid());
 f=fopen(path,"wb");assert(f);
 assert(fwrite(bytes,1,n,f)==n);assert(!fclose(f));
 assert(eh18_image_open(&img,path));
 assert(img.width==4&&img.height==4&&img.pixels);
 uint16_t col=img.pixels[1];
 assert(col!=0);
 char app[128];snprintf(app,sizeof(app),"/tmp/eh18_%ld.tns",(long)getpid());
 memset(fb,0,sizeof(fb));assert(eh_app_icon_draw(fb,0,app,0,0));
 assert(fb[12]==col);
 assert(!eh_app_icon_draw(fb,0,"/missing/example.tns",0,0));
 eh18_image_close(&img);assert(remove(path)==0);

 snprintf(path,sizeof(path),"/tmp/eh18_%ld.jpg.tns",(long)getpid());
 f=fopen(path,"wb");assert(f);assert(fputs("BAD JPEG DATA",f)>=0);assert(!fclose(f));
 assert(!eh18_image_open(&img,path)&&img.error[0]);
 eh18_image_close(&img);assert(remove(path)==0);
 puts("Run018 real JPEG + PNG decode, RGB565 render, zoom, fit, invalid data PASSED");
 return 0;
}
