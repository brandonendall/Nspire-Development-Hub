/*
 * EndraDiagnostics Run009 hardware dashboard.
 * Based on physically tested Run008 battery + unified fast exit.
 * Adds only ARM926 CP15 identity/cache/MMU register READS and page navigation.
 * No RAM stress testing, register writes, charger access or OS calls.
 */
#include "../run002/platform.h"
#include "../run004/vendor/battery_native.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "font8x8_basic.h"
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
#define MAX_STEPS 2000u
#define TOTAL_WORDS 1048576ul
static uint16_t *fb;
static BatteryNative batt;
static BatteryNativeSnapshot sample;
static int valid;
static unsigned updates,page;
static uint32_t midr,ctype,ctrl,ttbr,dacr;
void __attribute__((weak)) _fini(void) {}
void eh_platform_stop_mode(int refresh_ti_screen);

static void fill(int x,int y,int w,int h,uint16_t c) {
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
 if(x+w>EH_W)w=EH_W-x;
 if(y+h>EH_H)h=EH_H-y;
 for(int r=y;r<y+h;r++)for(int k=x;k<x+w;k++)fb[r*EH_W+k]=c;
}
static void write8(int x,int y,const char *t,int cap,uint16_t c) {
 for(int n=0;t&&t[n]&&n<cap;n++){
  unsigned char ch=(unsigned char)t[n];if(ch>127)ch='?';
  for(int row=0;row<8;row++){
   unsigned char bits=font8x8_basic[ch][row];
   for(int col=0;col<8;col++)if(bits&(1u<<col))
    fill(x+n*8+col,y+row,1,1,c);
  }
 }
}
static void read_cpu_identity(void) {
 __asm__ volatile("mrc p15, 0, %0, c0, c0, 0":"=r"(midr));
 __asm__ volatile("mrc p15, 0, %0, c0, c0, 1":"=r"(ctype));
 __asm__ volatile("mrc p15, 0, %0, c1, c0, 0":"=r"(ctrl));
 __asm__ volatile("mrc p15, 0, %0, c2, c0, 0":"=r"(ttbr));
 __asm__ volatile("mrc p15, 0, %0, c3, c0, 0":"=r"(dacr));
}
static const char *charge_mode(void){
 if(!(valid&&(sample.capabilities&BATTERY_NATIVE_CHARGE_MODE)))
  return "NOT AVAILABLE";
 switch(sample.charge_mode){
  case 1:return "DISCHARGE (OS)";
  case 2:case 3:case 4:case 5:return "CHARGING (OS)";
  case 6:return "CHECKING (OS)";
  default:return "UNKNOWN MODE";
 }
}
static void dashboard(unsigned step) {
 const uint16_t bg=RGB(9,17,29),panel=RGB(25,41,58);
 const uint16_t white=RGB(236,244,249),teal=RGB(62,224,197),dim=RGB(160,180,195);
 char s[52];fb=eh_framebuffer();fill(0,0,320,240,bg);
 fill(0,0,320,32,panel);fill(0,0,4,32,teal);
 write8(10,11,"ENDRA DIAGNOSTICS",18,teal);
 write8(274,11,"009",3,white);
 if(page==0) {
  write8(12,40,"1/3 BATTERY MONITOR",36,white);
  if(valid && (sample.capabilities&BATTERY_NATIVE_PERCENT) && sample.percent<=100) {
   snprintf(s,sizeof(s),"OS BATTERY: %lu%%",(unsigned long)sample.percent);
   write8(12,62,s,36,teal);
   fill(12,84,295,19,panel);
   fill(15,87,(int)(289u*sample.percent/100u),13,teal);
  }else{
   write8(12,62,"OS BATTERY: --",36,teal);
   fill(12,84,295,19,panel);
  }
  write8(12,111,"OS CHARGE MODE:",36,white);
  write8(12,128,charge_mode(),36,teal);
  if(!batt.complete) {
   uint32_t n=batt.scanned_words;if(n>TOTAL_WORDS)n=TOTAL_WORDS;
   snprintf(s,sizeof(s),"OS SCAN: %lu%%",(unsigned long)n*100u/TOTAL_WORDS);
  }else if(!valid)snprintf(s,sizeof(s),"SIGNATURE NOT AVAILABLE");
  else snprintf(s,sizeof(s),"LIVE CHECKS: %u",updates);
  write8(12,151,s,36,dim);
  write8(12,169,"COARSE OS DATA / NO VOLTAGE",36,dim);
 } else if(page==1){
  unsigned impl=(midr>>24)&255u,arch=(midr>>16)&15u;
  unsigned part=(midr>>4)&4095u,rev=midr&15u;
  write8(12,40,"2/3 CPU INFORMATION",36,white);
  if(impl==0x41u && part==0x926u)write8(12,61,"ARM926EJ-S / ARMv5TEJ",36,teal);
  else write8(12,61,"CPU: SEE ID REGISTER",36,teal);
  snprintf(s,sizeof(s),"CPU ID: %08lX",(unsigned long)midr);
  write8(12,84,s,36,white);
  snprintf(s,sizeof(s),"IMPL:%02X ARCH:%X PART:%03X",impl,arch,part);
  write8(12,106,s,36,white);
  snprintf(s,sizeof(s),"REVISION: %u",rev);
  write8(12,128,s,36,white);
  snprintf(s,sizeof(s),"CACHE ID: %08lX",(unsigned long)ctype);
  write8(12,151,s,36,white);
  write8(12,171,"CP15 READ ONLY / NO BENCHMARK",36,dim);
 }else{
  write8(12,40,"3/3 MEMORY / MMU",36,white);
  snprintf(s,sizeof(s),"MMU: %s",(ctrl&1u)?"ENABLED":"DISABLED");
  write8(12,62,s,36,teal);
  snprintf(s,sizeof(s),"CONTROL: %08lX",(unsigned long)ctrl);
  write8(12,83,s,36,white);
  snprintf(s,sizeof(s),"TABLE BASE: %08lX",(unsigned long)(ttbr&0xffffc000u));
  write8(12,104,s,36,white);
  snprintf(s,sizeof(s),"DOMAIN: %08lX",(unsigned long)dacr);
  write8(12,125,s,36,white);
  write8(12,151,"TOTAL RAM: NOT MEASURED",36,dim);
  write8(12,169,"FREE RAM: NOT MEASURED",36,dim);
 }
 fill(9,193,302,35,panel);
 write8(14,198,"MENU/RIGHT: NEXT  LEFT: PREV",36,teal);
 write8(14,211,"ESC/ENTER: FAST EXIT (30S)",36,dim);
 fill(15,222,(int)(290u*step/MAX_STEPS),2,teal);
 eh_platform_flip();
}
int main(void){
 memset(&sample,0,sizeof(sample));
 battery_native_init(&batt);
 read_cpu_identity();
 if(!eh_platform_start())return 1;
 dashboard(0);
 for(unsigned i=0;i<MAX_STEPS;i++){
  EhKey key=eh_platform_readkey();
  if(key==EH_KEY_ESC||key==EH_KEY_ENTER)break;
  if(key==EH_KEY_MENU||key==EH_KEY_RIGHT){page=(page+1u)%3u;dashboard(i+1);}
  if(key==EH_KEY_LEFT){page=(page+2u)%3u;dashboard(i+1);}
  if((!batt.complete && i%5u==0u)||(batt.complete && i%67u==0u)){
   valid=battery_native_default(&batt,&sample,batt.complete?0u:16384u);
   if(batt.complete)updates++;
  }
  if(i%65u==0u || (batt.complete && i%67u==0u))dashboard(i+1);
  eh_platform_sleep();
 }
 eh_platform_stop_mode(0);
 return 0;
}
