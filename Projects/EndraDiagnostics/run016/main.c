/*
 * EndraDiagnostics Run016: practical monitor; preserves proven Run014 platform.
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
 * Bounded scan: 256 directory entries, at most 16 folders, depth <= 3.
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
static void draw(void){
 const uint16_t bg=RGB(9,17,29),panel=RGB(25,41,58),white=RGB(236,244,249);
 const uint16_t teal=RGB(66,224,199),muted=RGB(163,183,195);
 char s[54];
 fb=eh_framebuffer();fill(0,0,320,240,bg);
 fill(0,0,320,33,panel);fill(0,0,4,33,teal);
 text8(10,12,"ENDRA SYSTEM MONITOR",22,teal);
 text8(277,12,"016",3,white);
 if(page==0){
  text8(11,40,"1/6 SYSTEM OVERVIEW",34,white);
  text8(11,64,"CPU: ARM926EJ-S",34,teal);
  if(level_known())snprintf(s,sizeof(s),"BATTERY: %lu%%",(unsigned long)snap.percent);
  else snprintf(s,sizeof(s),"BATTERY: UNAVAILABLE");
  text8(11,86,s,36,white);
  text8(11,108,"RAM FREE: UNAVAILABLE",36,white);
  text8(11,130,"STORAGE FREE: UNAVAILABLE",36,white);
  if(doc_state==1||doc_state==2){
    if(doc_size_known)snprintf(s,sizeof(s),"DOCS KNOWN: %lu KB%s",
       (unsigned long)(doc_bytes/1024ull),doc_state==2?" PART":"");
    else snprintf(s,sizeof(s),"DOC SIZE: UNAVAILABLE");
    text8(11,152,s,35,teal);
  }else text8(11,152,"DOCS: SCAN NOT RUN",36,muted);
  text8(11,172,"REAL DATA / NO GUESSED %",36,muted);
 }else if(page==1){
  text8(11,40,"2/6 CPU DETAILS",36,white);
  snprintf(s,sizeof(s),"CPU ID: %08lX",(unsigned long)cpu_id);
  text8(11,63,s,36,teal);
  snprintf(s,sizeof(s),"REVISION: %u",(unsigned)(cpu_id&15u));
  text8(11,84,s,36,white);
  snprintf(s,sizeof(s),"CACHE ID: %08lX",(unsigned long)cache_id);
  text8(11,105,s,36,white);
  snprintf(s,sizeof(s),"I-CACHE:%s D-CACHE:%s",
    (control&(1u<<12))?"ON":"OFF",(control&(1u<<2))?"ON":"OFF");
  text8(11,126,s,36,white);
  text8(11,148,"CPU LOAD: NOT AVAILABLE",36,muted);
  text8(11,169,"CPU MHz: NOT MEASURED",36,muted);
 }else if(page==2){
  text8(11,40,"3/6 RAM AND MEMORY",36,white);
  text8(11,61,"TOTAL RAM: NOT VERIFIED",36,muted);
  text8(11,82,"FREE RAM: NOT AVAILABLE",36,muted);
  text8(11,103,"USED RAM: NOT AVAILABLE",36,muted);
  text8(11,124,"UP: 4KB HEAP CHECK",36,teal);
  text8(11,146,heap_result==0?"HEAP TEST: NOT RUN":
    heap_result==1?"HEAP TEST: PASS":
    heap_result==2?"HEAP TEST: ALLOC FAILED":"HEAP TEST: MISMATCH",36,white);
  snprintf(s,sizeof(s),"MMU: %s TESTS:%u",
    (control&1u)?"ON":"OFF",heap_runs);
  text8(11,170,s,36,white);
 }else if(page==3){
  text8(11,40,"4/6 DOCUMENT STORAGE",36,white);
  text8(11,60,"UP: SAFE SCAN + SIZE CHECK",36,teal);
  text8(11,81,doc_state==0?"STATUS: NOT SCANNED":
    doc_state==1?"STATUS: COMPLETED":
    doc_state==2?"STATUS: PARTIAL":"STATUS: UNAVAILABLE",36,white);
  snprintf(s,sizeof(s),"FILES:%u FOLDERS:%u",doc_files,doc_dirs);
  text8(11,102,s,36,white);
  snprintf(s,sizeof(s),"SCANNED:%u ERR:%u",doc_examined,doc_errors);
  text8(11,123,s,36,white);
  snprintf(s,sizeof(s),"KNOWN FILE BYTES: %llu",doc_bytes);
  text8(11,144,s,36,teal);
  if(doc_size_known){
   snprintf(s,sizeof(s),"MEASURED:%u UNKNOWN:%u",doc_size_known,doc_size_unknown);
   text8(11,165,s,36,white);
  }else text8(11,165,"FILE SIZES: UNAVAILABLE",36,muted);
  text8(11,182,"TOTAL/FREE/PERCENT: UNKNOWN",36,muted);
 }else if(page==4){
  text8(11,40,"5/6 LIVE BATTERY",36,white);
  if(level_known()){
   snprintf(s,sizeof(s),"OS LEVEL: %lu%%",(unsigned long)snap.percent);
   text8(11,61,s,36,teal);
   fill(12,84,294,18,panel);
   fill(15,87,(int)(288u*snap.percent/100u),12,teal);
  }else text8(11,61,"OS LEVEL: --",36,teal);
  text8(11,111,charge_mode(),36,white);
  snprintf(s,sizeof(s),"SAMPLES: %u",hn);
  text8(11,133,s,36,white);
  text8(11,155,"OS COARSE LEVEL / NO ADC",36,muted);
  text8(11,174,"NO BATTERY HEALTH CLAIM",36,muted);
 }else{
  text8(11,40,"6/6 MONITOR DETAILS",36,white);
  snprintf(s,sizeof(s),"OS SCAN: %lu/1024K",(unsigned long)batt.scanned_words/1024ul);
  text8(11,61,s,36,white);
  snprintf(s,sizeof(s),"OS ERR:%02lX CAP:%02lX",
    (unsigned long)(snap.errors&255u),(unsigned long)(snap.capabilities&255u));
  text8(11,83,s,36,white);
  if(hn){
   unsigned lo=100,high=0,tot=0;
   for(unsigned n=0;n<hn;n++){
    unsigned v=history[(hi+HISTORY-hn+n)%HISTORY];
    if(v<lo)lo=v;if(v>high)high=v;tot+=v;
   }
   snprintf(s,sizeof(s),"BATT MIN:%u%% MAX:%u%%",lo,high);
   text8(11,107,s,36,teal);
   snprintf(s,sizeof(s),"BATT AVG:%u%%",(tot+hn/2u)/hn);
   text8(11,129,s,36,white);
  }else text8(11,107,"BATT HISTORY: NONE",36,muted);
  text8(11,155,"NO AUTO EXIT / NO FS WRITES",36,muted);
  text8(11,174,"READINGS NOT CALIBRATED",36,muted);
 }
 fill(8,197,304,31,panel);
 text8(12,200,"MENU/RIGHT NEXT LEFT PREV",36,teal);
 text8(12,214,"ESC/ENTER FAST EXIT",36,muted);
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
