/*
 * EndraDiagnostics Run020: practical monitor; preserves proven Run014 platform.
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

static void panel(int x,int y,int w,int h){
 fill(x,y,w,h,RGB(9,23,38));
 border(x,y,w,h,RGB(18,198,242));
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
  line(cx,cy,cx+(dial_x[ix]*(radius-5))/256,
    cy+(dial_y[ix]*(radius-5))/256,RGB(244,252,255));
  fill(cx-2,cy-2,5,5,cyan);
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
static void draw(void){
 const uint16_t bg=RGB(3,11,21),panelbg=RGB(9,25,43);
 const uint16_t white=RGB(232,246,255),cyan=RGB(22,215,245);
 const uint16_t green=RGB(72,239,149),dim=RGB(152,190,203);
 char s[52];
 fb=eh_framebuffer();
 fill(0,0,320,240,bg);
 /* Single-screen neon HUD closely follows the approved concept image. */
 fill(0,0,320,29,panelbg);fill(0,28,320,1,cyan);
 text8(8,9,"ENDRA",8,white);
 fill(55,14,22,1,cyan);fill(62,11,2,7,cyan);
 text8(83,9,"SYSTEM MONITOR",17,white);
 text8(280,9,"020",3,cyan);
 /* Two large analog gauges and a compact real CPU-info center panel. */
 tach(54,76,31,-1);
 tach(266,76,31,-1);
 center_text(54,57,"CPU",4,white);
 center_text(266,57,"RAM",4,white);
 center_text(54,74,"N/A",4,dim);
 center_text(266,74,"N/A",4,dim);
 text8(23,106,"0",1,white);text8(78,106,"100",3,white);
 text8(237,106,"0",1,white);text8(291,106,"100",3,white);
 panel(105,34,110,85);
 /* Every tab changes the real CENTER display without opening pages. */
 const char *titles[6]={"SYSTEM INFO","CPU DETAILS","RAM CHECK","DOC DETAILS","BATTERY","MONITOR"};
 center_text(160,38,titles[tab],12,cyan);
 if(tab==0u){
  text8(112,52,"ARM926EJ-S",12,white);
  snprintf(s,sizeof(s),"REV:%u",(unsigned)(cpu_id&15u));
  text8(112,65,s,12,white);
  text8(112,78,(control&(1u<<12))?"I-CACHE:ON":"I-CACHE:OFF",12,white);
  text8(112,91,(control&(1u<<2))?"D-CACHE:ON":"D-CACHE:OFF",12,white);
  text8(112,104,(control&1u)?"MMU:ON":"MMU:OFF",12,cyan);
 }else if(tab==1u){
  snprintf(s,sizeof(s),"ID:%08lX",(unsigned long)cpu_id);
  text8(112,52,s,12,white);
  snprintf(s,sizeof(s),"REV:%u",(unsigned)(cpu_id&15u));
  text8(112,65,s,12,white);
  text8(112,78,"LOAD:N/A",12,dim);
  text8(112,91,"MHz: N/A",12,dim);
  text8(112,104,"CPU READONLY",12,cyan);
 }else if(tab==2u){
  text8(112,52,"RAM USED:N/A",12,dim);
  text8(112,65,"RAM FREE:N/A",12,dim);
  text8(112,78,heap_result==1?"HEAP:PASS":
    heap_result==2?"HEAP:FAIL":heap_result==3?"HEAP:ERROR":"HEAP:WAIT",12,white);
  snprintf(s,sizeof(s),"TESTS:%u",heap_runs);
  text8(112,91,s,12,white);
  text8(112,104,"TESTED:4KB",12,cyan);
 }else if(tab==3u){
  text8(112,52,doc_state==1?"SCAN:DONE":
    doc_state==2?"SCAN:PART":doc_state==3?"SCAN:N/A":"SCAN:WAIT",12,white);
  snprintf(s,sizeof(s),"F:%u D:%u",doc_files,doc_dirs);
  text8(112,65,s,12,white);
  snprintf(s,sizeof(s),"SIZED:%u",doc_size_known);
  text8(112,78,s,12,white);
  snprintf(s,sizeof(s),"UNKNOWN:%u",doc_size_unknown);
  text8(112,91,s,12,dim);
  text8(112,104,"STORAGE:N/A",12,cyan);
 }else if(tab==4u){
  if(level_known())snprintf(s,sizeof(s),"LEVEL:%lu%%",(unsigned long)snap.percent);
  else strcpy(s,"LEVEL:N/A");
  text8(112,52,s,12,white);
  snprintf(s,sizeof(s),"SAMPLES:%u",hn);
  text8(112,65,s,12,white);
  text8(112,78,"OS COARSE",12,dim);
  text8(112,91,"HEALTH:N/A",12,dim);
  text8(112,104,"VOLTAGE:N/A",12,cyan);
 }else{
  snprintf(s,sizeof(s),"OS:%luK",(unsigned long)(batt.scanned_words/1024ul));
  text8(112,52,s,12,white);
  snprintf(s,sizeof(s),"CAP:%02lX",(unsigned long)(snap.capabilities&255u));
  text8(112,65,s,12,white);
  snprintf(s,sizeof(s),"ERR:%02lX",(unsigned long)(snap.errors&255u));
  text8(112,78,s,12,white);
  text8(112,91,"NO AUTOEXIT",12,cyan);
  text8(112,104,"NO FS WRITES",12,cyan);
 }
 /* Actual battery %, known Documents bytes and file counts. */
 panel(5,125,100,64);
 panel(110,125,118,64);
 panel(233,125,82,64);
 text8(12,130,"BATTERY",11,cyan);
 if(level_known())snprintf(s,sizeof(s),"%lu%%",(unsigned long)snap.percent);
 else strcpy(s,"N/A");
 text8(42,145,s,7,white);
 battery_icon(15,151,level_known()?(int)snap.percent:-1);
 if(batt_valid && (snap.capabilities&BATTERY_NATIVE_CHARGE_MODE)){
  if(snap.charge_mode==1)text8(42,160,"DISCHG",7,dim);
  else if(snap.charge_mode>=2 && snap.charge_mode<=5)text8(42,160,"CHARGE",7,green);
  else text8(42,160,"OS MODE",7,dim);
 }else text8(42,160,"OS N/A",7,dim);
 text8(112,130,"DOCUMENTS",13,cyan);
 if(doc_state==0)text8(114,146,"UP TO SCAN",13,white);
 else if(doc_size_known){
  snprintf(s,sizeof(s),"%lu KB",(unsigned long)(doc_bytes/1024ull));
  text8(114,146,s,13,white);
 }else text8(114,146,"SIZE: N/A",13,dim);
 text8(114,160,"FREE: N/A",13,dim);
 fill(115,178,109,4,RGB(31,68,82));
 /* No proportional storage bar: total/free capacity not verified. */
 text8(236,130,"FILES",8,cyan);
 if(doc_state!=0){
  snprintf(s,sizeof(s),"F:%u",doc_files);text8(237,144,s,9,white);
  snprintf(s,sizeof(s),"D:%u",doc_dirs);text8(237,157,s,9,white);
  snprintf(s,sizeof(s),"%s",doc_state==1?"DONE":doc_state==2?"PART":"N/A");
  text8(237,172,s,8,dim);
 }else{
  text8(237,145,"N/A",8,white);
  text8(237,170,"SCAN",8,dim);
 }
 /* Context panel changes on tab movement. The dashboard remains unified. */
 if(tab==1u){
  panel(110,125,118,64);
  text8(114,130,"CPU CACHE",13,cyan);
  snprintf(s,sizeof(s),"%08lX",(unsigned long)cache_id);
  text8(114,146,s,13,white);
  text8(114,161,(control&(1u<<2))?"D-CACHE: ON":"D-CACHE: OFF",13,dim);
 }else if(tab==2u){
  panel(110,125,118,64);
  text8(114,130,"MEMORY TEST",13,cyan);
  text8(114,146,heap_result==1?"4KB: PASS":
    heap_result==2?"4KB: FAIL":heap_result==3?"4KB: ERROR":"NOT TESTED",13,white);
  text8(114,162,"USED RAM: N/A",13,dim);
 }else if(tab==3u){
  panel(233,125,82,64);
  text8(236,130,"DOC CHECK",10,cyan);
  snprintf(s,sizeof(s),"F:%u",doc_files);text8(237,146,s,10,white);
  snprintf(s,sizeof(s),"U:%u",doc_size_unknown);text8(237,161,s,10,dim);
 }else if(tab==4u){
  panel(110,125,118,64);
  text8(114,130,"BATTERY INFO",13,cyan);
  snprintf(s,sizeof(s),"OS: %s",level_known()?"VALID":"WAIT");
  text8(114,146,s,13,white);
  snprintf(s,sizeof(s),"SAMPLES:%u",hn);text8(114,161,s,13,dim);
 }else if(tab==5u){
  panel(110,125,118,64);
  text8(114,130,"OS RESOLVER",13,cyan);
  snprintf(s,sizeof(s),"ERR:%02lX",(unsigned long)(snap.errors&255u));
  text8(114,146,s,13,white);
  snprintf(s,sizeof(s),"CAP:%02lX",(unsigned long)(snap.capabilities&255u));
  text8(114,161,s,13,dim);
 }
 /* Navigation-style tabs: selection updates context, no page transitions. */
 const char *tabs[6]={"HOME","CPU","RAM","DOCS","BATT","INFO"};
 for(unsigned i=0;i<6u;i++){
  int x=(int)i*53;
  fill(x,193,52,25,tab==i?RGB(16,92,121):RGB(8,29,50));
  border(x,193,52,25,tab==i?green:RGB(16,112,144));
  center_text(x+26,202,tabs[i],6,tab==i?white:cyan);
 }
 fill(0,221,320,19,panelbg);fill(0,220,320,1,cyan);
 if(tab==2)text8(5,225,"AUTO: HEAP OK",16,green);
 else if(tab==3)text8(5,225,"AUTO: DOCS OK",16,green);
 else text8(5,225,"LEFT/RIGHT: TAB",17,cyan);
 text8(170,225,"ESC/ENTER: EXIT",19,white);
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
 draw();
 docs_scan();
 draw();
 /* One dashboard, SIX interactive views of the SAME screen. */
 for(uint32_t t=0;;t++){
  EhKey key=eh_platform_readkey();
  if(key==EH_KEY_ESC||key==EH_KEY_ENTER)break;
  int changed=0;
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
