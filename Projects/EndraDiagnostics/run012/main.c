/*
 * EndraDiagnostics Run012: bundled low-risk diagnostic upgrades.
 * Run010 read-only OS battery scanner + Run008 ultra-fast manual exit.
 * Added cache register detail, ASIC ID, runtime/battery history,
 * bounded optional 4KB user-heap check, and battery resolver status.
 * NO automatic exit, no flash writes, no unsafe RAM probing.
 */
#include "../run002/platform.h"
#include "../run004/vendor/battery_native.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "font8x8_basic.h"
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
#define TOTAL_WORDS 1048576ul
#define PAGES 8u
#define HIST_LEN 32u
static uint16_t *fb;
static BatteryNative batt;
static BatteryNativeSnapshot sample;
static uint32_t midr,ctype,ctrl,ttbr,dacr,asic;
static unsigned page,updates,history_count,history_head;
static uint8_t history[HIST_LEN];
static int valid,heap_test=0;
static unsigned long seconds_in_app;
void __attribute__((weak)) _fini(void) {}
void eh_platform_stop_mode(int);

static void box(int x,int y,int w,int h,uint16_t color){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
 if(x+w>EH_W)w=EH_W-x;if(y+h>EH_H)h=EH_H-y;
 for(int r=y;r<y+h;r++)for(int c=x;c<x+w;c++)fb[r*EH_W+c]=color;
}
static void text8(int x,int y,const char *s,unsigned limit,uint16_t color){
 for(unsigned n=0;s&&s[n]&&n<limit;n++){
  unsigned char ch=(unsigned char)s[n];if(ch>127)ch='?';
  for(int r=0;r<8;r++){
   unsigned char b=font8x8_basic[ch][r];
   for(int c=0;c<8;c++)if(b&(1u<<c))box(x+(int)n*8+c,y+r,1,1,color);
  }
 }
}
static void cpu_read(void){
 __asm__ volatile("mrc p15,0,%0,c0,c0,0":"=r"(midr));
 __asm__ volatile("mrc p15,0,%0,c0,c0,1":"=r"(ctype));
 __asm__ volatile("mrc p15,0,%0,c1,c0,0":"=r"(ctrl));
 __asm__ volatile("mrc p15,0,%0,c2,c0,0":"=r"(ttbr));
 __asm__ volatile("mrc p15,0,%0,c3,c0,0":"=r"(dacr));
 /* This identification register is read by the existing Endra hardware tools. */
 asic=*(const volatile uint32_t *)(uintptr_t)0x900a0000u;
}
static const char *charge(void){
 if(!(valid&&(sample.capabilities&BATTERY_NATIVE_CHARGE_MODE)))return "UNAVAILABLE";
 switch(sample.charge_mode){
  case 1:return "DISCHARGING (OS)";
  case 2:case 3:case 4:case 5:return "CHARGING (OS)";
  case 6:return "CHECKING (OS)";
  default:return "UNKNOWN (OS)";
 }
}
static void record_sample(void){
 if(!(valid&&(sample.capabilities&BATTERY_NATIVE_PERCENT))||sample.percent>100u)return;
 history[history_head]=(uint8_t)sample.percent;
 history_head=(history_head+1u)%HIST_LEN;
 if(history_count<HIST_LEN)history_count++;
}
/* Only malloc-owned bytes are touched. At most one 4KB allocation.
 * No physical/OS memory writes, allocator introspection, or stress loops. */
