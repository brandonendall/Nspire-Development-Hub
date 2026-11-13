/* EndraCommander NX: original unified interface; 320 x 240. */
#include "nx.h"
#include "nx_platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include "font8x8_basic.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_ONLY_BMP
#define STBI_NO_HDR
#define STBI_NO_SIMD
#define STBI_NO_THREAD_LOCALS
#define STBI_MAX_DIMENSIONS 4096
#include "stb_image.h"
#define ROWS 14
#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
/* Run003 diagnostic: preserve browsing and edits, but do not nest Ndless applications. */
#define NX_RUN003_SAFE_LAUNCH 1
static uint16_t pixels[320*240];
static unsigned diag_actions,diag_heartbeats;
static int diag_last_key,diag_readonly=1;
static NxPane panes[2];
static int active,light,quit;
static char workspace[NX_PATH]="/documents",status[160]="Ready. MENU shows every action.";
static char config_path[NX_PATH];
static uint16_t bg,fg,muted,panel,accent,selected;
static const uint16_t gold=RGB(244,194,84);
static void palette(void){bg=light?RGB(237,241,246):RGB(13,20,30);fg=light?RGB(24,36,48):RGB(226,235,244);muted=light?RGB(80,99,112):RGB(137,156,174);panel=light?RGB(218,229,238):RGB(25,37,52);accent=light?RGB(0,95,114):RGB(60,216,211);selected=light?RGB(175,216,229):RGB(31,75,91);}
static void rect(int x,int y,int w,int h,uint16_t c){int endx=x+w,endy=y+h;if(x<0)x=0;if(y<0)y=0;if(endx>320)endx=320;if(endy>240)endy=240;for(int j=y;j<endy;++j)for(int i=x;i<endx;++i)pixels[j*320+i]=c;}
static void text(int x,int y,const char *s,uint16_t c,int max){for(int i=0;s[i]&&i<max;++i){unsigned char ch=(unsigned char)s[i];if(ch>=128||ch<32)ch='?';for(int row=0;row<8;++row)for(int col=0;col<8;++col)if((unsigned char)font8x8_basic[ch][row]&(1u<<col))rect(x+i*8+col,y+row,1,1,c);}}
static void tailtext(int x,int y,const char *s,uint16_t c,int max){size_t n=strlen(s);if(n>(size_t)max){text(x,y,"<",c,1);text(x+8,y,s+n-(max-1),c,max-1);}else text(x,y,s,c,max);}
static void clear(void){palette();rect(0,0,320,240,bg);}
static void title(const char *s){rect(0,0,320,20,panel);text(6,6,s,accent,38);}
static void footer(const char *s){rect(0,222,320,18,panel);text(5,227,s,muted,38);}
/* Every screen carries an in-RAM breadcrumb; no flash writes or hooks. */
static void present(void){
    char stamp[32];
    rect(164,0,156,20,panel);
    snprintf(stamp,sizeof(stamp),"A%03u H%03u K%03d",
        diag_actions%1000,diag_heartbeats%1000,diag_last_key);
    text(171,6,stamp,gold,18);
    platform_present(pixels);
}
void platform_trace_event(int key){
    if(key){++diag_actions;diag_last_key=key;}
    else ++diag_heartbeats;
    present(); /* display key BEFORE executing the action that might hang */
}
static NxEntry *entry(void){NxPane *p=&panes[active];return p->count?p->items+p->cursor:NULL;}
static int current_path(char *out){NxEntry *e=entry();return e?nx_join(out,panes[active].path,e->name):nx_fail("This folder is empty");}
static void render(void){
    clear();title("ENDRACOMMANDER NX");
    for(int k=0;k<2;++k){NxPane *p=&panes[k];int x=k*160;char head[32];
        rect(x,23,160,18,panel);if(k==active)rect(x,23,3,165,accent);
        snprintf(head,sizeof(head),"%c %s %s",k==active?'>':' ',k?"RIGHT":"LEFT",p->folders_only?"DIRS":"FILES");text(x+6,28,head,k==active?accent:muted,18);
        if(p->cursor<p->scroll)p->scroll=p->cursor;if(p->cursor>=p->scroll+ROWS)p->scroll=p->cursor-ROWS+1;
        for(int r=0;r<ROWS;++r){size_t i=p->scroll+(size_t)r;int y=45+r*10;if(i>=p->count)break;NxEntry *e=p->items+i;
            if(i==p->cursor)rect(x+4,y-1,152,10,k==active?selected:panel);
            char mark[3]={e->selected?'*':e->directory?'+':' ',0};text(x+7,y,mark,e->selected?gold:e->directory?accent:muted,1);text(x+17,y,e->name,e->directory?accent:fg,17);
        }
        if(!p->count)text(x+12,62,p->filter[0]?"No matches":"Empty folder",muted,17);
    }
    rect(159,23,1,165,muted);rect(0,190,320,1,muted);
    NxPane *p=&panes[active];NxEntry *e=entry();char info[160],size[24];unsigned count=0;
    for(size_t i=0;i<p->count;++i)if(p->items[i].selected)++count;
    if(e){nx_size(size,sizeof(size),e->size);snprintf(info,sizeof(info),"%s | %u marked",e->directory?"Folder":size,count);}else snprintf(info,sizeof(info),"%lu items | %s sort",(unsigned long)p->count,p->sort==0?"Name":p->sort==1?"Size":"Date");
    tailtext(5,194,p->path,muted,38);text(5,204,e?e->name:status,fg,38);text(5,214,info,muted,38);
    footer(diag_readonly?"005 SAFE  Q unlock  MENU actions":"005 WRITE  Q lock  MENU actions");present();
}
static void wrapped(const char *s,int x,int y,int cols,int rows,uint16_t color){int row=0,col=0;for(;*s&&row<rows;++s){if(*s=='\r')continue;if(*s=='\n'){++row;col=0;continue;}char ch[2]={*s=='\t'?' ':*s,0};text(x+col*8,y+row*10,ch,color,1);if(++col==cols){col=0;++row;}}}
static void message(const char *heading,const char *body){clear();title(heading);wrapped(body,8,32,38,18,fg);footer("ENTER / ESC back");present();int k;do{k=platform_key();}while(k!=K_ENTER&&k!=K_ESC);}
static int confirm(const char *heading,const char *body){int yes=0;for(;;){clear();title(heading);wrapped(body,8,36,38,13,fg);rect(16,184,136,24,yes?panel:selected);rect(168,184,136,24,yes?selected:panel);text(58,192,"NO",fg,2);text(210,192,"YES",yes?gold:fg,3);footer("LEFT / RIGHT choose   ENTER confirm");present();int k=platform_key();if(k==K_ESC)return 0;if(k==K_LEFT||k==K_RIGHT||k==K_TAB)yes=!yes;if(k==K_ENTER)return yes;}}
static int input(const char *heading,const char *prompt,char *value,size_t cap){size_t n=strlen(value);for(;;){clear();title(heading);wrapped(prompt,8,35,38,8,fg);rect(8,126,304,26,panel);tailtext(12,135,value,fg,36);text(8,171,"SHIFT: capitals   SHIFT-minus: _",muted,38);text(8,185,"DEL: erase        divide: /",muted,38);footer("ENTER accept   ESC cancel");present();int k=platform_key();if(k==K_ESC)return 0;if(k==K_ENTER)return 1;if(k==K_DELETE&&n)value[--n]=0;else if(k>=32&&k<127&&n+1<cap){value[n++]=(char)k;value[n]=0;}}}
static int navigate(NxPane *p,const char *path){char old[NX_PATH],dest[NX_PATH];if(!nx_normalize(dest,path))return 0;strcpy(old,p->path);strcpy(p->path,dest);size_t prev=p->cursor;p->cursor=p->scroll=0;if(!nx_scan(p)){strcpy(p->path,old);p->cursor=prev;return 0;}return 1;}
static void refresh(void){for(int k=0;k<2;++k)if(!nx_scan(&panes[k]))snprintf(status,sizeof(status),"%s",nx_error);}
static void error(void){message("Could not finish",nx_error);}
static void config_save(void){
    if(!config_path[0])return;char dir[NX_PATH];strcpy(dir,config_path);nx_parent(dir);
    if(!nx_is_dir(dir)&&!nx_mkdir(dir))return;
    if(!nx_writable(config_path))return;
    FILE *old=fopen(config_path,"rb");if(old){char magic[8]={0};fread(magic,1,7,old);fclose(old);if(strcmp(magic,"UCNX1\n\n"))return;}
    FILE *f=fopen(config_path,"wb");if(f){fprintf(f,"UCNX1\n\n%d\n%s\n",light,workspace);fclose(f);}
}
static void config_load(void){FILE *f=fopen(config_path,"rb");if(!f)return;char line[NX_PATH];if(fgets(line,sizeof(line),f)&&!strcmp(line,"UCNX1\n")){fgets(line,sizeof(line),f);if(fgets(line,sizeof(line),f))light=atoi(line)!=0;if(fgets(line,sizeof(line),f)){line[strcspn(line,"\r\n")]=0;char path[NX_PATH];if(nx_normalize(path,line)&&nx_is_dir(path))strcpy(workspace,path);}}fclose(f);}
static int progress(const char *name,uint64_t amount){static char last[NX_PATH];static uint64_t previous;int change=strcmp(last,name)||amount<previous||amount-previous>=262144;
    if(platform_cancel())return 0;
    if(change){char size[32];snprintf(last,sizeof(last),"%s",name);previous=amount;clear();title("Working...");wrapped(name,8,40,38,10,fg);nx_size(size,sizeof(size),amount);text(8,166,size,accent,32);footer("ESC cancel when operation responds");present();}return 1;}
