/* Endra SDL Run002: original nSDL browse plus bounded read-only text preview.
 * NO OS root, writes, ZIP, app launch, nested modal loops or old NX UI.
 */
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "safe_paths.h"
#include "preview.h"

#ifdef _TINSPIRE
void __attribute__((weak)) _fini(void) {}
#endif

#define MAX_FILES 64
#define VISIBLE 16
typedef struct { char name[96]; int directory; } Entry;
static Entry entries[MAX_FILES];
static unsigned count,position,scroll,keys_seen;
static int last_key,show_help,running=1,preview_active=0;
static unsigned preview_position;
static EsPreview preview;
static char preview_name[96];
static char folder[ES_PATH_MAX]="/documents";
static char status[72]="SDL restart: read-only";
static SDL_Surface *screen;
static nSDL_Font *font;

static int compare_entries(const void *a,const void *b){
    const Entry *x=(const Entry*)a,*y=(const Entry*)b;
    if(x->directory!=y->directory)return y->directory-x->directory;
    return strcmp(x->name,y->name);
}
static void scan_folder(void){
    DIR *dir;
    struct dirent *it;
    count=0;position=0;scroll=0;
    if(!es_path_allowed(folder)){
        snprintf(status,sizeof(status),"Outside Documents denied");
        return;
    }
    dir=opendir(folder);
    if(!dir){
        snprintf(status,sizeof(status),"Cannot open Documents folder");
        return;
    }
    while(count<MAX_FILES && (it=readdir(dir))){
        Entry item;
        char full[ES_PATH_MAX];
        struct stat st;
        if(!es_name_allowed(it->d_name)||
           !es_path_join(full,sizeof(full),folder,it->d_name))continue;
        if(stat(full,&st)!=0)continue;
        if(!S_ISDIR(st.st_mode)&&!S_ISREG(st.st_mode))continue;
        snprintf(item.name,sizeof(item.name),"%s",it->d_name);
        item.directory=S_ISDIR(st.st_mode)?1:0;
        entries[count++]=item;
    }
    closedir(dir);
    qsort(entries,count,sizeof(entries[0]),compare_entries);
    snprintf(status,sizeof(status),"Listed %u entries; limit %d",count,MAX_FILES);
}
static void bar(int x,int y,int w,int h,Uint8 r,Uint8 g,Uint8 b){
    SDL_Rect rect;
    rect.x=x;rect.y=y;rect.w=w;rect.h=h;
    SDL_FillRect(screen,&rect,SDL_MapRGB(screen->format,r,g,b));
}
static void label(int x,int y,const char *s){
    nSDL_DrawString(screen,font,x,y,"%s",s);
}
static void draw(void){
    int i;
    char line[96];
    bar(0,0,320,240,15,20,31);
    bar(0,0,320,24,28,61,80);
    label(8,7,"ENDRA SDL 002");
    snprintf(line,sizeof(line),"KEY %u / %d",keys_seen,last_key);
    label(205,7,line);
    if(preview_active){
        unsigned i;
        label(8,29,"TEXT PREVIEW (FIRST 2KB)");
        bar(0,45,320,1,78,92,107);
        for(i=0;i<16 && i+preview_position<preview.count;++i)
            label(8,51+(int)i*10,preview.lines[i+preview_position]);
        bar(0,211,320,1,78,92,107);
        snprintf(line,sizeof(line),"%.22s %u/%u%s",preview_name,
                 preview.count?preview_position+1:0,preview.count,
                 preview.truncated?" (LIMIT)":"");
        label(6,215,line);
        bar(0,230,320,10,28,61,80);
        label(6,231,"UP/DOWN page  ESC browser");
    }else if(show_help){
        label(10,38,"READ-ONLY SDL BASELINE");
        label(10,60,"UP/DOWN: SELECT");
        label(10,77,"ENTER: OPEN FOLDER");
        label(10,94,"ESC: PARENT OR EXIT");
        label(10,111,"MENU: THIS HELP");
        label(10,143,"TXT/CFG read-only preview.");
        label(10,160,"No ZIP, writes, app launch.");
        label(10,199,"ESC OR MENU: RETURN");
    }else{
        /* End of path is shown if long; we never render outside viewport. */
        size_t len=strlen(folder);
        const char *tail=len>43?folder+len-43:folder;
        label(6,30,tail);
        bar(0,46,320,1,78,92,107);
        for(i=0;i<VISIBLE&&(unsigned)(i+scroll)<count;++i){
            const Entry *e=&entries[i+scroll];
            int y=50+i*10;
            char name[46];
            if(i+scroll==position)bar(2,y-1,316,10,20,91,123);
            snprintf(name,sizeof(name),"%c %.37s",e->directory?'+':' ',e->name);
            label(7,y,name);
        }
        bar(0,211,320,1,78,92,107);
        label(6,217,status);
        bar(0,230,320,10,28,61,80);
        label(6,231,"ENTER folder   MENU help   ESC back");
    }
    SDL_Flip(screen);
}
static void on_key(SDLKey k){
    ++keys_seen;last_key=(int)k;
    if(preview_active){
        if(k==SDLK_ESCAPE||k==SDLK_RETURN||k==SDLK_KP_ENTER){
            preview_active=0;
        }else if(k==SDLK_DOWN){
            if(preview_position+16<preview.count)preview_position+=16;
        }else if(k==SDLK_UP){
            if(preview_position>=16)preview_position-=16;
            else preview_position=0;
        }
        return;
    }
    if(show_help){
        if(k==SDLK_ESCAPE||k==SDLK_MENU||k==SDLK_RETURN)show_help=0;
        return;
    }
    if(k==SDLK_UP){
        if(position) --position;
        if(position<scroll)scroll=position;
    }else if(k==SDLK_DOWN){
        if(position+1<count)++position;
        if(position>=scroll+VISIBLE)scroll=position-VISIBLE+1;
    }else if(k==SDLK_RETURN||k==SDLK_KP_ENTER||k==SDLK_RIGHT){
        if(count&&entries[position].directory){
            char next[ES_PATH_MAX];
            if(es_path_join(next,sizeof(next),folder,entries[position].name)){
                snprintf(folder,sizeof(folder),"%s",next);
                scan_folder();
            }
        }else if(count && es_preview_supported_name(entries[position].name)){
            char path[ES_PATH_MAX];
            if(es_path_join(path,sizeof(path),folder,entries[position].name) &&
               es_preview_read(path,&preview)){
                preview_active=1;
                preview_position=0;
                snprintf(preview_name,sizeof(preview_name),"%s",
                         entries[position].name);
            }else{
                snprintf(status,sizeof(status),
                         "Preview refused: binary/unreadable");
            }
        }else{
            snprintf(status,sizeof(status),"TXT/CFG only (read-only)");
        }
    }else if(k==SDLK_ESCAPE||k==SDLK_LEFT||k==SDLK_BACKSPACE){
        if(es_path_parent(folder))scan_folder();
        else if(k==SDLK_ESCAPE)running=0;
        else snprintf(status,sizeof(status),"Documents is safe root");
    }else if(k==SDLK_MENU||k==SDLK_TAB){
        show_help=1;
    }
}
int main(void){
    SDL_Event e;
    if(SDL_Init(SDL_INIT_VIDEO)<0)return 1;
    screen=SDL_SetVideoMode(320,240,16,SDL_SWSURFACE);
    if(!screen){SDL_Quit();return 1;}
    font=nSDL_LoadFont(NSDL_FONT_TINYTYPE,245,245,245);
    if(!font){SDL_Quit();return 1;}
    scan_folder();
    draw();
    while(running){
        int redraw=0;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT){running=0;break;}
            if(e.type==SDL_KEYDOWN){on_key(e.key.keysym.sym);redraw=1;}
        }
        if(redraw&&running)draw();
        SDL_Delay(25);
    }
    nSDL_FreeFont(font);
    SDL_Quit();
    return 0;
}
