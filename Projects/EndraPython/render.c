#include "app.h"
#include "device.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Draw at the CX's native resolution. UI glyphs use proportional advances,
 * source and console share a seven-pixel monospace cell. */
#define RGB(r,g,b) (((r)>>3)<<11 | ((g)>>2)<<5 | ((b)>>3))
enum {
    BG=RGB(16,17,20), BAR=RGB(24,26,31), CARD=RGB(33,38,48), LINE=RGB(48,54,65),
    WHITE=RGB(234,238,245), MUTED=RGB(169,181,199), FAINT=RGB(135,148,169),
    ACCENT=RGB(142,175,227), DEEP=RGB(35,48,71),
    GOLD=RGB(247,206,126), AQUA=RGB(160,197,241), LILAC=RGB(206,170,248),
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
    int index=c-32;
    if(c<32)index=(c=='\t'?'>':'?')-32;
    if(c>126){
        int base=95;index='?'-32;
        if(f==CODE)for(unsigned i=0;i<sizeof code_ranges/sizeof code_ranges[0];i++){
            int lo=code_ranges[i][0],hi=code_ranges[i][1];
            if(c>=lo && c<=hi){index=base+c-lo;break;}
            base+=hi-lo+1;
        }
    }
    return ep_font_data+fonts[f].offset+index*(1+fonts[f].stride*fonts[f].height);
}
static int width(int f,const char *s){
    int w=0;while(*s){int n,c=utf_decode(s,&n);w+=*glyph(c>126?CODE:f,c);s+=n;}return w;
}
static int glyph_alpha(const Font *font,const unsigned char *bitmap,unsigned row,unsigned col){
    return (bitmap[row*font->stride+col/4]>>(2*(col%4)))&3;
}
static int letter(int x,int y,int f,int color,int ch,int limit){
    if(ch>126)f=CODE;
    const Font *font=&fonts[f];const unsigned char *g=glyph(f,ch);
    int advance=*g++;
    for(unsigned row=0;row<font->height;row++)
        for(unsigned col=0;col<font->stride*4;col++){
            int px=x+(int)col, py=y+(int)row;
            int a=glyph_alpha(font,g,row,col);
            if(a && px>=0 && px<limit && px<320 && py>=0 && py<240){
                uint16_t *p=fb+py*320+px;*p=blend(color,*p,a);
            }
        }
    return advance;
}
static void text(int x,int y,int f,int c,const char *s,int limit){
    while(*s && x<limit){int n,ch=utf_decode(s,&n);x+=letter(x,y,f,c,ch,limit);s+=n;}
}
static void right(int x,int y,int f,int c,const char *s){text(x-width(f,s),y,f,c,s,x);}
static void fitted(int x,int y,int f,int c,const char *s,int end){
    if(width(f,s)<=end-x){text(x,y,f,c,s,end);return;}
    int stop=end-width(f,"...");
    while(*s){int n,ch=utf_decode(s,&n);if(x+*glyph(ch>126?CODE:f,ch)>stop)break;x+=letter(x,y,f,c,ch,stop);s+=n;}
    text(x,y,f,c,"...",end);
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
    box(0,0,320,30,BAR);box(0,29,320,1,LINE);
    panel(9,7,16,16,DEEP,DEEP);
    text(13,7,SMALL,ACCENT,">",24);
    text(31,5,TITLE,WHITE,"EndraPython",190);
    right(310,8,SMALL,MUTED,page);
}
static int key(int x,const char *k,const char *action){
    int w=width(SMALL,k)+10;panel(x,222,w,15,LINE,CARD);
    text(x+5,223,SMALL,WHITE,k,319);
    text(x+w+4,222,BODY,MUTED,action,319);
    return x+w+width(BODY,action)+14;
}
static void footer(void){box(0,218,320,22,BAR);box(0,218,320,1,LINE);}
static void workspace_footer(void){footer();key(8,"Menu","Commands & help");}
static void status(const char *s,int c){
    if(!*s)return;
    box(0,200,320,18,BAR);fitted(9,201,BODY,c,s,311);
}
static void files(void){
    char location[PATH_CAP+16], count[32];
    header("FILES");
    snprintf(location,sizeof location,"Documents%s",app.root+strlen(app.documents));
    fitted(10,35,BODY,WHITE,location,310);
    box(0,55,320,19,CARD);
    text(13,56,SMALL,MUTED,"Name",231);
    text(258,56,SMALL,MUTED,"Type",307);
    int first=app.selected>5?app.selected-5:0;
    for(int i=first;i<app.count && i<first+6;i++){
        int y=76+(i-first)*20,sel=app.selected==i;
        const struct FileEntry *entry=&app.entries[i];
        int py=valid_script(entry->name);
        if(sel){panel(7,y,306,19,ACCENT,DEEP);box(8,y+3,2,13,ACCENT);}
        else box(11,y+19,297,1,LINE);
        if(entry->folder){box(15,y+7,13,8,GOLD);box(16,y+4,6,4,GOLD);}
        else page_icon(16,y+3,py?AQUA:MUTED);
        fitted(37,y+1,BODY,sel?WHITE:MUTED,entry->name,247);
        text(258,y+2,SMALL,sel?ACCENT:FAINT,entry->folder?"Folder":py?"Python":"Text",306);
    }
    if(!app.count)text(12,90,BODY,MUTED,"This folder has no scripts or text files.",307);
    if(app.file_page || app.more_files)snprintf(count,sizeof count,"Page %d / Tab",1+app.file_page);
    else snprintf(count,sizeof count,"%d %s",app.count,app.count==1?"item":"items");
    status(app.status[0]?app.status:count,app.status[0]?GOLD:MUTED);
    footer();
    int x=8;
    if(app.count){
        const struct FileEntry *entry=&app.entries[app.selected];
        int python=!entry->folder && valid_script(entry->name);
        x=key(x,"Enter",python?"Run":"Open");
        if(python)x=key(x,"Right","Edit");
    }
    key(x,"Menu","Actions");
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
    int indent=0;
    while(start+indent<end && d->text[start+indent]==' ')indent++;
    for(int col=2;app.indent_guides && col<=indent;col+=2)
        if(col>=d->left && col-d->left<EDIT_COLS)
            box(32+7*(col-d->left)-3,y,1,12,LINE);
    int step=1;
    for(int p=start,col=-d->left;p<end;p+=step,col++){
        int c=utf_decode(d->text+p,&step),color=WHITE;
        if(!quote && c=='#')comment=1;
        if(comment)color=FAINT;
        else if(quote){
            color=GOLD;
            if(!escaped&&c==quote)quote=0;
            escaped=!escaped&&c=='\\';
        }else if(c=='\''||c=='"'){quote=c;color=GOLD;escaped=0;}
        else if(p<word_end)color=word_color;
        else if((c>=0 && c<128 && isalpha(c))||c=='_'){
            word_end=p+1;
            while(word_end<end && (isalnum((unsigned char)d->text[word_end])||d->text[word_end]=='_'))word_end++;
            color=word_color=token(d->text+p,word_end-p);
        }else if(c>=0 && c<128 && isdigit(c))color=ACCENT;
        if(col<0||col>=EDIT_COLS)continue;
        int x=32+7*col;
        if(d->selecting && ((p>=d->anchor&&p<d->cursor)||(p>=d->cursor&&p<d->anchor)))
            box(x,y,7,13,DEEP);
        letter(x,y-1,CODE,color,c,316);
    }
    if(current)box(32+7*(utf_columns(d->text+start,d->cursor-start)-d->left),y+1,1,12,ACCENT);
}
static void editor(void){
    const Editor *d=&app.doc;
    int python=valid_script(d->name);
    header("EDITOR");
    box(0,30,320,22,CARD);

    page_icon(11,35,python?AQUA:MUTED);
    fitted(31,34,BODY,WHITE,d->name,239);
    if(d->dirty)box(245,39,4,4,ACCENT);
    right(308,36,SMALL,MUTED,python?"Python":"Text");
    box(0,52,320,1,LINE);
    int start=0,row=0;
    while(row<d->top && d->text[start])if(d->text[start++]=='\n')row++;
    for(int i=0;i<EDIT_ROWS;i++,row++){
        int end=start;while(d->text[end]&&d->text[end]!='\n')end++;
        int y=53+i*12,current=d->cursor>=start&&d->cursor<=end;
        if(current){box(0,y,320,12,CARD);box(0,y,2,12,ACCENT);}
        if(app.line_numbers){
            char buf[12];snprintf(buf,sizeof buf,"%d",row+1);right(24,y-1,SMALL,current?ACCENT:FAINT,buf);
        }
        code_line(d,start,end,y,current);
        if(!d->text[end])break;
        start=end+1;
    }
    char loc[80];snprintf(loc,sizeof loc,"Ln %d, Col %d",1+line_number(d->text,d->cursor),1+utf_columns(d->text+line_start(d->text,d->cursor),d->cursor-line_start(d->text,d->cursor)));
    status(app.status[0]?app.status:loc,app.status[0]?GOLD:MUTED);
    if(!app.status[0] && python)right(307,202,SMALL,FAINT,"2 spaces");
    footer();
    int x=key(8,"Menu","Tools");
    if(python)x=key(x,"Ctrl+R","Run");
    key(x,"Ctrl+S","Save");
}
static void console(void){
    int input=app.page==READ_INPUT || (app.page==ACTIONS && app.origin==READ_INPUT);
    int shell=input || app.page==SHELL || (app.page==ACTIONS && app.origin==SHELL);
    header(input?"INPUT":shell?"SHELL":"OUTPUT");
    terminal_icon(10,39,shell?AQUA:ACCENT);
    fitted(34,34,BODY,WHITE,input?"Python input":shell?"MicroPython 1.24.1":app.doc.name,309);
    box(0,52,320,1,LINE);
    int visible=shell?10:12,last=app.rows-app.offset,first=last>visible?last-visible:0;
    if(shell && !input && app.rows==1 && !app.output[app.first][0]){
        text(10,62,CODE,MUTED,"Menu > Command catalog",312);
        text(10,77,CODE,MUTED,":help for terminal commands",312);
    }
    for(int i=first;i<last;i++){
        const char *line=app.output[(app.first+i)%OUT_LINES];
        int c=!strncmp(line,">>>",3)?AQUA:(!shell&&app.error&&i==last-2)?ERROR:WHITE;
        text(10,55+12*(i-first),CODE,c,line,311);
    }
    if(shell){
        if(app.status[0])fitted(9,168,BODY,GOLD,app.status,310);
        panel(7,185,306,28,LINE,CARD);text(14,190,CODE,ACCENT,">>>",39);
        const char *field=input?app.reply:app.command;
        int n=strlen(field),cols=utf_columns(field,n),from=utf_advance(field,0,n,cols-36);text(42,190,CODE,WHITE,field+from,310);
        box(42+7*utf_columns(field+from,n-from),193,1,12,ACCENT);
    }else{
        box(0,200,320,18,app.error?ERROR_BG:DEEP);
        text(9,201,BODY,app.error?ERROR:ACCENT,app.error?"Python exception":"Run complete",308);
    }
    if(input){footer();int x=key(8,"Enter","Submit");key(x,"Esc","Cancel");}
    else workspace_footer();
}
static void modal(void){
    int x=17,w=286;
    Page behind=app.page==ACTIONS?app.origin:app.page;
    if(behind==NAME || behind==FILES)files();
    else if(behind==SHELL || behind==OUTPUT || behind==READ_INPUT)console();else editor();
    /* Dim the underlying workspace while keeping the current location visible. */
    for(int i=0;i<320*240;i++)fb[i]=blend(BG,fb[i],2);
    int list=app.page==ACTIONS;
    int top=list?39:55,bottom=list?212:181;
    panel(x,top,w,bottom-top,LINE,BAR);box(x+2,top+2,3,22,ACCENT);
    const char *title=app.page==NAME?(app.new_text?"New text file":"New Python script"):app.page==FIND?"Find in script":app.page==DISCARD?"Unsaved changes":
        app.page==GOTO_LINE?"Go to line":app.page==REPLACE_FIND?"Replace all":
        app.page==REPLACE_WITH?"Replacement text":app.symbols?"Marks and symbols":"Menu";
    text(x+13,top+7,TITLE,WHITE,title,294);box(x+12,top+34,w-24,1,LINE);
    if(list){
        if(app.page==ACTIONS && app.symbols){
            for(int i=0;i<ep_symbol_count;i++){
                int cx=x+11+22*(i%12),cy=top+43+23*(i/12);
                if(i==app.action)panel(cx,cy,20,20,ACCENT,DEEP);
                text(cx+6,cy+2,CODE,WHITE,ep_symbols[i],cx+20);
            }
            text(31,161,BODY,MUTED,"Menu also offers TI symbols",290);
        }else{
            const Command *commands=page_commands(app.origin);
            const int rows=7;
            int first=app.action>=rows?app.action-rows+1:0;
            for(int i=first;i<first+rows && commands[i].label;i++){
                int y=top+38+(i-first)*19;
                if(i==app.action)panel(x+12,y,w-24,18,LINE,DEEP);
                fitted(x+20,y,BODY,i==app.action?WHITE:MUTED,commands[i].label,x+w-18);
            }
        }
    }else if(app.page==DISCARD){
        text(32,101,BODY,WHITE,"Save this script before leaving?",291);
        text(32,123,BODY,MUTED,"Ctrl+S saves. Enter discards.",291);
        text(32,145,BODY,MUTED,"Esc keeps editing.",291);
    }else{
        const char *s=app.page==FIND || app.page==REPLACE_FIND ? app.query :
            app.page==REPLACE_WITH ? app.replacement : app.name;
        const char *hint=app.page==FIND?"Enter finds the next match and wraps.":
            app.page==GOTO_LINE?"Enter a line number.":
            app.page==REPLACE_FIND?"Find all exact matches.":
            app.page==REPLACE_WITH?"Enter replaces all. Blank deletes.":
            "Enter a name to create a blank file.";
        int n=strlen(s),from=utf_advance(s,0,n,utf_columns(s,n)-34);
        panel(29,101,262,26,ACCENT,CARD);
        text(37,106,CODE,WHITE,s+from,284);box(37+7*utf_columns(s+from,n-from),108,1,12,ACCENT);
        fitted(30,139,BODY,app.status[0]?GOLD:MUTED,app.status[0]?app.status:hint,292);
    }
    footer();
    if(app.page==DISCARD)key(8,"Esc","Keep editing");
    else {int x=key(8,"Enter",app.page==NAME?"Create":app.page==FIND?"Find":"Choose");key(x,"Esc","Back");}
}
typedef struct {const char *key,*action;} HelpRow;
typedef struct {const char *title;HelpRow rows[7];} HelpTopic;
static const HelpTopic topics[HELP_COUNT]={
    [HELP_FILES]={"Files",{{"Menu","New file, shell or browse"},
        {"Enter","Run script or open item"},{"Right on script","Open script for editing"},{"Menu > Browse","Browse Documents"},
        {"Tab","Next page in a large folder"},{"Esc","Parent folder or exit"}}},
    [HELP_EDITOR]={"Editing and running",{{"Ctrl+R","Save and run this script"},
        {"Ctrl+S","Save changes"},{"Ctrl+F","Find text or find next"},
        {"? key","Insert a symbol"},{"Menu","Replace, go to line, outline"},
        {"Enter after :","Indent Python block"},{"Esc","Return to Files"}}},
    [HELP_EDITS]={"Selection and clipboard",{{"Shift + arrows","Extend selection"},
        {"Ctrl+A","Select all"},{"Ctrl+C","Copy"},{"Ctrl+X","Cut"},{"Ctrl+V","Paste"},
        {"Ctrl+Z","Undo"},{"Ctrl+Y","Redo"}}},
    [HELP_SHELL]={"Python shell",{{"Enter","Run the command"},{"Up","Recall last command"},
        {"Left / Right","Scroll earlier / later output"},{":help","List terminal commands"},
        {"? key","Insert a symbol"},{"Menu > Catalog","Browse Python modules"},{"Esc","Return to Files"}}},
    [HELP_OUTPUT]={"Script output",{{"Ctrl+R","Run the script again"},{"Up / Down","Scroll output"},
        {"Enter on error","Go to the failing line"},{"Hold Esc during run","Interrupt Python"},
        {"Esc","Return to editor"}}}
};
static void guide(void){
    const HelpTopic *topic=&topics[app.help_topic];
    header("HELP");text(10,38,TITLE,WHITE,topic->title,267);
    char pos[12];snprintf(pos,sizeof pos,"%d / %d",app.help_topic+1,HELP_COUNT);
    right(309,42,SMALL,MUTED,pos);box(10,63,300,1,LINE);
    for(int i=0;i<7 && topic->rows[i].key;i++){
        int y=71+i*20;
        text(10,y,SMALL,ACCENT,topic->rows[i].key,145);
        text(148,y-1,BODY,WHITE,topic->rows[i].action,314);
    }
    footer();int x=key(8,"Left / Right","Topics");key(x,"Esc","Back");
}
static void settings(void){
    static const char *const labels[]={"Auto-indent","Indent size","Indent guides","Line numbers"};
    const char *values[]={app.auto_indent?"On":"Off","2 spaces",app.indent_guides?"On":"Off",app.line_numbers?"On":"Off"};
    static const int options[]={0,-1,1,2};
    header("Settings");
    text(11,38,TITLE,WHITE,"Editor",300);
    for(int i=0;i<4;i++){
        int y=67+25*i, selected=options[i]==app.action;
        if(selected){box(9,y,302,23,DEEP);box(9,y,2,23,ACCENT);}
        text(17,y+3,BODY,options[i]<0?FAINT:WHITE,labels[i],210);
        right(302,y+4,SMALL,options[i]<0?FAINT:MUTED,values[i]);
        box(10,y+23,300,1,LINE);
    }
    footer();int x=key(8,"Enter","Change");key(x,"Esc","Back");
}
static void command_catalog(void){
    int modules=catalog_browsing_modules;
    header("Catalog");
    fitted(10,35,TITLE,WHITE,modules?"Python modules":catalog_groups[catalog_group],225);
    right(309,39,SMALL,MUTED,modules?"Enter to open":"Left to modules");
    box(10,60,300,1,LINE);
    int current=modules?catalog_group:catalog_index;
    int count=modules?catalog_group_count:catalog_count;
    int rows=modules?7:5, first=current>=rows?current-rows+1:0;
    for(int i=first;i<count && i<first+rows;i++){
        int y=64+19*(i-first),selected=i==current;
        if(selected){box(8,y,304,18,DEEP);box(8,y,2,18,ACCENT);}
        fitted(16,y+1,BODY,selected?WHITE:MUTED,modules?catalog_groups[i]:catalog_entries[i].name,299);
        if(modules)right(301,y+2,SMALL,FAINT,">");
    }
    if(!modules && count){
        char name[80],detail[96];catalog_name(name,sizeof name);
        const char *signature=catalog_signature();
        if(signature)snprintf(detail,sizeof detail,"%s",signature);
        else snprintf(detail,sizeof detail,"%s%s",name,catalog_entries[catalog_index].callable?"(...)":"");
        int end=utf_advance(detail,0,strlen(detail),42);
        if(detail[end]){
            text(10,181,CODE,AQUA,detail+end,311);
            detail[end]=0;
        }
        text(10,167,CODE,AQUA,detail,311);
    }else if(!count)text(10,68,BODY,MUTED,"No public commands",311);
    status(app.status[0]?app.status:catalog_filter[0]?catalog_filter:"Type a name to jump",MUTED);
    footer();int x=key(8,"Enter",modules?"Open":"Insert");
    if(!modules && catalog_group && strcmp(catalog_groups[catalog_group],"builtins"))x=key(x,"Tab","Import");
    key(x,"Esc","Back");
}
static void outline(void){
    header("OUTLINE");
    fitted(10,36,BODY,WHITE,app.doc.name,310);
    box(10,60,300,1,LINE);
    if(!app.outline_first)text(10,70,BODY,MUTED,"No functions or classes found.",310);
    int line=app.outline_first;
    for(int i=0;i<8 && line;i++){
        char label[80],number[12];
        edit_outline(&app.doc,line-1,1,label,sizeof label);
        int y=63+i*18;
        if(line==app.outline_line)panel(8,y,304,18,ACCENT,DEEP);
        snprintf(number,sizeof number,"%d",line);
        right(42,y+2,SMALL,ACCENT,number);
        fitted(50,y+1,CODE,WHITE,label,302);
        line=edit_outline(&app.doc,line,1,NULL,0);
    }
    footer();int x=key(8,"Enter","Go to line");key(x,"Esc","Back");
}
void render(uint16_t *pixels){
    fb=pixels;box(0,0,320,240,BG);
    if(app.page==CATALOG)command_catalog();
    else if(app.page==SETTINGS)settings();
    else if(app.page==OUTLINE)outline();
    else if(app.page==GUIDE)guide();
    else if(app.page==FILES)files();
    else if(app.page==EDITOR)editor();
    else if(app.page==SHELL||app.page==OUTPUT||app.page==READ_INPUT)console();
    else modal();
}

