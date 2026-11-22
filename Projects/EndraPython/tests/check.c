#define _POSIX_C_SOURCE 200809L
#include "app.h"
#include "runtime.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
static int cancel;
int cancel_pressed(void){return cancel;}
static void type(const char *s){while(*s)app_key((unsigned char)*s++);}
static int output_has(const char *s){
    for(int i=0;i<app.rows;i++)if(strstr(app.output[(app.first+i)%OUT_LINES],s))return 1;
    return 0;
}
static void preview(const char *name){
    static uint16_t screen[320*240];
    char path[128];snprintf(path,sizeof path,"build/%s.ppm",name);
    render(screen);FILE *f=fopen(path,"wb");assert(f);
    fprintf(f,"P6\n320 240\n255\n");
    for(int p=0;p<320*240;p++){
        unsigned v=screen[p];
        fputc((v>>11)*255/31,f);fputc(((v>>5)&63)*255/63,f);fputc((v&31)*255/31,f);
    }
    assert(!fclose(f));
}
static void write_file(const char *root,const char *name,const char *src){
    char path[512];snprintf(path,sizeof path,"%s/%s",root,name);
    FILE *f=fopen(path,"wb");assert(f);assert(fwrite(src,1,strlen(src),f)==strlen(src));assert(!fclose(f));
}
int main(void){
    int stack;
    char root[]="/tmp/endrapython-XXXXXX";assert(mkdtemp(root));
    char scripts[150];snprintf(scripts,sizeof scripts,"%s/scripts",root);
    app_init(scripts,&stack);assert(!app.count && access(scripts,F_OK));
    app_key(ENTER);app_key(ENTER);assert(app.page==EDITOR && app.doc.existing);
    assert(!strcmp(app.doc.name,"Hello_World.py.tns"));
    app_key(RUN);assert(app.page==OUTPUT && !app.error && output_has("Hello from EndraPython") && output_has("45"));
    preview("console");app_key(ESC);app_key(ESC);assert(app.count==1);
    app_key(ENTER);app_key(ENTER);assert(app.page==NAME && strstr(app.status,"in use"));app_key(ESC);
    app_key(DOWN);app_key(DOWN);app_key(ENTER);assert(app.page==EDITOR);
    assert(app.doc.cursor==0);app_key(RIGHT);app_key(RIGHT);preview("editor");
    app_key(LEFT);app_key(LEFT);type("#");app_key(RUN);assert(!app.error&&!output_has("Hello from EndraPython")&&output_has("45"));
    app_key(ESC);assert(app.doc.cursor==1);app_key(8);app_key(SAVE);app_key(ESC);preview("files");
    app_key(ENTER);type("Broken");app_key(ENTER);type("print(42)");app_key(ENTER);type("if :");app_key(RUN);
    assert(app.error && output_has("SyntaxError") && output_has("line 2"));preview("error");
    app_key(ESC);app_key(8);app_key(ESC);assert(app.page==DISCARD);app_key(ESC);assert(app.page==EDITOR);
    app_key(ESC);app_key(ENTER);assert(app.page==FILES);
    app_key(DOWN);app_key(ENTER);assert(app.page==SHELL);
    type("x = 7");app_key(ENTER);type("sum(range(x))");app_key(ENTER);
    assert(output_has("21"));app_key(UP);assert(!strcmp(app.command,"sum(range(x))"));preview("repl");
    assert(runtime_exec("<test>","import gc, math, collections\nprint(math.sqrt(81), 2**40)",0));
    assert(output_has("1099511627776"));
    assert(!runtime_exec("<test>","a = [0] * 2000000",0));assert(output_has("MemoryError"));
    cancel=1;assert(!runtime_exec("<test>","while True:\n pass\n",0));cancel=0;assert(output_has("KeyboardInterrupt"));
    assert(runtime_exec("<test>","print(6*7)",0));assert(output_has("42"));
    assert(!runtime_exec("<test>","import os",0));
    assert(!runtime_exec("<test>","import json",0));
    assert(!runtime_exec("<test>","import time",0));
    app_key(ESC);assert(app.page==FILES);
    for(int i=0;i<24;i++){assert(load_script("Hello_World.py.tns"));app.page=EDITOR;app_key(RUN);assert(!app.error && output_has("45"));app_key(ESC);}
    assert(!valid_script("../x.py")&&!valid_script("x/abc.py")&&!valid_script("system.tns"));
    write_file(scripts,"foreign.py","print('\xc3\xa9')\n");assert(!load_script("foreign.py"));
    static char big[SOURCE_CAP+2];memset(big,'x',SOURCE_CAP+1);write_file(scripts,"large.py",big);assert(!load_script("large.py"));
    write_file(scripts,"Windows.py","if True:\r\n    pass\r\n");assert(load_script("Windows.py"));
    app.doc.cursor=strlen(app.doc.text)-1;edit_key(&app.doc,ENTER);assert(strstr(app.doc.text,"pass\n    "));
    assert(save_script());assert(strstr(app.doc.original,"pass\r\n    "));
    write_file(scripts,"Windows.py","changed");assert(!save_script() && strstr(app.status,"changed"));
    assert(load_script("Windows.py"));app.doc.cursor=7;edit_key(&app.doc,'!');
    char lock[192];snprintf(lock,sizeof lock,"%s/.saving",scripts);assert(!mkdir(lock,0700));
    assert(!save_script() && app.doc.dirty);assert(!rmdir(lock));assert(load_script("Windows.py")&&!strcmp(app.doc.text,"changed"));
    memset(app.doc.text,'z',SOURCE_CAP);app.doc.text[SOURCE_CAP]=0;
    app.doc.cursor=SOURCE_CAP;edit_key(&app.doc,'!');assert(strlen(app.doc.text)==SOURCE_CAP);
    memset(&app.doc,0,sizeof app.doc);
    for(int i=0;i<100;i++){edit_key(&app.doc,'a');edit_key(&app.doc,ENTER);}
    for(int i=0;i<80;i++)edit_key(&app.doc,'x');
    assert(app.doc.top==88 && app.doc.left==47);edit_key(&app.doc,UP);assert(app.doc.left==1);
    app.page=FILES;app_key(ESC);assert(app.quit);
    const char *names[]={"Hello_World.py.tns","Broken.py.tns","Windows.py","foreign.py","large.py"};
    for(int i=0;i<5;i++){char path[256];snprintf(path,sizeof path,"%s/%s",scripts,names[i]);assert(!unlink(path));}
    assert(!rmdir(scripts)&&!rmdir(root));
    puts("PASS: script lifecycle, edit/run, console, syntax locations, REPL, modules, heap exhaustion, interrupt, 24 reruns, storage failures, file boundaries, 100-line navigation and 5 framebuffer previews.");
}
