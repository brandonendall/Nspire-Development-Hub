/*
 * EndraDiagnostics Run027: practical monitor; preserves proven Run014 platform.
 * Real values where validated; otherwise UNAVAILABLE, never fabricated.
 */
#include "../run002/platform.h"
#include "../run004/vendor/battery_native.h"
#include <libndls.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "font8x8_basic.h"
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
#define TAB_COUNT 6u
#define HISTORY 32u
#define DOC_MAX_ENTRIES 384u
#define DOC_MAX_DIRS 48u
#define DOC_MAX_DEPTH 4u
static uint16_t *fb;
static BatteryNative batt;
static BatteryNativeSnapshot snap;
static unsigned tab,updates,hn,hi,session_ticks;
static uint8_t history[HISTORY];
static uint32_t cpu_id,cache_id,control,ttbr,domain;
static int batt_valid,heap_result;
static unsigned heap_runs;
static int heap_probe_8k; /* 0 pending, 1 passed, 2 unavailable, 3 corrupt */
static unsigned heap_probe_bytes;
static unsigned cpu_test_pct,ram_test_pct;
static volatile uint32_t cpu_test_checksum;
static int cpu_test_done;
static uint32_t benchmark_ticks,benchmark_baseline,benchmark_samples;
static uint32_t last_benchmark_tick,last_heap_tick;
uint32_t eh_platform_ticks(void);
static unsigned long cpu_heartbeat;
static unsigned long cpu_id_checks;
static unsigned cpu_id_mismatches;
static unsigned doc_files,doc_dirs,doc_examined,doc_errors;
static unsigned doc_size_known,doc_size_zero,doc_size_unknown,doc_seek_checks;
static unsigned long long doc_bytes;
static int doc_state; /* 0 not scanned; 1 complete; 2 partial; 3 unavailable */
void __attribute__((weak)) _fini(void){}
void eh_platform_stop_mode(int);

static void fill(int x,int y,int w,int h,uint16_t c){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
 if(x+w>EH_W)w=EH_W-x;if(y+h>EH_H)h=EH_H-y;
 for(int j=y;j<y+h;j++)for(int i=x;i<x+w;i++)fb[j*EH_W+i]=c;
}
static void text8(int x,int y,const char *s,int limit,uint16_t color){
 for(int n=0;s&&s[n]&&n<limit;n++){
  unsigned char ch=(unsigned char)s[n];if(ch>127)ch='?';
  for(int r=0;r<8;r++){
   unsigned char b=font8x8_basic[ch][r];
   for(int k=0;k<8;k++)if(b&(1u<<k))fill(x+n*8+k,y+r,1,1,color);
  }
 }
}
static void sample_cpu(void){
 __asm__ volatile("mrc p15,0,%0,c0,c0,0":"=r"(cpu_id));
 __asm__ volatile("mrc p15,0,%0,c0,c0,1":"=r"(cache_id));
 __asm__ volatile("mrc p15,0,%0,c1,c0,0":"=r"(control));
 __asm__ volatile("mrc p15,0,%0,c2,c0,0":"=r"(ttbr));
 __asm__ volatile("mrc p15,0,%0,c3,c0,0":"=r"(domain));
}
static const char *charge_mode(void){
 if(!(batt_valid && (snap.capabilities&BATTERY_NATIVE_CHARGE_MODE)))return "UNAVAILABLE";
 switch(snap.charge_mode){
  case 1:return "DISCHARGING (OS)";
  case 2:case 3:case 4:case 5:return "CHARGING (OS)";
  case 6:return "CHECKING (OS)";
  default:return "UNKNOWN (OS)";
 }
}
static int level_known(void){
 return batt_valid && (snap.capabilities&BATTERY_NATIVE_PERCENT) && snap.percent<=100u;
}
static void remember_battery(void){
 if(!level_known())return;
 history[hi]=(uint8_t)snap.percent;
 hi=(hi+1u)%HISTORY;
 if(hn<HISTORY)hn++;
}
static void heap_check(void){
 const size_t n=4096u;
 uint8_t *p=(uint8_t*)malloc(n);
 heap_runs++;
 if(!p){heap_result=2;return;}
 for(size_t i=0;i<n;i++)p[i]=(uint8_t)((i*37u+11u)&255u);
 for(size_t i=0;i<n;i++)if(p[i]!=(uint8_t)((i*37u+11u)&255u)){heap_result=3;free(p);return;}
 for(size_t i=0;i<n;i++)p[i]=(uint8_t)((i^0xa5u)&255u);
 for(size_t i=0;i<n;i++)if(p[i]!=(uint8_t)((i^0xa5u)&255u)){heap_result=3;free(p);return;}
 free(p);heap_result=1;
}
/* Explicitly requested read-only DOCUMENT TREE accounting.
 * Bounded scan: 384 entries, at most 48 folders, depth <= 4.
 * Counts only files successfully stat()ed. No filesystem total/free claim.
 */