static void view_text_buffer(const char *heading,const char *data){size_t pos=0,n=strlen(data),history[256];unsigned page=0;for(;;){clear();title(heading);size_t next=pos;int row=0,col=0;for(;next<n&&row<18;++next){unsigned char c=(unsigned char)data[next];if(c=='\r')continue;if(c=='\n'){++row;col=0;continue;}char ch[2]={c=='\t'?' ':c>=32&&c<127?(char)c:'.',0};text(8+col*8,30+row*10,ch,fg,1);if(++col==38){col=0;++row;}}
        footer("UP / DOWN page   ESC back");present();int k=platform_key();if(k==K_ESC||k==K_ENTER)return;if(k==K_DOWN&&next<n&&page<256){history[page++]=pos;pos=next;}if(k==K_UP&&page)pos=history[--page];}}
/* Run005: bounded preview avoids repeated fseek/ftell on Nspire filesystem,
 * uses a heap buffer, and keeps page history out of the GUI stack. */
static void view_file(const char *path,int hex){
    enum { PREVIEW_BYTES = 4096, PREVIEW_PAGES = 40 };
    static size_t history[PREVIEW_PAGES];
    unsigned char *data=malloc(PREVIEW_BYTES);
    if(!data){message("File preview","Not enough memory for preview");return;}
    FILE *f=fopen(path,"rb");
    if(!f){free(data);nx_fail("Cannot open file for preview");error();return;}
    size_t n=fread(data,1,PREVIEW_BYTES,f);
    int io_error=ferror(f);
    int truncated=0;
    if(!io_error && n==PREVIEW_BYTES)truncated=fgetc(f)!=EOF;
    if(ferror(f))io_error=1;
    fclose(f);
    if(io_error){free(data);message("File preview","File could not be read safely.");return;}
    if(!n){free(data);message("File preview","This file is empty.");return;}
    size_t pos=0;
    unsigned page=0;
    for(;;){
        clear();title(hex?"HEX PREVIEW 005":"TEXT PREVIEW 005");
        size_t next=pos;
        int row=0,col=0;
        if(hex){
            for(row=0;row<18&&next<n;++row){
                char line[64],part[8];
                size_t index=next;
                size_t take=n-next<8?n-next:8;
                snprintf(line,sizeof(line),"%06lX ",(unsigned long)index);
                for(size_t j=0;j<take;++j){
                    snprintf(part,sizeof(part),"%02X ",(unsigned)data[next++]);
                    strcat(line,part);
                }
                text(5,30+row*10,line,fg,38);
            }
        }else{
            while(row<18 && next<n){
                unsigned char c=data[next++];
                if(c=='\r')continue;
                if(c=='\n'){++row;col=0;continue;}
                char ch[2]={c=='\t'?' ':(c>=32&&c<127?(char)c:'.'),0};
                text(8+col*8,30+row*10,ch,fg,1);
                if(++col==38){col=0;++row;}
            }
        }
        footer(truncated?"FIRST 4KB ONLY  DOWN PAGE  ESC BACK":"UP/DOWN PAGE  H MODE  ESC BACK");
        present();
        int k=platform_key();
        if(k==K_ESC||k==K_ENTER)break;
        if(k==K_DOWN&&next<n&&page<PREVIEW_PAGES){
            history[page++]=pos;
            pos=next;
        }
        if(k==K_UP&&page)pos=history[--page];
        if(k=='h'||k=='H'){hex=!hex;pos=0;page=0;}
    }
    free(data);
}
static void view_image(const char *path){int w,h,channels;if(!stbi_info(path,&w,&h,&channels)||w<1||h<1||(uint64_t)w*h>2097152){message("Image viewer","Cannot read image, or image exceeds\n2 million pixels.\n\nSupported: PNG, JPG and BMP.\nThe .tns suffix is accepted.");return;}
    clear();title("Loading image...");present();unsigned char *data=stbi_load(path,&w,&h,&channels,3);if(!data){message("Image viewer","Image decoding failed or memory\nis insufficient.");return;}
    int dw=320,dh=(int)((long)h*320/w);if(dh>198){dh=198;dw=(int)((long)w*198/h);}if(dw<1)dw=1;if(dh<1)dh=1;clear();title("IMAGE VIEWER");int x0=(320-dw)/2,y0=22+(198-dh)/2;
    for(int y=0;y<dh;++y)for(int x=0;x<dw;++x){size_t j=((size_t)(y*h/dh)*w+(x*w/dw))*3;pixels[(y0+y)*320+x0+x]=RGB(data[j],data[j+1],data[j+2]);}
    stbi_image_free(data);footer("Fit to screen   ENTER / ESC back");present();int k;do{k=platform_key();}while(k!=K_ESC&&k!=K_ENTER);
}
static void open_entry(void){char path[NX_PATH];NxEntry *e=entry();if(!e)return;if(!current_path(path)){error();return;}if(e->directory){if(!navigate(&panes[active],path))error();return;}
    int kind=nx_kind(path);if(kind==1)view_file(path,0);else if(kind==4)view_image(path);else if(kind==3){char *data=malloc(16384);if(!data){message("ZIP","Not enough memory");return;}if(nx_zip_list(path,data,16384))view_text_buffer("ZIP CONTENTS",data);else error();free(data);}else if(kind==2){
#if NX_RUN003_SAFE_LAUNCH
        message("Run003 safety mode","Nested app launching is temporarily disabled while diagnosing calculator freezes.\n\nExit EndraCommander and launch the app from Documents instead.");
#else
        if(confirm("Launch Ndless app?",e->name)){int result=platform_launch(path);refresh();if(result<0)message("App launch","Ndless could not launch this file.");}
#endif
    }else view_file(path,1);
}
static void batch(int mode){NxPane *p=&panes[active];size_t count=0;char prompt[256];for(size_t i=0;i<p->count;++i)if(p->items[i].selected)++count;if(!p->count)return;int marked=count>0;if(!count)count=1;
    snprintf(prompt,sizeof(prompt),"%s %lu item(s)?\n\n%s\n\n%s",mode==0?"Copy":mode==1?"Move":"Permanently delete",(unsigned long)count,mode==2?"Selected files and empty folders.":panes[1-active].path,mode==2?"Deletion cannot be undone.":"Existing files will not be overwritten.");
    if(!confirm(mode==2?"Confirm deletion":"Confirm transfer",prompt))return;
    size_t completed=0;for(size_t i=0;i<p->count;++i){if(marked?!p->items[i].selected:i!=p->cursor)continue;char src[NX_PATH],dst[NX_PATH];int ok=nx_join(src,p->path,p->items[i].name);if(ok&&mode!=2)ok=nx_join(dst,panes[1-active].path,p->items[i].name);if(ok)ok=mode==0?nx_copy(src,dst):mode==1?nx_move(src,dst):nx_delete(src);if(!ok){char msg[256];snprintf(msg,sizeof(msg),"%lu item(s) completed.\n\n%s",(unsigned long)completed,nx_error);message("Operation stopped",msg);break;}++completed;}
    refresh();snprintf(status,sizeof(status),"Completed %lu item(s)",(unsigned long)completed);
}
static void rename_entry(void){NxEntry *e=entry();char name[NX_NAME],src[NX_PATH],dst[NX_PATH];if(!e||!current_path(src))return;strcpy(name,e->name);if(!input("Rename","Edit the file or folder name:",name,sizeof(name)))return;if(!strcmp(name,e->name))return;if(!nx_join(dst,panes[active].path,name)||!nx_move(src,dst))error();refresh();}
static void new_folder(void){char name[NX_NAME]="",path[NX_PATH];if(input("New folder","Folder name:",name,sizeof(name))){if(!nx_join(path,panes[active].path,name)||!nx_mkdir(path))error();refresh();}}
static void zip_action(int extract){char src[NX_PATH],name[NX_NAME],dst[NX_PATH];NxEntry *e=entry();if(!e||!current_path(src))return;if(extract&&e->directory){message("Extract ZIP","Select a ZIP file first.");return;}
    size_t len=strlen(e->name);if(len>NX_NAME-20){message("ZIP","The selected name is too long.");return;}
    strcpy(name,e->name);strcat(name,extract?"_extracted":".zip.tns");if(!input(extract?"Extract ZIP":"Create ZIP","Destination name in the OTHER pane:",name,sizeof(name)))return;
    if(!nx_join(dst,panes[1-active].path,name)){error();return;}
    if(!(extract?nx_zip_extract(src,dst):nx_zip_create(src,dst)))error();else message("ZIP completed",extract?"Files extracted to the other pane.\nThe original ZIP was kept.":"ZIP created in the other pane.\nSource files were kept.");refresh();
}
static void workspace_set(void){char path[NX_PATH];NxEntry *e=entry();if(e&&e->directory){if(!current_path(path))return;}else strcpy(path,panes[active].path);strcpy(workspace,path);config_save();message("Workspace saved",workspace);}
static void go_path(void){char path[NX_PATH];strcpy(path,panes[active].path);if(input("Go to folder","Absolute folder path:",path,sizeof(path))&&!navigate(&panes[active],path))error();}
static void help(void){message("EndraCommander NX | Run 005","TAB switch panes\nArrows browse; LEFT goes up\nENTER open; SPACE mark\nC copy   M move   R rename\nN folder DEL delete\nV text   H hex    Z ZIP list\nP make ZIP   X extract ZIP\nF folders-only  S name filter\nT sort   A mark all\nW save workspace  B jump back\nG go to path  D Documents\nY light/dark  I device info\nMENU all actions  ESC quit\n\nInspired by Commander, SDL File\nBrowser and nSonic. Native nSonic\nOS hook is not installed.");}
static void action(int k);
static void menu(void){const char *labels[]={"Open selected","Copy to other pane","Move to other pane","Rename selected","Create folder","Delete marked/selected","Text viewer","Hex viewer","ZIP contents","Create ZIP in other pane","Extract ZIP to other pane","Toggle folders-only","Filter names in this folder","Sort: name / size / date","Save workspace folder","Open saved workspace","Go to folder path","Go to Documents","Toggle light / dark","Device info","Help / credits","Toggle diagnostic write lock"};
    const int keys[]={K_ENTER,'C','M','R','N',K_DELETE,'V','H','Z','P','X','F','S','T','W','B','G','D','Y','I','?','Q'};int chosen=0,top=0;for(;;){if(chosen<top)top=chosen;if(chosen>=top+18)top=chosen-17;clear();title("ACTIONS");for(int j=0;j<18&&top+j<22;++j){int i=top+j;int y=29+j*10;if(i==chosen)rect(4,y-1,312,10,selected);text(10,y,labels[i],i==chosen?accent:fg,37);}footer("UP / DOWN choose   ENTER   ESC back");present();int k=platform_key();if(k==K_ESC)return;if(k==K_UP&&chosen>0)--chosen;if(k==K_DOWN&&chosen<21)++chosen;if(k==K_ENTER){action(keys[chosen]);return;}}
}
static void action(int key){NxPane *p=&panes[active];int k=key<256?toupper((unsigned char)key):key;char path[NX_PATH];
    if(k=='Q'){
        if(diag_readonly){
            if(confirm("Unlock file writes?","Run005 diagnostics starts READ ONLY.\n\nOnly unlock to test disposable files. Freezes are still under investigation."))diag_readonly=0;
        }else diag_readonly=1;
        return;
    }
    if(diag_readonly&&(k=='C'||k=='M'||k==K_DELETE||k=='R'||k=='N'||
                       k=='P'||k=='X'||k=='W'||k=='Y')){
        message("Read-only diagnostic","Writes are locked for safe testing.\n\nUse Q or MENU > Toggle diagnostic write lock to enable writes only on disposable files.");
        return;
    }
    switch(k){
    case K_UP:if(p->cursor)--p->cursor;break;
    case K_DOWN:if(p->cursor+1<p->count)++p->cursor;break;
    case K_LEFT:strcpy(path,p->path);nx_parent(path);if(!navigate(p,path))error();break;
    case K_RIGHT:if(entry()&&entry()->directory)open_entry();break;
    case K_ENTER:open_entry();break;
    case K_TAB:active=1-active;break;
    case K_ESC:quit=1;break;
    case K_MENU:menu();break;
    case ' ':if(entry()){entry()->selected=!entry()->selected;if(p->cursor+1<p->count)++p->cursor;}break;
    case 'A':{int all=p->count>0;for(size_t i=0;i<p->count;++i)all&=p->items[i].selected;for(size_t i=0;i<p->count;++i)p->items[i].selected=!all;break;}
    case 'C':batch(0);break;case 'M':batch(1);break;case K_DELETE:batch(2);break;
    case 'R':rename_entry();break;case 'N':new_folder();break;
    case 'V':case 'H':if(entry()&&!entry()->directory&&current_path(path))view_file(path,k=='H');break;
    case 'Z':if(entry()&&!entry()->directory&&current_path(path)){char *data=malloc(16384);if(data){if(nx_zip_list(path,data,16384))view_text_buffer("ZIP CONTENTS",data);else error();free(data);}}break;
    case 'P':zip_action(0);break;case 'X':zip_action(1);break;
    case 'F':p->folders_only=!p->folders_only;p->cursor=p->scroll=0;if(!nx_scan(p))error();break;
    case 'S':{char q[64];strcpy(q,p->filter);if(input("Filter current folder","Part of a name. Blank shows everything:",q,sizeof(q))){strcpy(p->filter,q);p->cursor=p->scroll=0;if(!nx_scan(p))error();}break;}
    case 'T':p->sort=(p->sort+1)%3;nx_sort(p);p->cursor=p->scroll=0;break;
    case 'W':workspace_set();break;case 'B':if(!navigate(p,workspace))error();break;
    case 'G':go_path();break;case 'D':if(!navigate(p,nx_write_root))error();break;
    case 'Y':light=!light;config_save();break;
    case 'I':{char info[768];platform_info(info,sizeof(info));message("Device information",info);break;}
    case '?':help();break;default:break;
}}
int main(int argc,char **argv){
    const char *root="/documents";nx_progress=progress;
#ifndef _TINSPIRE
    if(argc>1)root=argv[1];
#else
    (void)argc;(void)argv;
#endif
    if(!nx_is_dir(root))root="/";
    for(int k=0;k<2;++k){snprintf(panes[k].path,sizeof(panes[k].path),"%s",root);nx_scan(&panes[k]);}
    snprintf(workspace,sizeof(workspace),"%s",root);
    char config_dir[NX_PATH];if(nx_join(config_dir,nx_write_root,"UltraCommanderNX")&&nx_join(config_path,config_dir,"settings.cfg.tns"))config_load();else config_path[0]=0;
    if(!platform_init()){nx_release(&panes[0]);nx_release(&panes[1]);return 1;}
#ifndef _TINSPIRE
    render();if(argc>3){if(!strcmp(argv[3],"@menu"))menu();else for(size_t i=0;i<panes[0].count;++i)if(!strcmp(panes[0].items[i].name,argv[3])){panes[0].cursor=i;open_entry();break;}}
    const char *output=argc>2?argv[2]:"preview.ppm";FILE *f=fopen(output,"wb");if(f){fprintf(f,"P6\n320 240\n255\n");for(int i=0;i<320*240;++i){uint16_t c=pixels[i];unsigned char rgb[3]={(unsigned char)(((c>>11)&31)*255/31),(unsigned char)(((c>>5)&63)*255/63),(unsigned char)((c&31)*255/31)};fwrite(rgb,1,3,f);}fclose(f);}quit=1;
#endif
    while(!quit){render();int key=platform_key();if(key)action(key);}
    platform_close();nx_release(&panes[0]);nx_release(&panes[1]);return 0;
}
