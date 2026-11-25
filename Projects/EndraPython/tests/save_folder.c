#include "app.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Simulate root mkdir results seen by the caller. All other operations use
 * the real host filesystem, including the exclusive save lock directory. */
static const char *watched_root;
static enum { NORMAL, AMBIGUOUS_CREATE, DENIED } response;
static int root_creates;
int __real_mkdir(const char *, mode_t);
int __wrap_mkdir(const char *path, mode_t mode){
    if(!watched_root || strcmp(path,watched_root))return __real_mkdir(path,mode);
    root_creates++;
    if(response==DENIED){errno=EACCES;return -1;}
    int rc=__real_mkdir(path,mode);
    if(response==AMBIGUOUS_CREATE || (rc && errno==EEXIST)){
        errno=EIO;return -1;
    }
    return rc;
}
void test_save_folder(void *stack){
    char parent[]="/tmp/endrapython-folder-XXXXXX";
    char folder[128],script[160];
    assert(mkdtemp(parent));
    snprintf(folder,sizeof folder,"%s/EndraPython",parent);
    snprintf(script,sizeof script,"%s/Hello_World.py.tns",folder);
    watched_root=folder;

    /* The first create makes the directory visible despite its return code. */
    response=AMBIGUOUS_CREATE;
    app_init(folder,stack);app_key(NEW_SCRIPT);
    for(const char *s="Hello_World";*s;s++)app_key(*s);
    app_key(ENTER);app_insert_text("print(sum(range(10)))");app_key(SAVE);
    assert(app.page==EDITOR && app.doc.existing && root_creates==1);

    /* Reopen Run001-compatible source and make the photographed edit. */
    response=NORMAL;
    app_init(folder,stack);assert(app.count==1 && !app.status[0]);
    app_key(RIGHT);
    char *ten=strstr(app.doc.text,"range(10)");assert(ten);
    app.doc.cursor=(int)(ten-app.doc.text)+8;
    app_key(8);app_key(8);app_key('5');app_key(RUN);
    assert(app.page==OUTPUT && !app.error && root_creates==1);
    int found=0;
    for(int i=0;i<app.rows;i++)if(!strcmp(app.output[(app.first+i)%OUT_LINES],"10"))found=1;
    assert(found);
    app_key(ESC);app_key(' ');app_key(SAVE);
    assert(!app.doc.dirty && root_creates==1);
    app_init(folder,stack);assert(load_script("Hello_World.py.tns"));
    assert(strstr(app.doc.text,"range(5 )") && root_creates==1);
    assert(!unlink(script) && !rmdir(folder));

    response=DENIED;
    app.page=EDITOR;app_key(' ');app_key(RUN);
    assert(app.page==EDITOR && app.doc.dirty && !access(parent,F_OK) && access(folder,F_OK));
    assert(strstr(app.status,"Cannot create"));
    FILE *block=fopen(folder,"wb");assert(block);assert(fputs("preserve",block)>=0);assert(!fclose(block));
    int before=root_creates;app_key(SAVE);
    assert(app.doc.dirty && root_creates==before && strstr(app.status,"not a folder"));
    char content[9]={0};block=fopen(folder,"rb");assert(block);
    assert(fread(content,1,8,block)==8 && !fclose(block) && !strcmp(content,"preserve"));
    assert(!unlink(folder)&&!rmdir(parent));
    watched_root=NULL;
    puts("PASS: first create despite ambiguous result, reopened example edit/run=10, repeat save, zero extra root mkdir calls, blocked path and denied creation.");
}
