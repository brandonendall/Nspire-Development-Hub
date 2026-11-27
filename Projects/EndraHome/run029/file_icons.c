#include "file_icons.h"
#include <string.h>

static int extension_is(const char *dot,size_t len,const char *suffix){
    if(strlen(suffix)!=len)return 0;
    for(size_t i=0;i<len;i++){
        char c=dot[i];
        if(c>='A'&&c<='Z')c=(char)(c+'a'-'A');
        if(c!=suffix[i])return 0;
    }
    return 1;
}

EhFileKind eh_file_kind(const char *name,int directory){
    if(directory)return EH_FILE_FOLDER;
    if(!name)return EH_FILE_UNKNOWN;
    const char *dot=strrchr(name,'.');
    if(!dot)return EH_FILE_UNKNOWN;
    size_t len=strlen(dot);
    int ti=extension_is(dot,len,".tns");
    if(ti){
        const char *last=dot;
        while(dot>name&&dot[-1]!='.')--dot;
        if(dot<=name+1)return EH_FILE_TI;
        --dot;
        len=(size_t)(last-dot);
    }
    static const struct { const char *suffix; EhFileKind kind; } types[]={
      {".txt",EH_FILE_TEXT},{".log",EH_FILE_TEXT},{".md",EH_FILE_TEXT},
      {".zip",EH_FILE_ARCHIVE},{".rar",EH_FILE_ARCHIVE},{".7z",EH_FILE_ARCHIVE},
      {".jpg",EH_FILE_IMAGE},{".jpeg",EH_FILE_IMAGE},{".png",EH_FILE_IMAGE},
      {".pdf",EH_FILE_PDF},{".cfg",EH_FILE_CONFIG},{".ini",EH_FILE_CONFIG},
      {".json",EH_FILE_CONFIG},{".theme",EH_FILE_CONFIG},{".csv",EH_FILE_CSV},
      {".gba",EH_FILE_ROM},{".gbz",EH_FILE_ROM},
      {".elf",EH_FILE_PROGRAM},{".zehn",EH_FILE_PROGRAM}
    };
    for(size_t i=0;i<sizeof(types)/sizeof(types[0]);i++)
        if(extension_is(dot,len,types[i].suffix))return types[i].kind;
    return ti?EH_FILE_TI:EH_FILE_UNKNOWN;
}

/* Each 14x13 glyph fits a 15-pixel file row. Dot leaves the wallpaper visible. */
typedef struct { uint16_t fill; char rows[13][15]; } Glyph;
static const Glyph glyphs[EH_FILE_KIND_COUNT-EH_FILE_TEXT]={
    [EH_FILE_TEXT-EH_FILE_TEXT]={EH_RGB(189,221,245),{
        "..ooooooo.....","..o#####oo....","..o#####o+o...","..o#####oooo..",
        "..o########o..","..o#----###o..","..o########o..","..o#------#o..",
        "..o########o..","..o#-----##o..","..o########o..","..oooooooooo..",".............."}},
    [EH_FILE_ARCHIVE-EH_FILE_TEXT]={EH_RGB(238,170,58),{
        "..............","..............",".oooooooooooo.",".o++++-+++++o.",
        ".o####--####o.",".o####-#####o.",".o####--####o.",".o####-#####o.",
        ".o####--####o.",".o####++####o.",".o##########o.",".oooooooooooo.",".............."}},
    [EH_FILE_IMAGE-EH_FILE_TEXT]={EH_RGB(74,200,242),{
        "..............","oooooooooooooo","o############o","o########++##o",
        "o########++##o","o##+#########o","o#+++##+#####o","o++-+++-+####o",
        "o+---+---+###o","o---------++#o","o------------o","oooooooooooooo",".............."}},
    [EH_FILE_PDF-EH_FILE_TEXT]={EH_RGB(232,62,79),{
        "...oooooo.....","...o####oo....","...o#####o+o..","...o######oo..",
        "...o#######o..","...o##++###o..","...o#+##+##o..","...o#+##+##o..",
        "..oo++##++oo..","..o++++++++o..","..o+######+o..","..oooooooooo..",".............."}},
    [EH_FILE_CONFIG-EH_FILE_TEXT]={EH_RGB(179,142,244),{
        ".....oooo.....",".....o++o.....","..ooo++++ooo..","..o++++++++o..",
        ".oo++----++oo.","oo+++o..o+++oo","o#+++o..o+++#o","oo+++o..o+++oo",
        ".oo++----++oo.","..o++++++++o..","..ooo++++ooo..",".....o++o.....",".....oooo....."}},
    [EH_FILE_CSV-EH_FILE_TEXT]={EH_RGB(78,209,128),{
        "..............",".oooooooooooo.",".o++++++++++o.",".o++++++++++o.",
        ".o#-##-##-##o.",".o----------o.",".o#-##-##-##o.",".o#-##-##-##o.",
        ".o----------o.",".o#-##-##-##o.",".o#-##-##-##o.",".oooooooooooo.",".............."}},
    [EH_FILE_ROM-EH_FILE_TEXT]={EH_RGB(247,151,73),{
        "...oooooooo...","...o++++++o...","...o++++++o...","..oo######oo..",
        ".oo########oo.",".o##------##o.",".o##-++++-##o.",".o##------##o.",
        ".o##########o.",".o#++++++++#o.",".oooooooooooo.","..o-+-++-+-o..","..oooooooooo.."}},
    [EH_FILE_PROGRAM-EH_FILE_TEXT]={EH_RGB(90,205,255),{
        "......oo......","....oo++oo....","..oo++++++oo..",".o+########+o.",
        ".o##++++++##o.",".o##+#######o.",".o##+++++###o.",".o##+#######o.",
        ".o##++++++##o.",".o+########+o.","..oo++++++oo..","....oo++oo....","......oo......"}},
    [EH_FILE_TI-EH_FILE_TEXT]={EH_RGB(111,173,248),{
        "....oooooo....","....o####oo...","....o#####oo..","....o#######o.",
        "....o#######o.","..ooooooo###o.","..o+++++o###o.","..o+----o###o.",
        "..o+----o###o.","..o+----o##oo.","..o+++++o#oo..","..ooooooooo...",".............."}},
    [EH_FILE_UNKNOWN-EH_FILE_TEXT]={EH_RGB(165,177,192),{
        ".....oooo.....","...oo++++oo...","..o+######+o..",".o+##+++###+o.",
        ".o##+###+###o.",".o######+###o.",".o#####++###o.",".o#####++###o.",
        ".o##########o.",".o+####+###+o.","..o+######+o..","...oo++++oo...",".....oooo....."}}
};

void eh_file_kind_icon(uint16_t *f,int x,int y,EhFileKind kind,const EhPalette *p){
    if(kind==EH_FILE_FOLDER){eh_folder_icon(f,x,y,1,p);return;}
    if((unsigned)kind>=EH_FILE_KIND_COUNT)kind=EH_FILE_UNKNOWN;
    const Glyph *g=glyphs+kind-EH_FILE_TEXT;
    const uint16_t colors[]={EH_RGB(19,26,39),g->fill,EH_RGB(247,250,254),EH_RGB(40,57,77)};
    for(int row=0;row<13;row++)
        for(int col=0;col<14;col++){
            char c=g->rows[row][col];
            if(c!='.')eh_box(f,x+col+1,y+row+1,1,1,colors[c=='o'?0:c=='#'?1:c=='+'?2:3]);
        }
}
