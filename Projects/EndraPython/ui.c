#include "app.h"
#include "runtime.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

App app;
const char *const ep_symbols[]={"\"","?","'","!","$","°","%",":","_","\\","[","]","(",")","{","}","=",",",".",";","#","+","-","*","/","<",">","@","&","|","^","~","`","¡","¿"};
const int ep_symbol_count=sizeof ep_symbols/sizeof ep_symbols[0];
const Command *page_commands(Page page){
    static const Command files[]={{"Help",OPEN_GUIDE},{"Browse Documents",BROWSE},{NULL,0}};
    static const Command editor[]={
        {"Help",OPEN_GUIDE},{"Command catalog",OPEN_CATALOG},{"Run script",RUN},{"Save",SAVE},{"Find text",SEARCH},
        {"Replace all",REPLACE},{"Go to line",GO_LINE},{"Outline",SHOW_OUTLINE},
        {"Marks and symbols",MARKS},{"TI catalog input",TI_INPUT},{"Undo",UNDO},
        {"Redo",REDO},{"Copy",COPY},{"Cut",CUT},{"Paste",PASTE},
        {"Select all",SELECT_ALL},{"Files",ESC},{NULL,0}
    };
    static const Command shell[]={{"Help",OPEN_GUIDE},{"Command catalog",OPEN_CATALOG},{"List modules",LIST_MODULES},
        {"Marks and symbols",MARKS},{"TI catalog input",TI_INPUT},{"Files",ESC},{NULL,0}};
    static const Command output[]={{"Help",OPEN_GUIDE},{"Run again",RUN},{"Editor",ESC},{NULL,0}};
    if(page==FILES)return files;
    if(page==SHELL)return shell;
    if(page==OUTPUT)return output;
    return editor;
}
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
    else if(app.page==READ_INPUT){dst=app.reply;cap=sizeof app.reply;}
    else if(app.page==FIND || app.page==REPLACE_FIND){dst=app.query;cap=sizeof app.query;}
    else if(app.page==REPLACE_WITH){dst=app.replacement;cap=sizeof app.replacement;}
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
    if(!app.name[0]){strcpy(app.status,"Enter a script name");return;}
    const char *base=app.name;
    int n=snprintf(name,sizeof name,"%s%s",base,valid_script(base)?"":".py.tns");
    if(n>=(int)sizeof name||!valid_script(name)){strcpy(app.status,"Choose a shorter plain filename");return;}
    edit_reset_history();
    memset(&app.doc,0,sizeof app.doc);strcpy(app.doc.name,name);
    app.doc.dirty=1;
    if(save_script())app.page=EDITOR;
}
static void shell_submit(void) {
    app.offset = 0;
    strcpy(app.history, app.command);
    app.command[0] = 0;
    if (!strcmp(app.history, ":clear")) {
        reset_output();
        return;
    }
    console_write(">>> ", 4);
    console_write(app.history, strlen(app.history));
    console_write("\n", 1);
    char path[PATH_CAP];
    int action = terminal_command(app.history, path);
    if (!action) runtime_exec("<shell>", app.history, 1);
    if (action < 2) return;

    char *name = strrchr(path, '/');
    *name++ = 0;
    const char *previous = app.root;
    app.root = path;
    int loaded = load_script(name);
    app.root = previous;
    if (!loaded || (strcmp(path, previous) && !browse_folder(path))) {
        console_write(app.status, strlen(app.status));
        console_write("\n", 1);
        return;
    }
    runtime_end();
    app.page = EDITOR;
    if (action == 3) run();
}

