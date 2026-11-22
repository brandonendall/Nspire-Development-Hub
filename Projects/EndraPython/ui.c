#include "app.h"
#include "runtime.h"
#include "../UltimateCommander/vendor/font8x8_basic.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

App app;
static const char hello[]="print('Hello from EndraPython')\nprint(sum(range(10)))\n";
static const char symbols[]="()[]{}:='\042_+-*/%<>!#&|^~@;\\";
static const char *actions[]={"Save script","Save and run","Insert symbol","Back to files"};
static void reset_output(void) {
    memset(app.output,0,sizeof app.output);
    app.rows=1;app.first=app.col=app.offset=0;
}
static void new_output_line(void) {
    int slot=(app.first+app.rows)%OUT_LINES;
    if(app.rows==OUT_LINES)app.first=(app.first+1)%OUT_LINES;
    else app.rows++;
    app.output[slot][0]=0;app.col=0;
}
void console_write(const char *s,size_t n) {
    for(size_t i=0;i<n;i++){
        unsigned char c=s[i];
        if(c=='\r')continue;
        if(c=='\n'){new_output_line();continue;}
        if(app.col==OUT_WIDTH)new_output_line();
        char *line=app.output[(app.first+app.rows-1)%OUT_LINES];
        line[app.col++]=(c>=32 && c<127)?c:'?';line[app.col]=0;
    }
}
void app_init(const char *root,void *stack) {
    memset(&app,0,sizeof app);
    app.root=root;app.stack=stack;files_refresh();
}
static void show_files(void){app.page=FILES;files_refresh();}
static void leave_doc(void){if(app.doc.dirty)app.page=DISCARD;else show_files();}
static void run(void){
    if((app.doc.dirty||!app.doc.existing)&&!save_script())return;
    app.status[0]=0;reset_output();runtime_start(app.stack);
    app.error=!runtime_exec(app.doc.name,app.doc.text,0);
    runtime_end();app.page=OUTPUT;
}
static void text_field(char *s,size_t cap,int k){
    size_t n=strlen(s);
    if(k==8&&n)s[n-1]=0;
    else if(k>=32&&k<127&&n+1<cap){s[n]=(char)k;s[n+1]=0;}
}
static void begin_name(void){app.page=NAME;app.name[0]=0;}
static void start_shell(void){
    reset_output();app.command[0]=app.history[0]=0;
    runtime_start(app.stack);app.page=SHELL;
}
static void create_doc(void){
    char name[NAME_CAP];
    const char *base=app.name[0]?app.name:"Hello_World";
    int n=snprintf(name,sizeof name,"%s%s",base,valid_script(base)?"":".py.tns");
    if(n>=(int)sizeof name||!valid_script(name)){strcpy(app.status,"Choose a shorter plain filename");return;}
    memset(&app.doc,0,sizeof app.doc);strcpy(app.doc.name,name);
    if(!app.name[0])strcpy(app.doc.text,hello);
    app.doc.dirty=1;
    if(save_script())app.page=EDITOR;
}
void app_key(int k) {
    if(!k)return;
    app.status[0]=0;
    if(app.page==FILES){
        int last=app.count+1;
        if(k==UP && app.selected)app.selected--;
        else if(k==DOWN && app.selected<last)app.selected++;
        else if(k==ESC)app.quit=1;
        else if(k==ENTER){
            if(app.selected==0)begin_name();
            else if(app.selected==1)start_shell();
            else if(load_script(app.names[app.selected-2]))app.page=EDITOR;
        }
    }else if(app.page==NAME){
        if(k==ESC)show_files();
        else if(k==ENTER)create_doc();
        else text_field(app.name,sizeof app.name,k);
    }else if(app.page==EDITOR){
        if(k==ESC)leave_doc();
        else if(k==MENU){app.page=ACTIONS;app.action=app.symbols=0;}
        else if(k==SAVE)save_script();
        else if(k==RUN)run();
        else edit_key(&app.doc,k);
    }else if(app.page==DISCARD){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER)show_files();
        else if(k==SAVE && save_script())show_files();
    }else if(app.page==ACTIONS){
        int count=app.symbols?(int)strlen(symbols):4;
        if(k==ESC)app.page=EDITOR;
        else if(k==DOWN||k==RIGHT)app.action=(app.action+1)%count;
        else if(k==UP||k==LEFT)app.action=(app.action+count-1)%count;
        else if(k==ENTER){
            app.page=EDITOR;
            if(app.symbols)edit_key(&app.doc,symbols[app.action]);
            else if(app.action==0)save_script();
            else if(app.action==1)run();
            else if(app.action==2){app.page=ACTIONS;app.symbols=1;app.action=0;}
            else leave_doc();
        }
    }else if(app.page==OUTPUT){
        if(k==ESC)app.page=EDITOR;
        else if(k==RUN)run();
        else if(k==UP && app.offset+13<app.rows)app.offset++;
        else if(k==DOWN && app.offset)app.offset--;
    }else if(app.page==SHELL){
        if(k==ESC){runtime_end();show_files();}
        else if(k==UP)strcpy(app.command,app.history);
        else if(k==ENTER && app.command[0]){
            console_write(">>> ",4);console_write(app.command,strlen(app.command));console_write("\n",1);
            strcpy(app.history,app.command);runtime_exec("<shell>",app.command,1);app.command[0]=0;
        }else text_field(app.command,sizeof app.command,k);
    }
}

