#include "app.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Draw at the CX's native resolution. UI glyphs use proportional advances,
 * source and console share a seven-pixel monospace cell. */
#define RGB(r,g,b) (((r)>>3)<<11 | ((g)>>2)<<5 | ((b)>>3))
enum {
    BG=RGB(15,20,25), BAR=RGB(21,29,34), CARD=RGB(31,42,47), LINE=RGB(49,64,69),
    WHITE=RGB(244,246,237), MUTED=RGB(171,189,186), FAINT=RGB(120,145,144),
    GREEN=RGB(114,237,167), DEEP=RGB(26,68,53), INK=RGB(13,36,29),
    GOLD=RGB(247,206,126), AQUA=RGB(122,215,238), LILAC=RGB(206,170,248),
    ERROR=RGB(255,155,146), ERROR_BG=RGB(67,41,42)
};
typedef struct {unsigned offset,stride,height;} Font;
enum {SMALL,BODY,TITLE,CODE};
#include "assets/font_layout.h"
extern const unsigned char ep_font_data[];
static uint16_t *fb;

static void box(int x,int y,int w,int h,int c){
    int x0=x<0?0:x, y0=y<0?0:y;
    for(int j=y0;j<y+h && j<240;j++)
        for(int i=x0;i<x+w && i<320;i++)fb[j*320+i]=(uint16_t)c;
}
static void panel(int x,int y,int w,int h,int border,int fill){
    box(x+2,y,w-4,h,border);box(x,y+2,w,h-4,border);
    box(x+2,y+1,w-4,h-2,fill);box(x+1,y+2,w-2,h-4,fill);
}
static int blend(int src,int dst,int a){
    if(a==3)return src;
    int b=3-a;
    return ((((src>>11)*a+(dst>>11)*b)/3)<<11)
        | (((((src>>5)&63)*a+((dst>>5)&63)*b)/3)<<5)
        | (((src&31)*a+(dst&31)*b)/3);
}
static const unsigned char *glyph(int f,int c){
    if(c<32||c>126)c=c=='\t'?'>':'?';
    return ep_font_data+fonts[f].offset+(c-32)*(1+fonts[f].stride*fonts[f].height);
}
static int width(int f,const char *s){
    int w=0;for(;*s;s++)w+=*glyph(f,(unsigned char)*s);return w;
}
static int letter(int x,int y,int f,int color,int ch,int limit){
    const Font *font=&fonts[f];const unsigned char *g=glyph(f,ch);
    int advance=*g++;
    for(unsigned row=0;row<font->height;row++)
        for(unsigned col=0;col<font->stride*4;col++){
            int px=x+(int)col, py=y+(int)row;
            int a=(g[row*font->stride+col/4]>>(2*(col%4)))&3;
            if(a && px>=0 && px<limit && px<320 && py>=0 && py<240){
                uint16_t *p=fb+py*320+px;*p=blend(color,*p,a);
            }
        }
    return advance;
}
static void text(int x,int y,int f,int c,const char *s,int limit){
    while(*s && x<limit)x+=letter(x,y,f,c,(unsigned char)*s++,limit);
}
static void right(int x,int y,int f,int c,const char *s){text(x-width(f,s),y,f,c,s,x);}
static void fitted(int x,int y,int f,int c,const char *s,int end){
    if(width(f,s)<=end-x){text(x,y,f,c,s,end);return;}
    int stop=end-width(f,"...");
    while(*s && x+*glyph(f,*s)<=stop)x+=letter(x,y,f,c,*s++,stop);
    text(x,y,f,c,"...",end);
}
static void mark(int x,int y){
    for(int row=0;row<17;row++){
        int slant=(16-row)/4;
        if(row<4 || (row>=7&&row<=10) || row>=14)
            box(x+slant,y+row,row<4?17:row>=14?13:10,1,GREEN);
        else box(x+slant,y+row,4,1,GREEN);
    }
}
static void page_icon(int x,int y,int c){
    box(x,y,8,1,c);box(x,y,1,13,c);box(x,y+12,11,1,c);
    box(x+10,y+3,1,10,c);box(x+7,y,1,4,c);
    for(int i=0;i<4;i++)box(x+7+i,y+i,1,1,c);
    box(x+3,y+6,5,1,c);box(x+3,y+9,4,1,c);
}
static void terminal_icon(int x,int y,int c){
    for(int i=0;i<5;i++){box(x+i,y+i,2,1,c);box(x+4-i,y+4+i,2,1,c);}
    box(x+8,y+8,6,2,c);
}
static void header(const char *page){
    box(0,0,320,30,BAR);box(0,29,320,1,LINE);mark(9,6);
    text(37,5,TITLE,WHITE,"EndraPython",180);
    int w=width(SMALL,page)+18;panel(311-w,6,w,18,DEEP,DEEP);
    text(320-w,8,SMALL,GREEN,page,309);
}
static int key(int x,const char *k,const char *action){
    int w=width(SMALL,k)+10;panel(x,222,w,15,LINE,CARD);
    text(x+5,223,SMALL,WHITE,k,319);
    text(x+w+4,222,BODY,MUTED,action,319);
    return x+w+width(BODY,action)+14;
}
static void footer(void){box(0,218,320,22,BAR);box(0,218,320,1,LINE);}
static void status(const char *s,int c){
    if(!*s)return;
    box(0,200,320,18,BAR);fitted(9,201,BODY,c,s,311);
}
static void tile(int x,int which,const char *name){
    int selected=app.selected==which;
    int fill=selected?GREEN:CARD, fg=selected?INK:WHITE;
    panel(x,41,147,45,selected?GREEN:LINE,fill);
    if(which==0){box(x+12,55,15,3,fg);box(x+18,49,3,15,fg);}
    else terminal_icon(x+12,52,selected?INK:AQUA);
    text(x+36,53,BODY,fg,name,x+141);
}
static void files(void){
    char count[32];header("FILES");tile(9,0,"New script");tile(164,1,"Python shell");
    box(10,103,13,8,GOLD);box(11,100,6,4,GOLD);
    text(29,95,TITLE,WHITE,"Scripts",155);
    snprintf(count,sizeof count,"%d %s",app.count,app.count==1?"file":"files");right(310,99,BODY,MUTED,count);
    box(10,118,300,1,LINE);
    int first=app.selected>5?app.selected-5:0;
    for(int i=first;i<app.count && i<first+4;i++){
        int y=122+(i-first)*19,sel=app.selected==i+2;
        if(sel){panel(8,y,304,19,GREEN,DEEP);box(10,y+3,2,13,GREEN);}
        page_icon(17,y+3,sel?GREEN:GOLD);
        fitted(38,y+1,BODY,sel?WHITE:MUTED,app.names[i],290);
        if(sel)text(298,y+2,BODY,GREEN,">",311);
    }
    if(!app.count)text(12,133,BODY,MUTED,"Create a script to start editing.",306);
    status(app.status[0]?app.status:"/documents/EndraPython/",app.status[0]?GOLD:FAINT);
    footer();int x=key(8,"Enter",app.selected==0?"Create":"Open");key(x,"Esc","Exit");
    right(311,223,SMALL,MUTED,"RUN 003");
}
static int token(const char *s,int n){
    static const char *const words[]={"and","as","assert","break","class","continue","def","del",
        "elif","else","except","False","finally","for","from","global","if","import","in","is",
        "lambda","None","nonlocal","not","or","pass","raise","return","True","try","while","with","yield"};
    for(unsigned i=0;i<sizeof words/sizeof words[0];i++)
        if((int)strlen(words[i])==n&&!memcmp(s,words[i],n))return LILAC;
    return s[n]=='('?AQUA:WHITE;
}
static void code_line(const Editor *d,int start,int end,int y,int current){
    int quote=0,escaped=0,comment=0,word_end=start,word_color=WHITE;
    for(int p=start;p<end;p++){
        unsigned char c=d->text[p];int color=WHITE;
        if(!quote && c=='#')comment=1;
        if(comment)color=FAINT;
        else if(quote){
            color=GOLD;
            if(!escaped&&c==quote)quote=0;
            escaped=!escaped&&c=='\\';
        }else if(c=='\''||c=='"'){quote=c;color=GOLD;escaped=0;}
        else if(p<word_end)color=word_color;
        else if(isalpha(c)||c=='_'){
            word_end=p+1;
            while(word_end<end && (isalnum((unsigned char)d->text[word_end])||d->text[word_end]=='_'))word_end++;
            color=word_color=token(d->text+p,word_end-p);
        }else if(isdigit(c))color=GREEN;
        int col=p-start-d->left;
        if(col<0||col>=EDIT_COLS)continue;
        int x=32+7*col;
        if(d->selecting && ((p>=d->anchor&&p<d->cursor)||(p>=d->cursor&&p<d->anchor)))
            box(x,y,7,13,DEEP);
        letter(x,y-1,CODE,color,c,316);
    }
    if(current)box(32+7*(d->cursor-start-d->left),y+1,1,12,GREEN);
}
static void editor(void){
    const Editor *d=&app.doc;header("EDITOR");
    page_icon(11,35,GOLD);fitted(31,34,BODY,WHITE,d->name,255);
    if(d->dirty)right(308,36,SMALL,GOLD,"MODIFIED");
    box(0,52,320,1,LINE);box(0,53,27,147,BAR);
    int start=0,row=0;
    while(row<d->top && d->text[start])if(d->text[start++]=='\n')row++;
    for(int i=0;i<EDIT_ROWS;i++,row++){
        int end=start;while(d->text[end]&&d->text[end]!='\n')end++;
        int y=53+i*12,current=d->cursor>=start&&d->cursor<=end;
        if(current){box(0,y,320,12,CARD);box(0,y,2,12,GREEN);}
        char buf[12];snprintf(buf,sizeof buf,"%d",row+1);right(24,y-1,SMALL,current?GREEN:FAINT,buf);
        code_line(d,start,end,y,current);
        if(!d->text[end])break;
        start=end+1;
    }
    char loc[80];snprintf(loc,sizeof loc,"Line %d    Col %d",1+line_number(d->text,d->cursor),1+d->cursor-line_start(d->text,d->cursor));
    status(app.status[0]?app.status:loc,app.status[0]?GOLD:MUTED);
    footer();int x=key(8,"Menu","Tools");x=key(x,"^S","Save");x=key(x,"^R","Run");key(x,"Esc","Files");
}
static void console(void){
    int shell=app.page==SHELL;header(shell?"SHELL":"OUTPUT");
    terminal_icon(10,39,shell?AQUA:GREEN);
    fitted(34,34,BODY,WHITE,shell?"MicroPython 1.24.1":app.doc.name,309);
    box(0,52,320,1,LINE);
    int visible=shell?10:12,last=app.rows-app.offset,first=last>visible?last-visible:0;
    if(shell && app.rows==1 && !app.output[app.first][0])
        text(10,62,CODE,MUTED,"help('modules') lists libraries.",312);
    for(int i=first;i<last;i++){
        const char *line=app.output[(app.first+i)%OUT_LINES];
        int c=!strncmp(line,">>>",3)?AQUA:(!shell&&app.error&&i==last-2)?ERROR:WHITE;
        text(10,55+12*(i-first),CODE,c,line,311);
    }
    if(shell){
        panel(7,185,306,28,LINE,CARD);text(14,190,CODE,GREEN,">>>",39);
        int n=strlen(app.command),from=n>36?n-36:0;text(42,190,CODE,WHITE,app.command+from,310);
        box(42+7*(n-from),193,1,12,GREEN);
    }else{
        box(0,200,320,18,app.error?ERROR_BG:DEEP);
        text(9,201,BODY,app.error?ERROR:GREEN,app.error?"Python exception":"Run complete",308);
    }
    footer();int x=key(8,shell?"Enter":"^R",shell?"Eval":"Run");
    if(shell)x=key(x,"Menu","Modules");
    else if(app.error_line>0)x=key(x,"Enter","Go to line");
    else x=key(x,"Up/Dn","Scroll");
    key(x,"Esc",shell?"Files":"Editor");
}
static void modal(void){
    int x=17,w=286;
    if(app.page==NAME)files();else editor();
    /* Dim the underlying workspace while keeping the current location visible. */
    for(int i=0;i<320*240;i++)fb[i]=blend(BG,fb[i],2);
    int top=app.page==ACTIONS?39:55,bottom=app.page==ACTIONS?212:181;
    panel(x,top,w,bottom-top,LINE,BAR);box(x+2,top+2,3,22,GREEN);
    const char *title=app.page==NAME?"New script":app.page==FIND?"Find in script":app.page==DISCARD?"Unsaved changes":app.symbols?"Insert symbol":"Editor tools";
    text(x+13,top+7,TITLE,WHITE,title,294);box(x+12,top+34,w-24,1,LINE);
    if(app.page==ACTIONS){
        if(app.symbols){
            for(int i=0;ep_symbols[i];i++){
                int cx=x+11+22*(i%12),cy=top+43+23*(i/12);
                if(i==app.action)panel(cx,cy,20,20,GREEN,DEEP);
                letter(cx+6,cy+2,CODE,WHITE,ep_symbols[i],cx+20);
            }
        }else{
            int first=app.action>5?app.action-5:0;
            for(int i=first;ep_actions[i] && i<first+6;i++){
                int y=top+38+(i-first)*20;
                if(i==app.action)panel(x+8,y,w-16,19,GREEN,DEEP);
                text(x+17,y+1,BODY,i==app.action?WHITE:MUTED,ep_actions[i],292);
            }
        }
    }else if(app.page==DISCARD){
        text(32,101,BODY,WHITE,"Save this script before leaving?",291);
        text(32,129,BODY,MUTED,"Esc keeps the editor open.",291);
    }else{
        const char *s=app.page==FIND?app.query:app.name;
        int n=strlen(s),from=n>34?n-34:0;
        panel(29,101,262,26,GREEN,CARD);
        text(37,106,CODE,WHITE,s+from,284);box(37+7*(n-from),108,1,12,GREEN);
        fitted(30,139,BODY,app.status[0]?GOLD:MUTED,app.status[0]?app.status:app.page==FIND?
               "Enter finds the next match and wraps.":"Leave blank for the Hello World example.",292);
    }
    footer();int next=key(8,"Enter",app.page==NAME?"Create":app.page==FIND?"Find":app.page==DISCARD?"Discard":"Choose");
    if(app.page==DISCARD)next=key(next,"^S","Save");
    key(next,"Esc","Back");
}
void render(uint16_t *pixels){
    fb=pixels;box(0,0,320,240,BG);
    if(app.page==FILES)files();
    else if(app.page==EDITOR)editor();
    else if(app.page==SHELL||app.page==OUTPUT)console();
    else modal();
}
