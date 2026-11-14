/* Run004: live OS-reported coarse battery level probe.
 * Read-only, fail closed when OS code/mappings don't validate.
 * Does not access charger, ADC or battery hardware registers.
 */
#include "../run002/platform.h"
#include "vendor/battery_native.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "font8x8_basic.h"
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
static uint16_t *fb;
static BatteryNative battery;
static BatteryNativeSnapshot sample;
static int readable;
static unsigned polls;
void __attribute__((weak)) _fini(void) {}
static void rect(int x,int y,int w,int h,uint16_t c){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
 if(x+w>EH_W)w=EH_W-x;if(y+h>EH_H)h=EH_H-y;
 for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)fb[j*EH_W+i]=c;
}
static void print8(int x,int y,const char *s,int n,uint16_t c){
 for(int i=0;s&&s[i]&&i<n;i++){
  unsigned char ch=(unsigned char)s[i];if(ch>127)ch='?';
  for(int r=0;r<8;r++){unsigned char b=font8x8_basic[ch][r];
   for(int k=0;k<8;k++)if(b&(1u<<k))rect(x+i*8+k,y+r,1,1,c);}
 }
}
static void display(unsigned step){
 uint16_t bg=RGB(9,17,29),panel=RGB(25,41,58),txt=RGB(238,244,249);
 uint16_t teal=RGB(66,224,199),dim=RGB(161,182,196);
 char b[40];
 fb=eh_framebuffer();rect(0,0,320,240,bg);
 rect(0,0,320,34,panel);rect(0,0,4,34,teal);
 print8(10,12,"ENDRA DIAGNOSTICS",18,teal);print8(275,12,"004",3,txt);
 print8(12,49,"BATTERY MONITOR (READ-ONLY)",32,txt);
 if(readable && (sample.capabilities&BATTERY_NATIVE_PERCENT)) {
    snprintf(b,sizeof b,"OS LEVEL: %lu%%",(unsigned long)sample.percent);
    print8(12,76,b,36,teal);
    print8(12,97,"COARSE OS LEVEL / NOT GAUGE",36,dim);
 } else {
    print8(12,76,"OS LEVEL: --",36,teal);
    print8(12,97,battery.complete?"NOT SUPPORTED ON THIS OS":"SCANNING OS BATTERY STATE",36,dim);
 }
 snprintf(b,sizeof b,"SCAN: %lu / 1048576",(unsigned long)battery.scanned_words);
 print8(12,124,b,37,txt);
 print8(12,145,"NO GUESSING / NO ADC WRITES",37,dim);
 print8(12,165,"READINGS MAY BE UNAVAILABLE",37,dim);
 rect(9,191,302,37,panel);
 print8(16,200,"ESC EXIT / AUTO ~12 SEC",37,teal);
 rect(15,219,(int)((step*290u)/800u),3,teal);
 eh_platform_flip();
}
int main(void){
 memset(&sample,0,sizeof sample);
 battery_native_init(&battery);
 if(!eh_platform_start())return 1;
 display(0);
 for(unsigned i=0;i<800u;i++){
    EhKey k=eh_platform_readkey();
    if(k==EH_KEY_ESC)break;
    if((i%10u)==0u) {
       readable=battery_native_default(&battery,&sample,battery.complete?0u:16384u);
       ++polls;
    }
    if((i%65u)==0u)display(i+1);
    eh_platform_sleep();
 }
 eh_platform_stop();
 return 0;
}