void device_draw_text(uint16_t *pixels, int x, int y, int size, uint16_t color, const char *value) {
    uint16_t *previous = fb;
    fb = pixels;
    text(x, y, size, color, value, EP_WIDTH);
    fb = previous;
}


void device_measure_text(const char *value, int font, int *w, int *h){
    *w=width(font,value);
    *h=fonts[font].height>fonts[CODE].height?fonts[font].height:fonts[CODE].height;
}

void device_rasterize_text(uint8_t *pixels,int w,int h,const char *value,
                           int font,uint16_t color,int antialias,int transparent){
    int x=0;
    while(*value && x<w){
        int length,ch=utf_decode(value,&length);
        int face=ch>126?CODE:font;
        const Font *layout=&fonts[face];
        const unsigned char *g=glyph(face,ch);
        for(unsigned row=0;row<layout->height && (int)row<h;row++){
            for(unsigned col=0;col<layout->stride*4 && x+(int)col<w;col++){
                int coverage=glyph_alpha(layout,g+1,row,col);
                if(!coverage || ((!antialias||transparent) && coverage<2))continue;
                uint8_t *p=pixels+2*(row*w+x+col);
                uint16_t result=antialias&&!transparent?blend(color,(p[0]<<8)|p[1],coverage):color;
                p[0]=(uint8_t)(result>>8);
                p[1]=(uint8_t)result;
            }
        }
        x+=*g;
        value+=length;
    }
}
