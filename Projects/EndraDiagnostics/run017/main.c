/*
 * EndraDiagnostics Run017: practical monitor; preserves proven Run014 platform.
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
#define PAGE_COUNT 6u
#define HISTORY 32u
#define DOC_MAX_ENTRIES 384u
#define DOC_MAX_DIRS 48u
#define DOC_MAX_DEPTH 4u
static uint16_t *fb;
static BatteryNative batt;
static BatteryNativeSnapshot snap;
static unsigned page,updates,hn,hi,session_ticks;
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
static void dial(int cx,int cy,int pct,const char *label,const char *sub){
 const uint16_t dim=RGB(37,66,83),silver=RGB(145,179,190);
 const uint16_t cyan=RGB(32,211,239),green=RGB(64,232,163);
 const uint16_t yellow=RGB(255,202,82),red=RGB(247,93,96);
 char value[17];center_text(cx,cy-39,label,12,cyan);
 const int segments=32;
 int count=pct<0?0:(pct>=100?segments:pct*segments/100);
 for(int i=0;i<segments;i++){
  int x0=cx+((int)dial_x[i]*27)/256;
  int y0=cy+((int)dial_y[i]*27)/256;
  int x1=cx+((int)dial_x[i+1]*27)/256;
  int y1=cy+((int)dial_y[i+1]*27)/256;
  uint16_t col=dim;
  if(i<count)col=i<20?green:(i<27?yellow:red);
  line(x0,y0,x1,y1,col);
 }
 if(pct<0)snprintf(value,sizeof(value),"N/A");
 else snprintf(value,sizeof(value),"%d%%",pct);
 center_text(cx,cy-5,value,12,pct<0?silver:green);
 center_text(cx,cy+8,sub,17,silver);
}
static void panel(int x,int y,int w,int h){
 fill(x,y,w,h,RGB(12,28,43));
 border(x,y,w,h,RGB(25,132,158));
}

static void draw(void){
 const uint16_t bg=RGB(5,15,27),panelbg=RGB(14,29,45);
 const uint16_t white=RGB(233,246,252),teal=RGB(66,224,199),muted=RGB(145,177,190);
 char s[64];
 fb=eh_framebuffer();fill(0,0,320,240,bg);
 fill(0,0,320,30,panelbg);fill(0,0,4,30,teal);
 text8(10,11,"ENDRA SYSTEM MONITOR",22,teal);
 text8(279,11,"017",3,white);
 if(page==0){
  /* CPU usage and OS RAM usage are NOT available from validated OS APIs.
   * Never convert ARM identification, malloc success or cache activity
   * into a guessed percentage. Storage capacity is also unverified.
   */
  panel(7,34,150,80);panel(163,34,150,80);
  panel(7,118,150,80);panel(163,118,150,80);
  dial(82,80,-1,"CPU LOAD","ARM926EJ-S");
  dial(238,80,-1,"RAM USED","NO OS DATA");
  dial(82,164,level_known()?(int)snap.percent:-1,"BATTERY","OS LEVEL");
  dial(238,164,-1,"STORAGE","NO CAPACITY");
 }else if(page==1){
  text8(11,38,"2/6 CPU HARDWARE",36,white);
  panel(8,55,304,137);
  snprintf(s,sizeof(s),"CPU ID: %08lX",(unsigned long)cpu_id);
  text8(16,64,s,35,teal);
  snprintf(s,sizeof(s),"ARM926EJ-S  REV: %u",(unsigned)(cpu_id&15u));
  text8(16,85,s,35,white);
  snprintf(s,sizeof(s),"CACHE ID: %08lX",(unsigned long)cache_id);
  text8(16,107,s,35,white);
  snprintf(s,sizeof(s),"I-CACHE:%s D-CACHE:%s",
    (control&(1u<<12))?"ON":"OFF",(control&(1u<<2))?"ON":"OFF");
  text8(16,128,s,35,teal);
  text8(16,150,"CPU LOAD: UNAVAILABLE",35,muted);
  text8(16,170,"CLOCK MHz: UNVERIFIED",35,muted);
 }else if(page==2){
  text8(11,38,"3/6 RAM & MEMORY",36,white);
  panel(8,55,304,137);
  text8(16,63,"SYSTEM RAM USED: N/A",35,white);
  text8(16,84,"SYSTEM RAM FREE: N/A",35,white);
  text8(16,105,"UP: CHECK OWNED 4KB HEAP",35,teal);
  text8(16,126,heap_result==0?"HEAP TEST: NOT RUN":
    heap_result==1?"HEAP TEST: PASS":
    heap_result==2?"HEAP TEST: ALLOC FAILED":"HEAP TEST: MISMATCH",35,white);
  snprintf(s,sizeof(s),"MMU: %s  RUNS: %u",(control&1u)?"ON":"OFF",heap_runs);
  text8(16,147,s,35,white);
  text8(16,169,"NOT A FREE-RAM ESTIMATE",35,muted);
 }else if(page==3){
  text8(11,38,"4/6 DOCUMENT STORAGE",36,white);
  panel(8,55,304,137);
  text8(16,62,"UP: READ-ONLY SIZE SCAN",35,teal);
  text8(16,82,doc_state==0?"SCAN: NOT STARTED":
    doc_state==1?"SCAN: COMPLETE":
    doc_state==2?"SCAN: PARTIAL":"SCAN: UNAVAILABLE",35,white);
  snprintf(s,sizeof(s),"FILES:%u FOLDERS:%u",doc_files,doc_dirs);
  text8(16,102,s,35,white);
  snprintf(s,sizeof(s),"SEEN:%u ERR:%u",doc_examined,doc_errors);
  text8(16,122,s,35,white);
  /* The Ndless printf implementation on OS 6.4 may not support %%llu.
   * Its prior display showed "0" while %%lu on the overview showed
   * 84,468 KB. Convert to kilobytes before formatting as 32-bit %%lu.
   */
  if(doc_size_known){
   snprintf(s,sizeof(s),"KNOWN DOCUMENTS: %lu KB",
     (unsigned long)(doc_bytes/1024ull));
   text8(16,142,s,35,teal);
   snprintf(s,sizeof(s),"SIZED:%u UNKNOWN:%u",doc_size_known,doc_size_unknown);
   text8(16,163,s,35,white);
  }else text8(16,143,"FILE SIZES UNAVAILABLE",35,muted);
  text8(16,181,"",35,muted);
 }else if(page==4){
  text8(11,38,"5/6 BATTERY MONITOR",36,white);
  panel(8,55,304,137);
  if(level_known()){
   snprintf(s,sizeof(s),"OS BATTERY: %lu%%",(unsigned long)snap.percent);
   text8(16,64,s,35,teal);
   fill(16,87,288,17,RGB(32,57,72));
   fill(18,89,(int)(284u*snap.percent/100u),13,teal);
  }else text8(16,64,"OS BATTERY: UNAVAILABLE",35,white);
  text8(16,116,charge_mode(),35,white);
  snprintf(s,sizeof(s),"SAMPLES: %u",hn);
  text8(16,138,s,35,white);
  text8(16,160,"COARSE OS LEVEL ONLY",35,muted);
  text8(16,175,"NO VOLTAGE/HEALTH PROBE",35,muted);
 }else{
  text8(11,38,"6/6 SYSTEM DETAILS",36,white);
  panel(8,55,304,137);
  snprintf(s,sizeof(s),"OS SCAN: %lu/1024K",(unsigned long)(batt.scanned_words/1024ul));
  text8(16,64,s,35,white);
  snprintf(s,sizeof(s),"OS CAP:%02lX ERR:%02lX",
    (unsigned long)(snap.capabilities&255u),(unsigned long)(snap.errors&255u));
  text8(16,85,s,35,teal);
  if(hn){
   unsigned mn=100,mx=0,sum=0;
   for(unsigned n=0;n<hn;n++){
    unsigned value=history[(hi+HISTORY-hn+n)%HISTORY];
    if(value<mn)mn=value;if(value>mx)mx=value;sum+=value;
   }
   snprintf(s,sizeof(s),"BATT MIN:%u%% MAX:%u%%",mn,mx);
   text8(16,107,s,35,white);
   snprintf(s,sizeof(s),"BATT AVG:%u%%",(sum+hn/2u)/hn);
   text8(16,129,s,35,white);
  }else text8(16,107,"BATT HISTORY: NOT READY",35,muted);
  text8(16,153,"STORAGE FREE: UNKNOWN",35,muted);
  text8(16,172,"NO AUTO EXIT / NO WRITES",35,muted);
 }
 fill(0,207,320,33,panelbg);fill(0,207,320,1,RGB(25,132,158));
 text8(8,211,"MENU/RIGHT NEXT LEFT PREV",36,teal);
 text8(8,225,"ESC/ENTER FAST EXIT",36,white);
 eh_platform_flip();
}

int main(void){
 memset(&snap,0,sizeof snap);battery_native_init(&batt);
 sample_cpu();
 if(!eh_platform_start())return 1;
 draw();
 for(uint32_t t=0;;t++){
  EhKey key=eh_platform_readkey();
  if(key==EH_KEY_ESC||key==EH_KEY_ENTER)break;
  int changed=0;
  if(key==EH_KEY_MENU||key==EH_KEY_RIGHT){page=(page+1u)%PAGE_COUNT;changed=1;}
  if(key==EH_KEY_LEFT){page=(page+PAGE_COUNT-1u)%PAGE_COUNT;changed=1;}
  if(key==EH_KEY_UP&&page==2u){heap_check();changed=1;}
  if(key==EH_KEY_UP&&page==3u){docs_scan();changed=1;}
  if((!batt.complete && t%5u==0u)||(batt.complete && t%67u==0u)){
   batt_valid=battery_native_default(&batt,&snap,batt.complete?0u:16384u);
   if(batt.complete){updates++;remember_battery();}
   if(page==0u||page==4u||page==5u)changed=1;
  }
  if(changed)draw();
  eh_platform_sleep();
 }
 eh_platform_stop_mode(0);
 return 0;
}
