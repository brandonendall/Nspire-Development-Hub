#include "app.h"
#include "runtime.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

App app;
static const char hello[]="print('Hello from EndraPython')\nprint(sum(range(10)))\n";
const char *const ep_symbols[]={"\"","?","'","!","$","°","%",":","_","\\","[","]","(",")","{","}","=",",",".",";","#","+","-","*","/","<",">","@","&","|","^","~","`","¡","¿"};
const int ep_symbol_count=sizeof ep_symbols/sizeof ep_symbols[0];
const char *const ep_actions[]={"Marks and symbols","TI catalog input","Save","Run script","Find text","Undo","Redo","Copy","Cut","Paste","Select all","Files",NULL};
static const int action_keys[]={MARKS,TI_INPUT,SAVE,RUN,SEARCH,UNDO,REDO,COPY,CUT,PASTE,SELECT_ALL,ESC};
static void reset_output(void) {
    memset(app.output,0,sizeof app.output);
    app.rows=1;app.first=app.col=app.output_bytes=app.offset=0;
}
static void new_output_line(void) {
    int slot=(app.first+app.rows)%OUT_LINES;
    if(app.rows==OUT_LINES)app.first=(app.first+1)%OUT_LINES;
    else app.rows++;
    app.output[slot][0]=0;app.col=app.output_bytes=0;
}
void console_write(const char *s,size_t n) {
    for(size_t i=0;i<n;i++){
        unsigned char c=s[i];
        if(c=='\r')continue;
        if(c=='\n'){new_output_line();continue;}
        int lead=(c&0xc0)!=0x80;
        if((lead && app.col==OUT_WIDTH) || app.output_bytes==4*OUT_WIDTH)new_output_line();
        char *line=app.output[(app.first+app.rows-1)%OUT_LINES];
        if(lead)app.col++;
        line[app.output_bytes++]=c>=32 && c!=127?(char)c:'?';line[app.output_bytes]=0;
    }
}
void app_init(const char *root,void *stack) {
    memset(&app,0,sizeof app);
    app.home=root;app.stack=stack;
    if(strlen(root)>=sizeof app.folder){app.quit=1;return;}
    strcpy(app.folder,root);app.root=app.folder;strcpy(app.documents,root);
    char *end=strrchr(app.documents,'/');if(end && end!=app.documents)*end=0;
    files_refresh();
}
static void show_files(void){app.page=FILES;files_refresh();}
static void leave_doc(void){if(app.doc.dirty)app.page=DISCARD;else show_files();}
static void run(void){
    if((app.doc.dirty||!app.doc.existing)&&!save_script())return;
    app.status[0]=0;reset_output();runtime_start(app.stack);
    app.error=!runtime_exec(app.doc.name,app.doc.text,0);
    app.error_line=runtime_error_line();
    runtime_end();app.page=OUTPUT;
}
static void text_field(char *s,size_t cap,int k){
    size_t n=strlen(s);
    if(k==8&&n)s[utf_prev(s,(int)n)]=0;
    else if(k>=32&&k<127&&n+1<cap){s[n]=(char)k;s[n+1]=0;}
}
void app_insert_text(const char *value){
    if(!*value)return;
    if(app.page==EDITOR){edit_insert(&app.doc,value);return;}
    char *dst;size_t cap;
    if(app.page==SHELL){dst=app.command;cap=sizeof app.command;}
    else if(app.page==FIND){dst=app.query;cap=sizeof app.query;}
    else if(app.page==NAME){dst=app.name;cap=sizeof app.name;}
    else return;
    if(strlen(dst)+strlen(value)>=cap){strcpy(app.status,"Text too long for this field");return;}
    strcat(dst,value);
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
    edit_reset_history();
    memset(&app.doc,0,sizeof app.doc);strcpy(app.doc.name,name);
    if(!app.name[0])strcpy(app.doc.text,hello);
    app.doc.dirty=1;
    if(save_script())app.page=EDITOR;
}
void app_key(int k) {
    if(!k)return;
    app.status[0]=0;
    if(k==MARKS && (app.page==EDITOR || app.page==SHELL || app.page==NAME || app.page==FIND)){
        app.origin=app.page;app.page=ACTIONS;app.action=0;app.symbols=1;return;
    }
    if(k==TI_INPUT && (app.page==EDITOR || app.page==SHELL || app.page==FIND || app.page==NAME)){
        app.request_input=1;return;
    }
    if(app.page==FILES){
        int last=app.count+1;
        if(k==RIGHT && app.selected==0)app.selected=1;
        else if(k==LEFT && app.selected==1)app.selected=0;
        else if(k==DOWN && app.selected<2 && app.count)app.selected=2;
        else if(k==UP && app.selected)app.selected--;
        else if(k==DOWN && app.selected<last)app.selected++;
        else if(k==INDENT && (app.more_files || app.file_page)){
            app.file_page=app.more_files?app.file_page+1:0;files_refresh();
            if(app.count)app.selected=2;
        }
        else if(k==MENU)browse_folder(app.documents);
        else if((k==ESC || k==LEFT) && app.browsing)browse_parent();
        else if(k==ESC)app.quit=1;
        else if(k==ENTER){
            if(app.selected==0)begin_name();
            else if(app.selected==1)start_shell();
            else {
                struct FileEntry *item=&app.entries[app.selected-2];
                if(item->folder){
                    char path[PATH_CAP];
                    if(snprintf(path,sizeof path,"%s/%s",app.root,item->name)>=(int)sizeof path)
                        strcpy(app.status,"Folder path too long");
                    else browse_folder(path);
                }else if(load_script(item->name))app.page=EDITOR;
            }
        }
    }else if(app.page==NAME){
        if(k==ESC)show_files();
        else if(k==ENTER)create_doc();
        else text_field(app.name,sizeof app.name,k);
    }else if(app.page==EDITOR){
        if(k==ESC)leave_doc();
        else if(k==MENU){app.page=ACTIONS;app.action=app.symbols=0;}
        else if(k==SEARCH)app.page=FIND;
        else if(k==SAVE)save_script();
        else if(k==RUN)run();
        else edit_key(&app.doc,k);
    }else if(app.page==FIND){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER){app.page=EDITOR;edit_find(&app.doc,app.query);}
        else text_field(app.query,sizeof app.query,k);
    }else if(app.page==DISCARD){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER)show_files();
        else if(k==SAVE && save_script())show_files();
    }else if(app.page==ACTIONS){
        int count=app.symbols?ep_symbol_count:(int)(sizeof action_keys/sizeof action_keys[0]);
        if(k==ESC)app.page=app.symbols?app.origin:EDITOR;
        else if(k==TI_INPUT && app.symbols){app.page=app.origin;app_key(TI_INPUT);}
        else if(app.symbols && k==DOWN){if(app.action+12<count)app.action+=12;}
        else if(app.symbols && k==UP){if(app.action>=12)app.action-=12;}
        else if(k==DOWN||k==RIGHT)app.action=(app.action+1)%count;
        else if(k==UP||k==LEFT)app.action=(app.action+count-1)%count;
        else if(k==ENTER){
            app.page=app.symbols?app.origin:EDITOR;
            if(app.symbols)app_insert_text(ep_symbols[app.action]);
            else app_key(action_keys[app.action]);
        }
    }else if(app.page==OUTPUT){
        if(k==ESC)app.page=EDITOR;
        else if(k==RUN)run();
        else if(k==ENTER && app.error_line>0){
            app.page=EDITOR;edit_goto_line(&app.doc,app.error_line);
        }
        else if(k==UP && app.offset+12<app.rows)app.offset++;
        else if(k==DOWN && app.offset)app.offset--;
    }else if(app.page==SHELL){
        if(k==ESC){runtime_end();show_files();}
        else if(k==MENU){
            const char *command="help('modules')";
            app.offset=0;console_write(">>> ",4);console_write(command,strlen(command));console_write("\n",1);
            runtime_exec("<shell>",command,1);
        }
        else if(k==UP)strcpy(app.command,app.history);
        else if(k==LEFT && app.offset+10<app.rows)app.offset++;
        else if(k==RIGHT && app.offset)app.offset--;
        else if(k==ENTER && app.command[0]){
            app.offset=0;console_write(">>> ",4);console_write(app.command,strlen(app.command));console_write("\n",1);
            strcpy(app.history,app.command);runtime_exec("<shell>",app.command,1);app.command[0]=0;
        }else text_field(app.command,sizeof app.command,k);
    }
}
