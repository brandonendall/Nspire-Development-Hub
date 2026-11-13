/* EndraHome Run001: stable SDL desktop, configured app shortcuts only. */
#include <os.h>
#include <libndls.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <string.h>
#include "shortcuts.h"
#define W 320
#define H 240
#define CFG "/documents/EndraHome/shortcuts.cfg.tns"
static SDL_Surface *screen;
static nSDL_Font *font_main,*font_dim,*font_cyan;
static EhShortcut slots[EH_SLOTS];
static int chosen,help,running=1;
static char notice[72]="Choose a dock icon with arrows";
static Uint32 rgb(int r,int g,int b) { return SDL_MapRGB(screen->format,(Uint8)r,(Uint8)g,(Uint8)b); }
static void box(int x,int y,int w,int h,Uint32 c) {
    SDL_Rect a={(Sint16)x,(Sint16)y,(Uint16)w,(Uint16)h};SDL_FillRect(screen,&a,c);
}
static void txt(int x,int y,const char *s,nSDL_Font *f) {
    if(f) nSDL_DrawString(screen,f,x,y,"%s",s);
}
static void clipped(int x,int y,const char *s,int limit,nSDL_Font *f) {
    char b[48];size_t n=strlen(s);
    if(limit>47)limit=47;
    if(n>(size_t)limit)n=(size_t)limit;
    memcpy(b,s,n);b[n]=0;txt(x,y,b,f);
}
static int start_video(void) {
    if(SDL_Init(SDL_INIT_VIDEO)<0)return 0;
    screen=SDL_SetVideoMode(W,H,has_colors?16:8,SDL_SWSURFACE);
    if(!screen){SDL_Quit();return 0;}
    font_main=nSDL_LoadFont(NSDL_FONT_TINYTYPE,235,242,249);
    font_dim=nSDL_LoadFont(NSDL_FONT_TINYTYPE,148,163,184);
    font_cyan=nSDL_LoadFont(NSDL_FONT_TINYTYPE,69,222,218);
    if(!font_main||!font_dim||!font_cyan){
        if(font_main)nSDL_FreeFont(font_main);
        if(font_dim)nSDL_FreeFont(font_dim);
        if(font_cyan)nSDL_FreeFont(font_cyan);
        font_main=font_dim=font_cyan=NULL;
        SDL_Quit();screen=NULL;return 0;
    }
    return 1;
}
static void stop_video(void){
    if(font_main)nSDL_FreeFont(font_main);
    if(font_dim)nSDL_FreeFont(font_dim);
    if(font_cyan)nSDL_FreeFont(font_cyan);
    font_main=font_dim=font_cyan=NULL;
    if(screen)SDL_Quit();
    screen=NULL;
}
static void icon(int x,int y,int i,int active){
    static const char *glyph[]={"GBA","FILE","ZIP","APP"};
    static const unsigned char shades[4][3]={{54,163,185},{100,160,219},{173,129,214},{101,189,143}};
    box(x,y,70,76,rgb(active?28:22,active?62:38,active?77:55));
    if(active){
        Uint32 line=rgb(63,222,220);
        box(x,y,70,2,line);box(x,y+74,70,2,line);
        box(x,y,2,76,line);box(x+68,y,2,76,line);
    }
    box(x+15,y+8,40,37,rgb(shades[i][0],shades[i][1],shades[i][2]));
    box(x+19,y+12,32,29,rgb(15,29,44));
    txt(x+35-(int)strlen(glyph[i])*4,y+22,glyph[i],font_main);
    clipped(x+4,y+51,slots[i].name,8,active?font_cyan:font_main);
    txt(x+4,y+64,slots[i].path[0]?"READY":"SET PATH",font_dim);
}
static void paint(void){
    if(!screen)return;
    SDL_FillRect(screen,NULL,rgb(10,20,33));
    box(0,0,W,30,rgb(20,36,55));box(0,0,5,30,rgb(59,211,209));
    txt(13,9,"ENDRA HOME",font_cyan);txt(258,9,"001",font_dim);
    if(!help){
        box(11,43,298,73,rgb(23,41,58));box(11,43,3,73,rgb(55,179,201));
        txt(26,58,"Your Nspire. Your home.",font_main);
        txt(26,77,"Launch your favorite Ndless apps.",font_dim);
        txt(26,96,"SDL desktop | no system hooks",font_dim);
        txt(12,129,"QUICK LAUNCH",font_cyan);
        box(9,143,302,80,rgb(17,30,47));
        for(int i=0;i<EH_SLOTS;i++)icon(14+76*i,145,i,i==chosen);
        box(0,225,W,15,rgb(20,36,55));
        clipped(5,228,notice,39,font_dim);
    }else{
        box(11,43,298,180,rgb(23,41,58));
        txt(24,55,"SHORTCUT SETTINGS",font_cyan);
        txt(24,74,"Edit this file on your phone:",font_main);
        txt(24,91,"/documents/EndraHome/",font_dim);
        txt(24,104,"shortcuts.cfg.tns",font_dim);
        txt(24,126,"0=GBA|/documents/Apps/",font_main);
        txt(24,139,"EndraGBA.tns",font_main);
        txt(24,163,"Unconfigured icons cannot launch.",font_dim);
        txt(24,186,"MENU / ESC / ENTER = back",font_cyan);
        txt(24,202,"No root access or ndless.cfg edits",font_dim);
    }
    SDL_Flip(screen);
}
static int key(void){
    if(isKeyPressed(KEY_NSPIRE_ESC))return 1;
    if(isKeyPressed(KEY_NSPIRE_MENU))return 2;
    if(isKeyPressed(KEY_NSPIRE_ENTER)||isKeyPressed(KEY_NSPIRE_RET)||isKeyPressed(KEY_NSPIRE_CLICK))return 3;
    if(isKeyPressed(KEY_NSPIRE_LEFT)||isKeyPressed(KEY_NSPIRE_UP))return 4;
    if(isKeyPressed(KEY_NSPIRE_RIGHT)||isKeyPressed(KEY_NSPIRE_DOWN))return 5;
    return 0;
}
static void launch(void){
    char path[EH_PATH_CAP];strcpy(path,slots[chosen].path);
    if(!path[0]){
        snprintf(notice,sizeof(notice),"Configure %s in shortcuts.cfg.tns",slots[chosen].name);return;
    }
    FILE *f=fopen(path,"rb");
    if(!f){snprintf(notice,sizeof(notice),"App missing: check configured .tns path");return;}
    fclose(f);
    snprintf(notice,sizeof(notice),"Launching %s",slots[chosen].name);
    paint();wait_no_key_pressed();stop_video();
    char *argv[]={path,NULL};
    int result=nl_exec(path,1,argv);
    if(!start_video()){running=0;return;}
    if(result<0)snprintf(notice,sizeof(notice),"Launch failed; verify app and Ndless");
    else snprintf(notice,sizeof(notice),"Returned from %s",slots[chosen].name);
}
int main(void){
    eh_defaults(slots);eh_load(slots,CFG);
    if(!start_video())return 1;
    paint();int previous=0;
    while(running){
        int k=key();
        if(!k)previous=0;
        if(k&&!previous){
            previous=k;
            if(help){
                if(k<=3)help=0;
            }else if(k==1)running=0;
            else if(k==2)help=1;
            else if(k==3)launch();
            else if(k==4||k==5){
                chosen=(chosen+(k==4?EH_SLOTS-1:1))%EH_SLOTS;
                strcpy(notice,"ENTER launch | MENU help | ESC exit");
            }
            paint();
        }
        msleep(25);
    }
    stop_video();refresh_osscr();return 0;
}