typedef struct {char path[256];unsigned depth;} Folder;
static void docs_scan(void){
 Folder queue[DOC_MAX_DIRS];
 unsigned front=0,back=0;
 doc_state=3;doc_files=doc_dirs=doc_examined=doc_errors=0;doc_bytes=0;
 doc_size_known=doc_size_zero=doc_size_unknown=doc_seek_checks=0;
 const char *root=get_documents_dir();
 if(!root||!*root||strlen(root)>=sizeof(queue[0].path))return;
 strcpy(queue[back].path,root);queue[back].depth=0;back++;
 int partial=0;
 while(front<back && doc_examined<DOC_MAX_ENTRIES){
  Folder *f=&queue[front++];
  DIR *d=opendir(f->path);
  if(!d){doc_errors++;partial=1;continue;}
  struct dirent *ent;
  while((ent=readdir(d))!=NULL){
   if(!strcmp(ent->d_name,".")||!strcmp(ent->d_name,".."))continue;
   if(doc_examined>=DOC_MAX_ENTRIES){partial=1;break;}
   doc_examined++;
   char path[320];
   if(snprintf(path,sizeof(path),"%s/%s",f->path,ent->d_name)>=(int)sizeof(path)){
    doc_errors++;partial=1;continue;
   }
   struct stat st;
   if(stat(path,&st)!=0){doc_errors++;partial=1;continue;}
   if(S_ISDIR(st.st_mode)){
    doc_dirs++;
    if(f->depth+1u<DOC_MAX_DEPTH && back<DOC_MAX_DIRS &&
       strlen(path)<sizeof(queue[0].path)){
     strcpy(queue[back].path,path);
     queue[back++].depth=f->depth+1u;
    }else partial=1;
   }else if(S_ISREG(st.st_mode)){
    doc_files++;
    /* Ndless virtual Documents stat() can return zero for nonempty files.
     * For at most 48 entries, optionally cross-check with file seek-to-end.
     * fopen(...,"rb") never writes; SEEK_END / ftell does not read contents.
     * Treat zero from BOTH as unknown, not a reliable zero-size file.
     */
    long sz=st.st_size>0?(long)st.st_size:-1L;
    if(sz<=0 && doc_seek_checks<48u){
      doc_seek_checks++;
      FILE *fp=fopen(path,"rb");
      if(fp){
        if(fseek(fp,0,SEEK_END)==0){
          long n=ftell(fp);
          if(n>0)sz=n;
          else if(n==0)doc_size_zero++;
        }
        fclose(fp);
      }
    }
    if(sz>0){doc_bytes+=(unsigned long long)sz;doc_size_known++;}
    else doc_size_unknown++;
   }
  }
  closedir(d);
 }
 if(front<back)partial=1;
 doc_state=partial?2:1;
}

/* A deliberately small, bounded allocation probe. This is NOT total/free RAM.
 * Only touches memory returned by malloc; immediately releases it.
 * One 8 KiB allocation, with sampled first/last bytes and no OS probing.
 */
static void heap_probe(void){
 const size_t n=8192u;
 uint8_t *p=(uint8_t*)malloc(n);
 if(!p){heap_probe_8k=2;return;}
 for(size_t i=0;i<n;i++)p[i]=(uint8_t)((i*17u+23u)&255u);
 heap_probe_8k=1;
 for(size_t i=0;i<n;i++)if(p[i]!=(uint8_t)((i*17u+23u)&255u)){
  heap_probe_8k=3;break;
 }
 if(heap_probe_8k==1)heap_probe_bytes=(unsigned)n;
 free(p);
}
/* Periodic bounded CPU throughput benchmark, not total CPU utilization.
 * Measured using the same timer already owned by the tested display platform.
 * Baseline is the FIRST observed runtime, and 100 means as fast as baseline.
 * Keep the sample short so keyboard and display remain responsive.
 */
