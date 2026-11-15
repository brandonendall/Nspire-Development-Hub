/* EndraHome Run018 image renderer.
 * mViewer CX Edition 4.1 source supplied by the owner was studied for its
 * zoom, pan and exit controls. We do NOT copy its legacy launcher, browser,
 * LCD/hook ownership, or GPL-licensed implementation. JPEG/PNG decoding
 * uses the repository's existing stb_image.h, MIT/public-domain licensed.
 * Only bounded static memory plus image pixels are held between frames.
 */
#include "image_viewer.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_SIMD
#define STBI_NO_HDR
#define STBI_MAX_DIMENSIONS 2048
#include "stb_image.h"
extern char font8x8_basic[128][8];
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static void box(uint16_t *f,int x,int y,int w,int h,uint16_t c){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=320||y>=240||w<=0||h<=0)return;
 if(x+w>320)w=320-x;
 if(y+h>240)h=240-y;
 for(int yy=y;yy<y+h;yy++)for(int xx=x;xx<x+w;xx++)f[yy*320+xx]=c;
}
static void text(uint16_t *f,int x,int y,const char *s,uint16_t c,int max){
 for(int i=0;s&&s[i]&&i<max;i++){
  unsigned ch=(unsigned char)s[i];if(ch>127)ch='?';
  for(int a=0;a<8;a++){
   unsigned bits=(unsigned char)font8x8_basic[ch][a];
   for(int b=0;b<8;b++)if(bits&(1u<<b))box(f,x+i*8+b,y+a,1,1,c);
  }
 }
}
static int ext(const char *s,const char *suf){
 size_t n=strlen(s),m=strlen(suf);
 if(n<m)return 0;
 for(size_t i=0;i<m;i++){
  char a=s[n-m+i],b=suf[i];
  if(a>='A'&&a<='Z')a=(char)(a+32);
  if(a!=b)return 0;
 }
 return 1;
}
int eh18_is_image_name(const char *s){
 if(!s)return 0;
 return ext(s,".jpg")||ext(s,".jpeg")||ext(s,".png")||
        ext(s,".jpg.tns")||ext(s,".jpeg.tns")||ext(s,".png.tns");
}
void eh18_image_close(Eh18Image *v){
 if(!v)return;
 free(v->pixels);
 memset(v,0,sizeof(*v));
}
static void fail(Eh18Image *v,const char *message){
 strncpy(v->error,message,sizeof(v->error)-1);
 v->error[sizeof(v->error)-1]=0;
}
static int fitted(const Eh18Image *v){
 int a=(32000+v->width-1)/v->width,b=(24000+v->height-1)/v->height;
 /* Preserve the full photo and its aspect ratio; zoom enables panning. */
 int z=a<b?a:b;
 if(z<10)z=10;
 if(z>100)z=100;
 return z;
}
static void clamp_pan(Eh18Image *v){
 int w=(v->width*v->zoom+50)/100,h=(v->height*v->zoom+50)/100;
 if(w<=320)v->pan_x=0;
 else{
  int allowance=(w-320+1)/2;
  if(v->pan_x>allowance)v->pan_x=allowance;
  if(v->pan_x< -allowance)v->pan_x= -allowance;
 }
 if(h<=240)v->pan_y=0;
 else{
  int allowance=(h-240+1)/2;
  if(v->pan_y>allowance)v->pan_y=allowance;
  if(v->pan_y< -allowance)v->pan_y= -allowance;
 }
}
int eh18_image_open(Eh18Image *v,const char *path){
 if(!v)return 0;
 eh18_image_close(v);
 if(!path||!eh18_is_image_name(path)){fail(v,"UNSUPPORTED IMAGE TYPE");return 0;}
 struct stat st;
 if(stat(path,&st)!=0||!S_ISREG(st.st_mode)||st.st_size<1){
  fail(v,"CANNOT READ IMAGE");return 0;
 }
 if((unsigned long)st.st_size>EH18_IMAGE_FILE_LIMIT){
  fail(v,"IMAGE EXCEEDS 5MB LIMIT");return 0;
 }
 int w=0,h=0,ch=0;
 if(!stbi_info(path,&w,&h,&ch)||w<=0||h<=0){
  fail(v,"INVALID JPEG/PNG DATA");return 0;
 }
 if(w>2048||h>2048||(unsigned long)w*(unsigned long)h>EH19_IMAGE_DECODE_PIXELS_LIMIT){
  fail(v,"IMAGE EXCEEDS 2MP LIMIT");return 0;
 }
 unsigned char *decoded=stbi_load(path,&w,&h,&ch,3);
 if(!decoded){
  fail(v,"JPEG/PNG DECODE FAILED");return 0;
 }
 if(w<1||h<1||(unsigned long)w*(unsigned long)h>EH19_IMAGE_DECODE_PIXELS_LIMIT){
  stbi_image_free(decoded);
  fail(v,"INVALID IMAGE DIMENSIONS");return 0;
 }
 /* Run019: retain at most 1MP RGB565. Decode ceiling is 2MP, so peak
  * temporary RGB allocation is bounded; this is NOT streaming JPEG decode.
  * Nearest-neighbor sampling avoids a second full-resolution pixel buffer. */
 int step=1;
 while(((unsigned long)((w+step-1)/step))*((unsigned long)((h+step-1)/step))>EH18_IMAGE_PIXELS_LIMIT)step++;
 int outw=(w+step-1)/step,outh=(h+step-1)/step;
 size_t count=(size_t)outw*outh;
 uint16_t *packed=(uint16_t*)malloc(count*sizeof(uint16_t));
 if(!packed){
  stbi_image_free(decoded);fail(v,"NOT ENOUGH MEMORY");return 0;
 }
 for(int y=0;y<outh;y++)for(int x=0;x<outw;x++){
  const unsigned char *rgb=decoded+3*((size_t)(y*step)*w+x*step);
  packed[(size_t)y*outw+x]=(uint16_t)((((unsigned)rgb[0]>>3)<<11)|
       (((unsigned)rgb[1]>>2)<<5)|((unsigned)rgb[2]>>3));
 }
 stbi_image_free(decoded);
 v->pixels=packed;v->width=outw;v->height=outh;
 v->fit_zoom=fitted(v);v->zoom=v->fit_zoom;v->pan_x=v->pan_y=0;
 v->error[0]=0;
 return 1;
}
int eh18_image_zoom(Eh18Image *v,int direction){
 if(!v||!v->pixels||!direction)return 0;
 int before=v->zoom;
 int step=before/4;if(step<10)step=10;
 int after=before+(direction>0?step:-step);
 if(after<10)after=10;
 if(after>400)after=400;
 if(after==before)return 0;
 /* Keep the image center anchored as zoom changes; pan in view pixels. */
 v->pan_x=(int)((long)v->pan_x*after/before);
 v->pan_y=(int)((long)v->pan_y*after/before);
 v->zoom=after;clamp_pan(v);
 return 1;
}
int eh18_image_key(Eh18Image *v,EhKey key){
 if(!v||!v->pixels)return 0;
 int oldx=v->pan_x,oldy=v->pan_y;
 if(key==EH_KEY_LEFT)v->pan_x+=20;
 else if(key==EH_KEY_RIGHT)v->pan_x-=20;
 else if(key==EH_KEY_UP)v->pan_y+=20;
 else if(key==EH_KEY_DOWN)v->pan_y-=20;
 else if(key==EH_KEY_ENTER){
  v->zoom=v->fit_zoom;
  v->pan_x=v->pan_y=0;
  return 1;
 }else return 0;
 clamp_pan(v);
 return v->pan_x!=oldx||v->pan_y!=oldy;
}
void eh18_image_render(uint16_t *f,const Eh18Image *v){
 if(!f||!v||!v->pixels)return;
 box(f,0,0,320,240,RGB(8,12,23));
 int width=(v->width*v->zoom+50)/100;
 int height=(v->height*v->zoom+50)/100;
 int ox=(320-width)/2+v->pan_x;
 int oy=(240-height)/2+v->pan_y;
 int src_x[320],src_y[240];
 for(int x=0;x<320;x++){
  int rel=x-ox;
  src_x[x]=rel<0?-1:rel*100/v->zoom;
  if(src_x[x]>=v->width)src_x[x]=-1;
 }
 for(int y=0;y<240;y++){
  int rel=y-oy;
  src_y[y]=rel<0?-1:rel*100/v->zoom;
  if(src_y[y]>=v->height)src_y[y]=-1;
 }
 for(int y=0;y<240;y++){
  int sy=src_y[y];if(sy<0)continue;
  uint16_t *dst=f+y*320;
  const uint16_t *source=v->pixels+sy*v->width;
  for(int x=0;x<320;x++)if(src_x[x]>=0)dst[x]=source[src_x[x]];
 }
}

int eh18_image_pan(Eh18Image *v,int dx,int dy){
 if(!v||!v->pixels)return 0;
 int oldx=v->pan_x,oldy=v->pan_y;
 v->pan_x+=dx;v->pan_y+=dy;
 clamp_pan(v);
 return v->pan_x!=oldx||v->pan_y!=oldy;
}