/* One RGB565 renderer serves both the Ndless surface and host previews. */
enum {BG=0x10e4,BAR=0x1926,PANEL=0x21a8,WHITE=0xf79e,MUTED=0x9d54,ACCENT=0x4e74,
      SELECT=0x1aeb,GOLD=0xf5ae,RED=0xfb4d};
static uint16_t *fb;
static void rect(int x,int y,int w,int h,int color){
    for(int j=y;j<y+h && j<240;j++)for(int i=x;i<x+w && i<320;i++)
        if(i>=0 && j>=0)fb[j*320+i]=color;
}
static void label(int x,int y,int color,const char *s,int max){
    for(int i=0;s[i] && i<max && x+8<=320;i++,x+=8){
        unsigned c=(unsigned char)s[i];
        if(c=='\t')c='>';
        if(c<32||c>=127)c='?';
        for(int row=0;row<8;row++)for(int col=0;col<8;col++)
            if(font8x8_basic[c][row]&(1<<col))fb[(y+row)*320+x+col]=color;
    }
}
static void item(int y,const char *s,int selected){
    if(selected){rect(4,y-3,312,15,SELECT);rect(4,y-3,2,15,ACCENT);}
    label(12,y,selected?WHITE:MUTED,s,37);
}
static void editor(void){
    Editor *d=&app.doc;char number[12];int start=0,row=0;
    rect(0,43,34,156,BAR);
    while(row<d->top && d->text[start]){if(d->text[start++]=='\n')row++;}
    for(int i=0;i<13;i++,row++){
        int end=start;while(d->text[end] && d->text[end]!='\n')end++;
        int y=47+i*12;
        snprintf(number,sizeof number,"%4d",row+1);label(0,y,MUTED,number,4);
        int comment=0,quote=0;
        for(int p=start;p<end;p++){
            char c=d->text[p];if(c=='#'&&!quote)comment=1;
            int delimiter=c=='\''||c=='"';
            int color=comment?MUTED:quote||delimiter?GOLD:isdigit((unsigned char)c)?ACCENT:WHITE;
            int col=p-start-d->left;
            if(col>=0 && col<34){char glyph[2]={c,0};label(39+col*8,y,color,glyph,1);}
            if(delimiter && !comment && (p==start||d->text[p-1]!='\\')){
                if(!quote)quote=c;else if(quote==c)quote=0;
            }
        }
        if(d->cursor>=start && d->cursor<=end)
            rect(38+(d->cursor-start-d->left)*8,y+8,7,2,ACCENT);
        if(!d->text[end])break;
        start=end+1;
    }
}
void render(uint16_t *pixels){
    fb=pixels;
    static const char *pages[]={"Files","Editor","Console","REPL","New script","Menu","Unsaved"};
    rect(0,0,320,240,BG);rect(0,0,320,22,BAR);rect(0,20,320,2,ACCENT);
    label(8,6,WHITE,"EndraPython",12);
    int len=strlen(pages[app.page]);label(312-len*8,6,ACCENT,pages[app.page],12);
    const char *sub="",*hint="";char detail[84]="";
    if(app.page==FILES){
        sub="Scripts";hint="Enter  Open                 Esc  Exit";
        int first=app.selected>8?app.selected-8:0;
        for(int i=first;i<app.count+2 && i<first+9;i++)
            item(48+(i-first)*17,i==0?"+ New script":i==1?"> Python shell":app.names[i-2],i==app.selected);
        snprintf(detail,sizeof detail,"%d saved script%s",app.count,app.count==1?"":"s");
    }else if(app.page==EDITOR){
        sub=app.doc.name;hint="Menu   Ctrl+S Save   Ctrl+R Run   Esc";
        editor();
        snprintf(detail,sizeof detail,"Ln %d   Col %d   %s",1+line_number(app.doc.text,app.doc.cursor),
            1+app.doc.cursor-line_start(app.doc.text,app.doc.cursor),app.doc.dirty?"Modified":"Saved");
    }else if(app.page==SHELL||app.page==OUTPUT){
        int sh=app.page==SHELL;
        sub=sh?"MicroPython 1.24.1":app.doc.name;
        int visible=sh?12:13, last=app.rows-app.offset,first=last>visible?last-visible:0;
        for(int i=first;i<last;i++)label(10,47+12*(i-first),WHITE,app.output[(app.first+i)%OUT_LINES],OUT_WIDTH);
        if(sh){
            rect(4,193,312,18,PANEL);label(9,198,ACCENT,">>>",3);
            size_t n=strlen(app.command);label(40,198,WHITE,app.command+(n>33?n-33:0),33);
            hint="Enter Eval    Up History    Esc Files";
        }else{
            strcpy(detail,app.error?"Python exception":"Run complete");hint="Up/Dn Scroll   Ctrl+R Run   Esc Edit";
        }
    }else if(app.page==NAME){
        sub="Script name";label(9,57,MUTED,"Blank creates the Hello World example.",38);
        rect(6,82,308,22,PANEL);
        size_t n=strlen(app.name);label(10,89,WHITE,app.name+(n>36?n-36:0),36);
        hint="Enter Create                Esc Back";
    }else if(app.page==ACTIONS){
        sub=app.symbols?"Insert symbol":"Editor actions";hint="Arrows Select   Enter Apply   Esc Back";
        if(!app.symbols){for(int i=0;i<4;i++)item(54+20*i,actions[i],i==app.action);}
        else for(int i=0;symbols[i];i++){
            int x=10+24*(i%12),y=58+24*(i/12);
            if(i==app.action)rect(x-3,y-3,16,15,SELECT);
            label(x,y,WHITE,symbols+i,1);
        }
    }else{
        sub="Leave editor?";label(10,61,WHITE,"This script has unsaved changes.",37);
        label(10,87,MUTED,"Ctrl+S saves before leaving.",37);
        hint="Enter Discard              Esc Keep";
    }
    label(8,30,WHITE,sub,38);
    label(8,207,app.status[0]?GOLD:app.page==OUTPUT&&app.error?RED:MUTED,app.status[0]?app.status:detail,38);
    rect(0,222,320,18,BAR);label(6,228,MUTED,hint,38);
}