static void cpu_benchmark(void){
 uint32_t t0=eh_platform_ticks();
 uint32_t x=cpu_test_checksum?cpu_test_checksum:0x13579BDFu;
 for(unsigned i=0;i<8192u;i++)x=(x<<5)^(x>>3)^(i*33u+17u);
 cpu_test_checksum=x;
 uint32_t dt=eh_platform_ticks()-t0;
 if(dt==0u){cpu_test_pct=0;benchmark_ticks=0;return;}
 benchmark_ticks=dt;
 if(benchmark_baseline==0u)benchmark_baseline=dt;
 benchmark_samples++;
 /* Relative benchmark throughput; cap at 100. Not CPU load. */
 uint32_t score=(uint32_t)(((uint64_t)benchmark_baseline*100u)/dt);
 cpu_test_pct=score>100u?100u:(unsigned)score;
}
/* 270-degree instrument dial: fixed-point 8.8 angles precomputed offline.
 * No trigonometry, floating point, delays or extra register access at runtime.
 * Negative pct means telemetry genuinely unavailable: neutral ring, "N/A".
 */
static const int16_t dial_x[33]={-181,-206,-226,-241,-251,-256,-255,-248,-237,-220,-198,-172,-142,-109,-74,-38,0,38,74,109,142,172,198,220,237,248,255,256,251,241,226,206,181};
static const int16_t dial_y[33]={181,152,121,86,50,13,-25,-62,-98,-132,-162,-190,-213,-231,-245,-253,-256,-253,-245,-231,-213,-190,-162,-132,-98,-62,-25,13,50,86,121,152,181};
static void line(int x0,int y0,int x1,int y1,uint16_t c){
 int dx=abs(x1-x0),sx=x0<x1?1:-1,dy=-abs(y1-y0),sy=y0<y1?1:-1,err=dx+dy;
 for(;;){
  fill(x0-1,y0-1,3,3,c);
  if(x0==x1&&y0==y1)break;
  int e2=err*2;if(e2>=dy){err+=dy;x0+=sx;}if(e2<=dx){err+=dx;y0+=sy;}
 }
}
static void border(int x,int y,int w,int h,uint16_t c){
 fill(x,y,w,1,c);fill(x,y+h-1,w,1,c);
 fill(x,y,1,h,c);fill(x+w-1,y,1,h,c);
}
static void center_text(int cx,int y,const char *s,int cap,uint16_t color){
 int n=0;while(s[n]&&n<cap)n++;
 text8(cx-4*n,y,s,cap,color);
}

/* Endra Graphics Engine prototype: RGB565 gradient fills and bevels.
 * Draws entirely to the proven application framebuffer; no new MMIO.
 */
static void ege_gradient(int x,int y,int w,int h,uint16_t top,uint16_t bottom){
 if(w<=0||h<=0)return;
 unsigned tr=(top>>11)&31u,tg=(top>>5)&63u,tb=top&31u;
 unsigned br=(bottom>>11)&31u,bg=(bottom>>5)&63u,bb=bottom&31u;
 for(int row=0;row<h;row++){
  unsigned d=(unsigned)(h>1?h-1:1),a=(unsigned)row;
  unsigned rr=(tr*(d-a)+br*a)/d;
  unsigned gg=(tg*(d-a)+bg*a)/d;
  unsigned bbv=(tb*(d-a)+bb*a)/d;
  fill(x,y+row,w,1,(uint16_t)((rr<<11)|(gg<<5)|bbv));
 }
}
static void ege_bevel(int x,int y,int w,int h){
 border(x,y,w,h,RGB(13,91,121));
 if(w>4&&h>4){
  fill(x+2,y+1,w-4,1,RGB(52,218,246));
  fill(x+2,y+h-2,w-4,1,RGB(4,47,77));
 }
}
static void panel(int x,int y,int w,int h){
 ege_gradient(x,y,w,h,RGB(12,39,60),RGB(5,15,29));
 ege_bevel(x,y,w,h);
}
/* Color segments show a SCALE, not measured utilization.
 * Needle and numeric percent appear ONLY for a valid measurement.
 * For CPU/RAM utilization (currently unverified), clearly show N/A.
 */