static void run_heap_check(void){
 const size_t size=4096;
 uint8_t *p=(uint8_t *)malloc(size);
 if(!p){heap_test=2;return;}
 unsigned errors=0;
 for(size_t i=0;i<size;i++)p[i]=(uint8_t)((i*37u+11u)&255u);
 for(size_t i=0;i<size;i++)if(p[i]!=(uint8_t)((i*37u+11u)&255u)){errors++;break;}
 for(size_t i=0;i<size;i++)p[i]=(uint8_t)((i^0xa5u)&255u);
 for(size_t i=0;i<size;i++)if(p[i]!=(uint8_t)((i^0xa5u)&255u)){errors++;break;}
 free(p);
 heap_test=errors?3:1;
}
static void draw(void){
 const uint16_t bg=RGB(9,17,29),panel=RGB(25,41,58);
 const uint16_t white=RGB(238,244,248),teal=RGB(66,224,199),dim=RGB(158,181,194);
 char s[56];
 fb=eh_framebuffer();box(0,0,EH_W,EH_H,bg);
 box(0,0,EH_W,32,panel);box(0,0,4,32,teal);
 text8(10,11,"ENDRA DIAGNOSTICS",18,teal);
 text8(275,11,"012",3,white);
 if(page==0){
  text8(12,40,"1/8 LIVE BATTERY",36,white);
  if(valid&&(sample.capabilities&BATTERY_NATIVE_PERCENT)&&sample.percent<=100u){
   snprintf(s,sizeof(s),"OS BATTERY: %lu%%",(unsigned long)sample.percent);
   text8(12,61,s,36,teal);
   box(12,84,295,18,panel);box(15,87,289*(int)sample.percent/100,12,teal);
  }else text8(12,61,"OS BATTERY: --",36,teal);
  text8(12,111,"CHARGE MODE:",36,white);
  text8(12,128,charge(),36,teal);
  if(!batt.complete){
   snprintf(s,sizeof(s),"SCANNING: %lu%%",(unsigned long)batt.scanned_words*100ul/TOTAL_WORDS);
  }else if(!valid)snprintf(s,sizeof(s),"OS SIGNATURE UNAVAILABLE");
  else snprintf(s,sizeof(s),"LIVE UPDATES: %u",updates);
  text8(12,152,s,36,dim);
  text8(12,171,"COARSE OS LEVEL / NO VOLTAGE",36,dim);
 }else if(page==1){
  unsigned impl=(midr>>24)&255u,arch=(midr>>16)&15u,part=(midr>>4)&4095u;
  text8(12,40,"2/8 CPU AND HARDWARE",36,white);
  text8(12,60,impl==0x41u&&part==0x926u?"ARM926EJ-S / ARMv5TEJ":"CPU: UNRECOGNIZED",36,teal);
  snprintf(s,sizeof(s),"CPU ID: %08lX",(unsigned long)midr);text8(12,82,s,36,white);
  snprintf(s,sizeof(s),"IMPL:%02X ARCH:%X PART:%03X",impl,arch,part);text8(12,102,s,36,white);
  snprintf(s,sizeof(s),"CPU REV: %u",(unsigned)(midr&15u));text8(12,122,s,36,white);
  snprintf(s,sizeof(s),"ASIC ID: %08lX",(unsigned long)asic);text8(12,142,s,36,white);
  text8(12,166,"READ ONLY / NO SPEED CLAIM",36,dim);
 }else if(page==2){
  text8(12,40,"3/8 CACHE INFORMATION",36,white);
  snprintf(s,sizeof(s),"CACHE ID: %08lX",(unsigned long)ctype);text8(12,64,s,36,teal);
  snprintf(s,sizeof(s),"I-FIELD: %03lX",(unsigned long)((ctype>>12)&4095u));
  text8(12,87,s,36,white);
  snprintf(s,sizeof(s),"D-FIELD: %03lX",(unsigned long)(ctype&4095u));
  text8(12,109,s,36,white);
  snprintf(s,sizeof(s),"CACHE ENABLE I:%u D:%u",(unsigned)((ctrl>>12)&1u),(unsigned)((ctrl>>2)&1u));
  text8(12,132,s,36,white);
  text8(12,156,"RAW REGISTER FIELDS",36,dim);
  text8(12,175,"NO CACHE MODIFICATION",36,dim);
 }else if(page==3){
  text8(12,40,"4/8 MMU / MEMORY",36,white);
  text8(12,61,(ctrl&1u)?"MMU: ENABLED":"MMU: DISABLED",36,teal);
  snprintf(s,sizeof(s),"CONTROL: %08lX",(unsigned long)ctrl);text8(12,84,s,36,white);
  snprintf(s,sizeof(s),"TABLE: %08lX",(unsigned long)(ttbr&0xffffc000u));text8(12,106,s,36,white);
  snprintf(s,sizeof(s),"DOMAIN: %08lX",(unsigned long)dacr);text8(12,128,s,36,white);
  text8(12,150,"TOTAL RAM: NOT MEASURED",36,dim);
  text8(12,170,"FREE RAM: NOT MEASURED",36,dim);
 }else if(page==4){
  text8(12,40,"5/8 SAFE HEAP TEST",36,white);
  text8(12,63,"PRESS UP TO TEST 4KB",36,teal);
  if(heap_test==0)text8(12,88,"RESULT: NOT RUN",36,white);
  else if(heap_test==1)text8(12,88,"RESULT: PASS",36,teal);
  else if(heap_test==2)text8(12,88,"RESULT: ALLOC FAILED",36,white);
  else text8(12,88,"RESULT: DATA MISMATCH",36,white);
  text8(12,114,"2 PATTERNS / OWNED MEMORY",36,white);
  text8(12,137,"ALWAYS FREED AFTER TEST",36,dim);
  text8(12,160,"NO SYSTEM RAM SCAN",36,dim);
  text8(12,178,"NO PHYSICAL RAM WRITES",36,dim);
 }else if(page==5){
  text8(12,40,"6/8 MONITOR DIAGNOSTICS",36,white);
  snprintf(s,sizeof(s),"OS SCAN: %lu/1024K",(unsigned long)batt.scanned_words/1024ul);
  text8(12,61,s,36,white);
  snprintf(s,sizeof(s),"P:%u C:%u A:%u V:%u",batt.percent_matches,batt.charge_matches,
     batt.adc_matches,batt.converter_matches);
  text8(12,83,s,36,white);
  snprintf(s,sizeof(s),"CAP:%02lX ERR:%02lX",(unsigned long)(sample.capabilities&255u),
     (unsigned long)(sample.errors&255u));text8(12,105,s,36,white);
  snprintf(s,sizeof(s),"SESSION: %lu SEC",seconds_in_app);text8(12,127,s,36,white);
  snprintf(s,sizeof(s),"BATTERY SAMPLES: %u",history_count);text8(12,149,s,36,white);
  if(history_count){
   uint8_t first=history[(history_head+HIST_LEN-history_count)%HIST_LEN];
   uint8_t last=history[(history_head+HIST_LEN-1u)%HIST_LEN];
   snprintf(s,sizeof(s),"FIRST:%u%% LATEST:%u%%",(unsigned)first,(unsigned)last);
   text8(12,170,s,36,teal);
  }
 }else if(page==6){
  text8(12,40,"7/8 BATTERY TREND",36,white);
  text8(12,58,"LAST 32 OS SAMPLES",36,dim);
  box(12,80,296,79,panel);
  if(history_count){
   unsigned min=100u,max=0u;
   for(unsigned j=0;j<history_count;j++){
    unsigned v=history[(history_head+HIST_LEN-history_count+j)%HIST_LEN];
    if(v<min)min=v;if(v>max)max=v;
    int x=17+(int)(j*286u/(HIST_LEN-1u));
    int h=(int)(v*67u/100u);
    box(x,153-h,5,h,teal);
   }
   snprintf(s,sizeof(s),"MIN:%u%% MAX:%u%%",min,max);
   text8(12,166,s,36,white);
  }else text8(25,113,"AWAITING OS BATTERY DATA",34,dim);
 }else{
  text8(12,40,"8/8 SAFETY / STATUS",36,white);
  text8(12,60,"NO AUTO EXIT; FAST MANUAL",36,teal);
  text8(12,81,"NO FLASH / ADC WRITES",36,white);
  text8(12,102,"NO CLOCK/HOOK MODIFICATION",36,white);
  text8(12,123,"4KB HEAP TEST IS OPTIONAL",36,white);
  snprintf(s,sizeof(s),"OS RESOLVER: %s",batt.complete?"FINISHED":"IN PROGRESS");
  text8(12,144,s,36,dim);
  snprintf(s,sizeof(s),"HEAP TEST: %s",heap_test==0?"NOT RUN":heap_test==1?"PASSED":heap_test==2?"ALLOC FAILED":"MISMATCH");
  text8(12,165,s,36,dim);
 }
 box(9,196,302,32,panel);
 text8(14,199,"MENU/RIGHT NEXT | LEFT PREV",36,teal);
 text8(14,212,"ESC/ENTER FAST EXIT",36,dim);
 eh_platform_flip();
}
int main(void){
 memset(&sample,0,sizeof(sample));
 battery_native_init(&batt);
 cpu_read();
 if(!eh_platform_start())return 1;
 draw();
 /* Forever until manual exit. No auto-exit branch or timeout. */
 for(uint32_t loop=0;;loop++){
  EhKey key=eh_platform_readkey();
  if(key==EH_KEY_ESC||key==EH_KEY_ENTER)break;
  int redraw=0;
  if(key==EH_KEY_MENU||key==EH_KEY_RIGHT){page=(page+1u)%PAGES;redraw=1;}
  if(key==EH_KEY_LEFT){page=(page+PAGES-1u)%PAGES;redraw=1;}
  if(key==EH_KEY_UP&&page==4u){run_heap_check();redraw=1;}
  if((!batt.complete && loop%5u==0u)||(batt.complete && loop%67u==0u)){
   valid=battery_native_default(&batt,&sample,batt.complete?0u:16384u);
   if(batt.complete){updates++;record_sample();}
   redraw=1;
  }
  /* Wall-time estimate for monitoring only, based on existing 15ms sleep.
   * It is NOT a calibrated real-time clock. */
  if(loop%67u==0u)seconds_in_app++;
  if(redraw && (page==0u||page==5u||key!=EH_KEY_NONE))draw();
  eh_platform_sleep();
 }
 eh_platform_stop_mode(0);
 return 0;
}