void app_key(int k) {
    if(!k)return;
    app.status[0]=0;
    if(k==MENU && (app.page==FILES || app.page==EDITOR || app.page==SHELL || app.page==OUTPUT)){
        app.origin=app.page;app.page=ACTIONS;app.action=app.symbols=0;return;
    }
    if(k==OPEN_GUIDE){
        app.help_topic=app.page==FILES?HELP_FILES:app.page==SHELL?HELP_SHELL:app.page==OUTPUT?HELP_OUTPUT:HELP_EDITOR;
        app.origin=app.page;app.page=GUIDE;return;
    }
    if(app.page==GUIDE){
        if(k==ESC || k==MENU)app.page=app.origin;
        if(k==RIGHT || k==DOWN)app.help_topic=(app.help_topic+1)%HELP_COUNT;
        if(k==LEFT || k==UP)app.help_topic=(app.help_topic+HELP_COUNT-1)%HELP_COUNT;
        return;
    }
    if(app.page==CATALOG){catalog_key(k);return;}
    if(k==OPEN_CATALOG && (app.page==SHELL || app.page==EDITOR)){catalog_open();return;}
    if(k==MARKS && (app.page==EDITOR || app.page==SHELL || app.page==NAME || app.page==FIND || app.page==REPLACE_FIND || app.page==REPLACE_WITH || app.page==READ_INPUT)){
        app.origin=app.page;app.page=ACTIONS;app.action=0;app.symbols=1;return;
    }
    if(k==TI_INPUT && (app.page==EDITOR || app.page==SHELL || app.page==FIND || app.page==REPLACE_FIND || app.page==REPLACE_WITH || app.page==NAME || app.page==READ_INPUT)){
        app.request_input=1;return;
    }
    if(app.page==READ_INPUT){
        if(k==ENTER)app.input_result=1;
        else if(k==ESC)app.input_result=-1;
        else if(k>=32 && k<127 && strlen(app.reply)+1>=sizeof app.reply)
            strcpy(app.status,"Input field is full");
        else text_field(app.reply,sizeof app.reply,k);
        return;
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
        else if(k==BROWSE)browse_folder(app.documents);
        else if((k==ESC || k==LEFT) && app.browsing)browse_parent();
        else if(k==ESC)app.quit=1;
        else if(k==ENTER || (k==RIGHT && app.selected>=2 && !app.entries[app.selected-2].folder)){
            if(app.selected==0)begin_name();
            else if(app.selected==1)start_shell();
            else {
                struct FileEntry *item=&app.entries[app.selected-2];
                if(item->folder){
                    char path[PATH_CAP];
                    if(snprintf(path,sizeof path,"%s/%s",app.root,item->name)>=(int)sizeof path)
                        strcpy(app.status,"Folder path too long");
                    else browse_folder(path);
                }else if(load_script(item->name)){
                    app.page=EDITOR;
                    if(k==ENTER)run();
                }
            }
        }
    }else if(app.page==NAME){
        if(k==ESC)show_files();
        else if(k==ENTER)create_doc();
        else text_field(app.name,sizeof app.name,k);
    }else if(app.page==EDITOR){
        if(k==ESC)leave_doc();
        else if(k==SEARCH)app.page=FIND;
        else if(k==GO_LINE){app.name[0]=0;app.page=GOTO_LINE;}
        else if(k==REPLACE){app.replacement[0]=0;app.page=REPLACE_FIND;}
        else if(k==SHOW_OUTLINE){
            app.outline_line=edit_outline(&app.doc,0,1,NULL,0);
            app.outline_first=app.outline_line;
            app.page=OUTLINE;
        }
        else if(k==SAVE)save_script();
        else if(k==RUN)run();
        else edit_key(&app.doc,k);
    }else if(app.page==FIND){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER){app.page=EDITOR;edit_find(&app.doc,app.query);}
        else text_field(app.query,sizeof app.query,k);
    }else if(app.page==GOTO_LINE){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER){
            int line=0;
            for(const char *p=app.name;*p;p++)line=line*10+(*p-'0');
            if(line>0 && line<=line_number(app.doc.text,(int)strlen(app.doc.text))+1){
                edit_goto_line(&app.doc,line);
                app.page=EDITOR;
            }else strcpy(app.status,"Enter a line in this script");
        }else if(k==8 || (k>='0' && k<='9'))text_field(app.name,5,k);
    }else if(app.page==REPLACE_FIND || app.page==REPLACE_WITH){
        if(k==ESC)app.page=app.page==REPLACE_WITH?REPLACE_FIND:EDITOR;
        else if(k==ENTER && app.page==REPLACE_FIND){
            if(app.query[0])app.page=REPLACE_WITH;
            else strcpy(app.status,"Enter text to replace");
        }else if(k==ENTER){
            int count=edit_replace_all(&app.doc,app.query,app.replacement);
            if(count < 0)strcpy(app.status,"Result exceeds the 8 KiB limit");
            else {
                snprintf(app.status,sizeof app.status,"Replaced %d matches",count);
                app.page=EDITOR;
            }
        }else if(app.page==REPLACE_FIND)text_field(app.query,sizeof app.query,k);
        else text_field(app.replacement,sizeof app.replacement,k);
    }else if(app.page==OUTLINE){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER && app.outline_line){
            edit_goto_line(&app.doc,app.outline_line);
            app.page=EDITOR;
        }else if(app.outline_line && (k==UP || k==DOWN)){
            int next=edit_outline(&app.doc,app.outline_line,k==DOWN?1:-1,NULL,0);
            if(next){
                app.outline_line=next;
                if(next<app.outline_first)app.outline_first=next;
                else {
                    int last=app.outline_first;
                    for(int i=0;i<7 && last && last<next;i++)last=edit_outline(&app.doc,last,1,NULL,0);
                    if(last && last<next)app.outline_first=edit_outline(&app.doc,app.outline_first,1,NULL,0);
                }
            }
        }
    }else if(app.page==DISCARD){
        if(k==ESC)app.page=EDITOR;
        else if(k==ENTER)show_files();
        else if(k==SAVE && save_script())show_files();
    }else if(app.page==ACTIONS){
        const Command *commands=page_commands(app.origin);
        int count=0;
        if(app.symbols)count=ep_symbol_count;else while(commands[count].label)count++;
        if(k==ESC)app.page=app.origin;
        else if(k==TI_INPUT && app.symbols){app.page=app.origin;app_key(TI_INPUT);}
        else if(app.symbols && k==DOWN){if(app.action+12<count)app.action+=12;}
        else if(app.symbols && k==UP){if(app.action>=12)app.action-=12;}
        else if(k==DOWN||k==RIGHT)app.action=(app.action+1)%count;
        else if(k==UP||k==LEFT)app.action=(app.action+count-1)%count;
        else if(k==ENTER){
            app.page=app.origin;
            if(app.symbols)app_insert_text(ep_symbols[app.action]);
            else app_key(commands[app.action].key);
        }
    }else if(app.page==OUTPUT){
        if(k==ESC)app.page=EDITOR;
        else if(k==RUN)run();
        else if(k==ENTER && app.error_line>0){
            const char *file = runtime_error_file();
            if (!strcmp(file, app.doc.name) || load_script(file)) {
                app.page=EDITOR;
                edit_goto_line(&app.doc,app.error_line);
            }
        }
        else if(k==UP && app.offset+12<app.rows)app.offset++;
        else if(k==DOWN && app.offset)app.offset--;
    }else if(app.page==SHELL){
        if(k==ESC){runtime_end();show_files();}
        else if(k==LIST_MODULES){
            const char *command="help('modules')";
            app.offset=0;console_write(">>> ",4);console_write(command,strlen(command));console_write("\n",1);
            runtime_exec("<shell>",command,1);
        }
        else if(k==UP)strcpy(app.command,app.history);
        else if(k==LEFT && app.offset+10<app.rows)app.offset++;
        else if(k==RIGHT && app.offset)app.offset--;
        else if(k==ENTER && app.command[0])shell_submit();
        else text_field(app.command,sizeof app.command,k);
    }
}