static void tach(int cx,int cy,int radius,int pct){
 const uint16_t green=RGB(59,232,112),yellow=RGB(255,209,69);
 const uint16_t orange=RGB(255,129,46),red=RGB(240,66,76);
 const uint16_t cyan=RGB(24,183,244),inactive=RGB(83,111,132);
 for(unsigned i=0;i<32u;i++){
  uint16_t c=i<17u?green:i<24u?yellow:i<29u?orange:red;
  /* All graduated segments are a printed scale, NOT filled usage. */
  int x0=cx+(dial_x[i]*radius)/256;
  int y0=cy+(dial_y[i]*radius)/256;
  int x1=cx+(dial_x[i+1]*radius)/256;
  int y1=cy+(dial_y[i+1]*radius)/256;
  line(x0,y0,x1,y1,c);
  int ax=cx+(dial_x[i]*(radius+5))/256;
  int ay=cy+(dial_y[i]*(radius+5))/256;
  int bx=cx+(dial_x[i+1]*(radius+5))/256;
  int by=cy+(dial_y[i+1]*(radius+5))/256;
  if(i%2u==0u)line(ax,ay,bx,by,cyan);
 }
 if(pct>=0 && pct<=100){
  int ix=(pct*32+50)/100;
  if(ix>32)ix=32;
  /* Dark needle shadow and bright high-contrast center pivot. */
  line(cx+1,cy+1,cx+1+(dial_x[ix]*(radius-7))/256,
    cy+1+(dial_y[ix]*(radius-7))/256,RGB(2,8,15));
  line(cx,cy,cx+(dial_x[ix]*(radius-7))/256,
    cy+(dial_y[ix]*(radius-7))/256,RGB(255,77,65));
  line(cx-1,cy,cx-1+(dial_x[ix]*(radius-9))/256,
    cy+(dial_y[ix]*(radius-9))/256,RGB(255,216,207));
  fill(cx-4,cy-4,9,9,RGB(26,52,68));
  fill(cx-3,cy-3,7,7,RGB(159,189,199));
  fill(cx-1,cy-1,3,3,RGB(238,250,255));
 }else{
  /* Neutral hub makes it obvious that no needle is being shown. */
  fill(cx-1,cy-1,3,3,inactive);
 }
}
static void battery_icon(int x,int y,int pct){
 const uint16_t bright=RGB(51,243,133),dark=RGB(34,69,83);
 border(x,y,19,30,RGB(227,243,248));
 fill(x+6,y-3,7,3,RGB(227,243,248));
 for(int n=0;n<4;n++){
  int from=25*n,to=25*(n+1);
  fill(x+3,y+3+6*(3-n),13,4,pct>=to?bright:dark);
 }
}
/* Run027: rebuilt cockpit geometry, large instruments and status rail. */
static void draw(void){
 const uint16_t white=RGB(234,247,255),cyan=RGB(42,218,248);
 const uint16_t green=RGB(66,238,155),dim=RGB(131,171,189);
 char s[52];fb=eh_framebuffer();
 ege_gradient(0,0,320,240,RGB(7,26,45),RGB(1,7,16));
 ege_gradient(0,0,320,27,RGB(19,63,91),RGB(5,23,41));
 fill(0,26,320,2,cyan);
 text8(8,8,"ENDRA",5,white);
 text8(58,8,"DIAGNOSTICS",11,cyan);
 text8(276,8,"027",3,green);
 ege_gradient(7,34,150,113,RGB(16,50,73),RGB(3,15,30));
 ege_gradient(163,34,150,113,RGB(16,50,73),RGB(3,15,30));
 ege_bevel(7,34,150,113);ege_bevel(163,34,150,113);
 center_text(82,39,"CPU SPEED",9,cyan);
 center_text(238,39,"MEMORY TEST",11,cyan);
 tach(82,93,43,(int)cpu_test_pct);
 tach(238,93,43,(int)ram_test_pct);
 fill(51,121,62,18,RGB(2,11,22));
 fill(207,121,62,18,RGB(2,11,22));
 border(51,121,62,18,RGB(26,116,149));
 border(207,121,62,18,RGB(26,116,149));
 snprintf(s,sizeof(s),"%u/100",cpu_test_pct);
 center_text(82,126,s,7,white);
 snprintf(s,sizeof(s),"%u/100",ram_test_pct);
 center_text(238,126,s,7,white);
 ege_gradient(7,152,306,40,RGB(13,41,62),RGB(4,17,31));
 ege_bevel(7,152,306,40);
 const char *labels[6]={"SYSTEM STATUS","CPU BENCHMARK","MEMORY INTEGRITY","DOCUMENT SCAN","BATTERY STATUS","DEVICE INFO"};
 text8(14,156,labels[tab],18,cyan);
 if(tab==0u){
  snprintf(s,sizeof(s),"ARM926 REV:%u CACHE:%s",(unsigned)(cpu_id&15u),(control&(1u<<12))?"ON":"OFF");
 }else if(tab==1u){
  snprintf(s,sizeof(s),"SCORE:%u TICKS:%lu LOAD:N/A",cpu_test_pct,(unsigned long)benchmark_ticks);
 }else if(tab==2u){
  snprintf(s,sizeof(s),"4K:%s 8K:%s USED:N/A",heap_result==1?"PASS":"WAIT",heap_probe_8k==1?"PASS":"WAIT");
 }else if(tab==3u){
  snprintf(s,sizeof(s),"FILES:%u DIRS:%u %s",doc_files,doc_dirs,doc_state==1?"DONE":doc_state==2?"PART":"WAIT");
 }else if(tab==4u){
  if(level_known())snprintf(s,sizeof(s),"BATTERY:%lu%% SAMPLES:%u",(unsigned long)snap.percent,hn);
  else snprintf(s,sizeof(s),"BATTERY:N/A SAMPLES:%u",hn);
 }else{
  snprintf(s,sizeof(s),"CPU ID:%08lX ERR:%02lX",(unsigned long)cpu_id,(unsigned long)(snap.errors&255u));
 }
 text8(14,172,s,35,white);
 const char *tabs[6]={"HOME","CPU","RAM","DOCS","BATT","INFO"};
 for(unsigned i=0;i<6u;i++){
  int x=(int)i*53;
  if(tab==i){
   ege_gradient(x,199,52,24,RGB(18,113,144),RGB(6,48,77));
   fill(x,199,52,3,green);
  }else ege_gradient(x,199,52,24,RGB(11,36,57),RGB(3,16,30));
  fill(x+51,203,1,15,RGB(20,73,94));
  center_text(x+26,208,tabs[i],6,tab==i?white:dim);
 }
 fill(0,227,320,13,RGB(2,12,24));
 fill(0,226,320,1,RGB(20,112,147));
 if(level_known())snprintf(s,sizeof(s),"BAT:%lu%%",(unsigned long)snap.percent);
 else strcpy(s,"BAT:N/A");
 text8(5,230,s,11,green);
 text8(93,230,"CPU:BENCH RAM:TEST",18,dim);
 text8(285,230,"ESC",3,white);
 eh_platform_flip();
}

int main(void){
 memset(&snap,0,sizeof(snap));battery_native_init(&batt);
 sample_cpu();
 if(!eh_platform_start())return 1;
 /* Automatic, ONE-TIME small heap check and bounded read-only Documents scan.
  * Explicit user request: no more manual UP to get baseline diagnostics.
  * These are previously physically tested and never write system/storage.
  * Scan occurs once at startup, not continuously.
  */
 /* Paint immediately before potentially slow Documents enumeration. */
 draw();
 heap_check();
 if(heap_result==1)ram_test_pct=50u;
 draw();
 heap_probe();
 if(heap_result==1 && heap_probe_8k==1)ram_test_pct=100u;
 draw();
 docs_scan();
 draw();
 last_benchmark_tick=eh_platform_ticks();last_heap_tick=last_benchmark_tick;
 cpu_benchmark();draw();
 /* One dashboard, SIX interactive views of the SAME screen. */
 for(uint32_t t=0;;t++){
  /* Live read-only CPU identity consistency check, not a load metric. */
  if(t%67u==0u){
   uint32_t id_now;
   __asm__ volatile("mrc p15,0,%0,c0,c0,0":"=r"(id_now));
   cpu_id_checks++;
   if(id_now!=cpu_id)cpu_id_mismatches++;
  }
  cpu_heartbeat++;
  int changed=0;
  uint32_t now=eh_platform_ticks();
  if((uint32_t)(now-last_benchmark_tick)>=32768u){
   last_benchmark_tick=now;
   cpu_benchmark();
   changed=1;
  }
  if((uint32_t)(now-last_heap_tick)>=32768u*5u){
   last_heap_tick=now;
   heap_check();heap_probe();
   ram_test_pct=(heap_result==1?50u:0u)+(heap_result==1&&heap_probe_8k==1?50u:0u);
   changed=1;
  }
  EhKey key=eh_platform_readkey();
  if(key==EH_KEY_ESC||key==EH_KEY_ENTER)break;
  if(key==EH_KEY_RIGHT||key==EH_KEY_MENU){
   tab=(tab+1u)%TAB_COUNT;changed=1;
  }
  if(key==EH_KEY_LEFT){
   tab=(tab+TAB_COUNT-1u)%TAB_COUNT;changed=1;
  }
  if((!batt.complete && t%5u==0u)||(batt.complete && t%67u==0u)){
   batt_valid=battery_native_default(&batt,&snap,batt.complete?0u:16384u);
   if(batt.complete){updates++;remember_battery();}
   changed=1;
  }
  if(changed)draw();
  eh_platform_sleep();
 }
 eh_platform_stop_mode(0);
 return 0;
}
