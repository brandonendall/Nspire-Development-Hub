#define _POSIX_C_SOURCE 200809L
#include "app.h"
#include "device.h"
#include "runtime.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
void test_save_folder(void *stack);
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
static void pick(const char *ch){
    app_key(MARKS);assert(app.page==ACTIONS && app.symbols);
    for(int i=0;strcmp(ch,ep_symbols[app.action]);i++){assert(i<ep_symbol_count);app_key(RIGHT);}
    app_key(ENTER);
}
static void choose_command(int command){
    app_key(MENU);assert(app.page==ACTIONS && !app.symbols);
    const Command *commands=page_commands(app.origin);
    while(commands[app.action].key!=command){
        assert(commands[app.action+1].label);app_key(DOWN);
    }
    app_key(ENTER);
}
static void help_check(int topic){
    Page page=app.page;int pos=app.doc.cursor,sel=app.selected,offset=app.offset;
    choose_command(OPEN_GUIDE);assert(app.page==GUIDE && app.help_topic==topic);
    for(int i=0;i<HELP_COUNT;i++){
        char frame[24];snprintf(frame,sizeof frame,"help%d",app.help_topic);preview(frame);
        app_key(RIGHT);
    }
    assert(app.help_topic==topic);app_key(LEFT);app_key(RIGHT);
    app_key(ESC);assert(app.page==page && app.doc.cursor==pos && app.selected==sel && app.offset==offset);
}
static const char *answer;
void platform_wait_input(void){
    assert(app.page==READ_INPUT);
    if(answer){app_insert_text(answer);if(*answer)preview("input");app_key(ENTER);}
    else app_key(ESC);
}
static void catalog_checks(const char *root,void *stack){
    app_init(root,stack);help_check(HELP_FILES);
    choose_command(NEW_SCRIPT);type("catalog_test");app_key(ENTER);assert(!app.doc.text[0]);
    app_key(INDENT);assert(!strcmp(app.doc.text,"  "));app_key(ENTER);assert(!strcmp(app.doc.text,"  \n  "));
    app_key('x');app_key(INDENT);assert(strstr(app.doc.text,"  x "));
    app_key(SELECT_ALL);type("name=");
    app_key(CATALOG_KEY);assert(app.page==EDITOR && !app.request_input);
    choose_command(OPEN_CATALOG);assert(app.page==CATALOG && catalog_browsing_modules);preview("catalog_modules");
    app_key(ENTER);type("in");preview("catalog");
    app_key(ENTER);assert(!strcmp(app.doc.text,"name=input("));type("'Name? ')");app_key(ENTER);
    choose_command(OPEN_CATALOG);app_key(ENTER);app_key(ENTER);type("name)");preview("input_script");help_check(HELP_EDITOR);
    app_key(MENU);type("settings");preview("focus_palette");app_key(ENTER);
    assert(app.page==SETTINGS);preview("focus_settings");
    app_key(ENTER);assert(!app.auto_indent);app_key(DOWN);
    app_key(ENTER);assert(!app.indent_guides);app_key(DOWN);
    app_key(ENTER);assert(!app.line_numbers);app_key(ESC);
    assert(app.page==EDITOR);
    choose_command(OPEN_SETTINGS);
    for(int i=0;i<3;i++){app_key(ENTER);app_key(DOWN);}
    app_key(ESC);assert(app.auto_indent && app.indent_guides && app.line_numbers);
    answer="Brandon";app_key(RUN);assert(!app.error && output_has("Brandon"));help_check(HELP_OUTPUT);
    app_key(ESC);app_key(ESC);choose_command(OPEN_SHELL);assert(app.page==SHELL);
    type("keep=27");app_key(ENTER);app_key(CATALOG_KEY);assert(app.page==SHELL && !app.request_input);
    choose_command(OPEN_CATALOG);app_key(ENTER);
    for(int i=0;i<catalog_count;i++){
        char check[96];snprintf(check,sizeof check,"assert callable(%s)",catalog_entries[i].name);
        assert(runtime_exec("<check>",check,0));
    }
    app_key(LEFT);
    assert(catalog_browsing_modules && catalog_group_count > 18);
    for(int i=0;i<catalog_group_count;i++){
        assert(strcmp(catalog_groups[i],"casioplot"));
        app_key(ENTER);
        if(strcmp(catalog_groups[i],"__main__"))assert(catalog_count>0 && catalog_count<CATALOG_CAP);
        app_key(LEFT);app_key(DOWN);
    }
    type("pygame");app_key(ENTER);type("draw.rect");
    assert(!strcmp(catalog_entries[catalog_index].name,"draw.rect"));
    assert(!strcmp(catalog_signature(),"pygame.draw.rect(surface,color,rect[,width])"));
    app_key(LEFT);
    type("builtins");app_key(ENTER);type("input");
    assert(!strcmp(catalog_entries[catalog_index].name,"input"));
    app_key(ESC);assert(catalog_browsing_modules);app_key(ESC);
    type("import math");app_key(ENTER);choose_command(OPEN_CATALOG);type("math");app_key(ENTER);
    type("sqrt");assert(!strcmp(catalog_signature(),"math.sqrt(x)"));
    app_key(ENTER);assert(!strcmp(app.command,"math.sqrt("));type("9)");app_key(ENTER);assert(output_has("3.0"));
    type("import casioplot");app_key(ENTER);assert(output_has("ImportError"));
    type("keep");app_key(ENTER);assert(output_has("27"));
    answer="";type("len(input())");app_key(ENTER);assert(output_has("0"));
    answer=NULL;type("input()");app_key(ENTER);assert(output_has("KeyboardInterrupt"));
    type("keep+1");app_key(ENTER);assert(output_has("28"));app_key(ESC);
    char path[512];snprintf(path,sizeof path,"%s/catalog_test.py.tns",root);assert(!unlink(path));
}
static void browse_checks(const char *home,void *stack){
    app_init(home,stack);
    char other[400],nested[420],path[540];
    snprintf(other,sizeof other,"%s/Class Notes",app.documents);assert(!mkdir(other,0700));
    snprintf(nested,sizeof nested,"%s/Algebra",other);assert(!mkdir(nested,0700));
    write_file(nested,"Lesson (1).PY.TNS","print('original')");
    write_file(home,"Lesson (1).PY.TNS","print('home')");
    write_file(nested,"unrelated.tns","leave me");
    choose_command(BROWSE);assert(app.browsing && !strcmp(app.root,app.documents));
    while(strcmp(app.entries[app.selected].name,"Class Notes")){assert(app.selected+1<app.count);app_key(DOWN);}
    app_key(ENTER);assert(!strcmp(app.root,other));app_key(ENTER);assert(!strcmp(app.root,nested));
    assert(app.count==1);preview("browser");
    app_key(ENTER);assert(app.page==OUTPUT && !app.error && output_has("original"));app_key(ESC);
    app_key(SELECT_ALL);type("print(");pick("\"");pick("°");pick("\"");type(")");
    app_key(RUN);assert(!app.error && output_has("°"));
    app_key(ESC);app_key(ESC);app_key(RIGHT);assert(app.page==EDITOR && !strcmp(app.doc.text,"print(\"°\")"));
    app_key(SELECT_ALL);app_key(TI_INPUT);assert(app.request_input);app.request_input=0;
    uint16_t ti[]={'p','r','i','n','t','(','"',0x03c0,0x2264,0x03b8,'"',')'};
    char converted[64];assert(utf16_input(ti,sizeof ti/sizeof ti[0],converted,sizeof converted));
    app_insert_text(converted);app_key(RUN);assert(!app.error && output_has("π≤θ"));app_key(ESC);
    app_key(UNDO);assert(!strcmp(app.doc.text,"print(\"°\")"));
    app_key(SELECT_ALL);app_insert_text("aπb");app_key(LEFT);app_key(8);
    assert(!strcmp(app.doc.text,"ab"));app_key(UNDO);assert(!strcmp(app.doc.text,"aπb"));
    app_key(ESC);app_key(ENTER);app_key(ESC);assert(!strcmp(app.root,other));
    app_key(ESC);assert(!strcmp(app.root,app.documents));app_key(ESC);assert(!strcmp(app.root,home));
    assert(load_script("Lesson (1).PY.TNS") && !strcmp(app.doc.text,"print('home')"));
    choose_command(OPEN_SHELL);assert(app.page==SHELL);type("print(");pick("[");pick("'");
    type("ok");pick("'");pick("]");type(")");app_key(ENTER);assert(output_has("['ok']"));
    app_key(MARKS);preview("symbols");app_key(ESC);assert(app.page==SHELL && !app.command[0]);
    for(int i=0;i<ep_symbol_count;i++){pick(ep_symbols[i]);assert(!strcmp(app.command,ep_symbols[i]));app_key(8);assert(!app.command[0]);}
    app_key(MARKS);app_key(TI_INPUT);assert(app.request_input && app.page==SHELL);app.request_input=0;
    uint16_t emoji[]={0xd83d,0xde00};assert(utf16_input(emoji,2,converted,sizeof converted) && strlen(converted)==4);
    assert(!utf16_input(emoji,1,converted,sizeof converted) && !utf16_input(ti,12,converted,3));
    app_key(ESC);
    snprintf(path,sizeof path,"%s/Lesson (1).PY.TNS",home);assert(!unlink(path));
    snprintf(path,sizeof path,"%s/Lesson (1).PY.TNS",nested);assert(!unlink(path));
    snprintf(path,sizeof path,"%s/unrelated.tns",nested);assert(!unlink(path));
    for(int i=0;i<FILE_CAP+2;i++){char name[24];snprintf(name,sizeof name,"clip%03d.py",i);write_file(nested,name,"pass");}
    assert(browse_folder(nested) && app.more_files && app.count==FILE_CAP);app_key(INDENT);
    assert(app.count==2 && !app.more_files && app.file_page==1);app_key(INDENT);assert(!app.file_page);
    for(int i=0;i<FILE_CAP+2;i++){snprintf(path,sizeof path,"%s/clip%03d.py",nested,i);assert(!unlink(path));}
    assert(!rmdir(nested) && !rmdir(other));
}

static void ide_checks(const char *root,void *stack){
    char path[220];
    /* Exercise actual bundled examples in the normal load/edit/run flow. */
    const char *samples[]={"Functions.py.tns","Dice_Roller.py.tns"};
    for(int i=0;i<2;i++){
        snprintf(path,sizeof path,"examples/%s",samples[i]);
        char data[512]={0};FILE *src=fopen(path,"rb");assert(src);
        size_t n=fread(data,1,sizeof data-1,src);assert(n && n<sizeof data-1 && !fclose(src));
        write_file(root,samples[i],data);
    }
    app_init(root,stack);assert(app.selected==0);preview("files");
    assert(load_script("Dice_Roller.py.tns"));app.page=EDITOR;app_key(RUN);
    assert(app.page==OUTPUT && !app.error && output_has("Rolling a d20"));
    app_key(ESC);edit_goto_line(&app.doc,4);preview("editor");
    app_key(MENU);preview("tools");app_key(ESC);
    app_key(SEARCH);type("randint");app_key(ENTER);assert(app.doc.selecting);
    app_key(COPY);app_key(CUT);assert(!strstr(app.doc.text,"return randint"));
    app_key(PASTE);assert(strstr(app.doc.text,"return randint")&&!app.doc.dirty);
    app_key(UNDO);assert(!strstr(app.doc.text,"return randint"));app_key(REDO);
    assert(!app.doc.dirty);app_key(SEARCH);app_key(ENTER);assert(app.doc.anchor==19);
    app_key(LEFT);assert(!app.doc.selecting);app_key(EXT_RIGHT);app_key(EXT_RIGHT);
    assert(app.doc.cursor-app.doc.anchor==2);app_key(COPY);app_key(SELECT_ALL);app_key(PASTE);
    assert(!strcmp(app.doc.text,"ra"));app_key(SAVE);assert(!app.doc.dirty);
    app_key(UNDO);assert(app.doc.dirty);app_key(REDO);assert(!app.doc.dirty);
    app_key(SELECT_ALL);type("def f():");app_key(ENTER);type("  return 1//0");app_key(ENTER);
    for(int i=0;i<4;i++)app_key(8);
    type("f()");app_key(RUN);assert(app.error && app.error_line==2);app_key(ENTER);
    assert(app.page==EDITOR && app.doc.cursor==9);
    assert(load_script("Functions.py.tns"));app_key(UNDO);assert(!app.doc.dirty && strstr(app.status,"Nothing"));
    app_key(SELECT_ALL);app_key('x');
    for(int i=0;i<16;i++)app_key('x');
    for(int i=0;i<16;i++)app_key(UNDO);
    assert(strlen(app.doc.text)==5);app_key('y');app_key(REDO);assert(!strcmp(app.doc.text,"xxxxxy"));
    app_init(root,stack);choose_command(OPEN_SHELL);assert(app.page==SHELL);
    type("6*7");help_check(HELP_SHELL);assert(!strcmp(app.command,"6*7"));
    app_key(ENTER);assert(output_has("42"));
    choose_command(LIST_MODULES);assert(output_has("collections") && output_has("random") && output_has("struct"));
    preview("repl");type("help('modules')");app_key(ENTER);assert(output_has("__main__"));
    type("import random, array, cmath, struct");app_key(ENTER);
    type("random.seed(7)");app_key(ENTER);type("v=random.randint(1,20)");app_key(ENTER);
    type("1 <= v <= 20");app_key(ENTER);assert(output_has("True"));
    type("sum(array.array('i', [7, 8]))");app_key(ENTER);assert(output_has("15"));
    type("cmath.sqrt(-1)");app_key(ENTER);assert(output_has("1j"));
    type("struct.unpack('<H',struct.pack('<H',513))");app_key(ENTER);assert(output_has("513"));
    type("help()");app_key(ENTER);assert(output_has("EndraPython shell"));
    app_key(LEFT);assert(app.offset==1);app_key(RIGHT);assert(!app.offset);app_key(ESC);
    for(int i=0;i<2;i++){snprintf(path,sizeof path,"%s/%s",root,samples[i]);assert(!unlink(path));}
}

static void project_import_checks(const char *documents, void *stack) {
    char project[PATH_CAP];
    snprintf(project, sizeof project, "%s/project", documents);
    assert(mkdir(project, 0700) == 0);
    write_file(project, "main.py.tns", "from helper import twice\nprint(twice(6))\n");
    write_file(project, "helper.py.tns", "import factor\ndef twice(x):\n  return x * factor.value\n");
    write_file(project, "helper.py", "def twice(x):\n  return -1\n");
    write_file(project, "factor.py", "value = 2\r\n");
    app_init(documents, stack);
    assert(browse_folder(project));
    assert(load_script("main.py.tns"));
    app.page = EDITOR;
    app_key(RUN);
    assert(!app.error && output_has("12"));
    write_file(project, "factor.py", "value = 3\n");
    app_key(RUN);
    assert(!app.error && output_has("18"));
    write_file(project, "factor.py", "# nested failure\nvalue = 1 // 0\n");
    app_key(RUN);
    assert(app.error && app.error_line == 2 && output_has("factor.py"));
    preview("import_error");
    app_key(ENTER);
    assert(app.page == EDITOR && !strcmp(app.doc.name, "factor.py"));
    assert(app.doc.cursor == (int)strlen("# nested failure\n"));
    write_file(project, "helper.py.tns", "# syntax failure\nif :\n");
    assert(load_script("main.py.tns"));
    app_key(RUN);
    assert(app.error && app.error_line == 2 && output_has("SyntaxError"));
    app_key(ENTER);
    assert(app.page == EDITOR && !strcmp(app.doc.name, "helper.py.tns"));
    char path[PATH_CAP + NAME_CAP];
    snprintf(path, sizeof path, "%s/helper.py.tns", project);
    assert(!unlink(path));
    assert(load_script("main.py.tns"));
    app_key(RUN);
    assert(!app.error && output_has("-1"));
    snprintf(path, sizeof path, "%s/helper.py", project);
    assert(!unlink(path));
    app_key(RUN);
    assert(app.error && output_has("ImportError"));
    app_key(ENTER);
    assert(app.page == EDITOR && !strcmp(app.doc.name, "main.py.tns"));
    write_file(project, "helper.py", "\xc0\xaf");
    app_key(RUN);
    assert(app.error && output_has("ImportError"));
    char resolved[NAME_CAP];
    assert(!resolve_module_file("../other.py", resolved));
    for (const char **name = (const char *[]){"main.py.tns", "factor.py", "helper.py", NULL}; *name; name++) {
        snprintf(path, sizeof path, "%s/%s", project, *name);
        assert(!unlink(path));
    }
    assert(!rmdir(project));
}

static void storage_io_checks(const char *documents, void *stack) {
    char project[PATH_CAP], moved[PATH_CAP], home[PATH_CAP], path[PATH_CAP + NAME_CAP];
    snprintf(project, sizeof project, "%s/original", documents);
    snprintf(moved, sizeof moved, "%s/moved", documents);
    snprintf(home, sizeof home, "%s/EndraPython", documents);
    assert(mkdir(project, 0700) == 0);
    app_init(home, stack);
    assert(browse_folder(project));

    FILE *test = fopen("tests/Run008_Storage_Test.py.tns", "rb");
    assert(test);
    char source[SOURCE_CAP + 1];
    size_t bytes = fread(source, 1, sizeof source, test);
    assert(bytes < sizeof source && !ferror(test) && !fclose(test));
    source[bytes] = 0;
    write_file(project, "storage.py.tns", source);
    assert(load_script("storage.py.tns"));
    app.page = EDITOR;
    app_key(RUN);
    if (app.error) for (int i=0; i<app.rows; i++) fprintf(stderr,"%s\n", app.output[(app.first+i)%OUT_LINES]);
    assert(app.page == OUTPUT && !app.error && output_has("Saved runs: 1"));
    preview("storage_result");
    assert(rename(project, moved) == 0);
    app_init(home, stack);
    assert(browse_folder(moved) && load_script("storage.py.tns"));
    app.page = EDITOR;
    app_key(RUN);
    assert(!app.error && output_has("Saved runs: 2"));

    runtime_start(stack);
    assert(runtime_exec("<io>", "import os, json, io\n"
        "assert os.getcwd() == '/'\nos.mkdir('data')\nos.chdir('data')\n"
        "assert os.getcwd() == '/data' and 'data' in os.listdir('..')\n"
        "with open('bytes.tns', 'wb') as f:\n  f.write(bytes([0, 128, 255]))\n"
        "with open('bytes.tns', 'ab') as f:\n  f.write(b'!')\n"
        "with open('/data/bytes.tns', 'rb') as f:\n"
        "  assert f.read(2) == bytes([0, 128])\n  assert f.tell() == 2\n"
        "  f.seek(1)\n  assert f.read() == bytes([128, 255, 33])\n"
        "with open('note.tns', 'w') as f:\n  f.write('abc\\n')\n  f.flush()\n"
        "with open('note.tns', 'a') as f:\n  f.write('def')\n"
        "with open('note.tns') as f:\n  assert f.readline() == 'abc\\n'\n  assert f.read() == 'def'\n"
        "g = open('pending.tns', 'w')\ng.write('persist')\n"
        "assert json.loads(json.dumps([1, True, None])) == [1, True, None]", 0));
    assert(!runtime_exec("<bad>", "open('../..' + '/escape', 'w')", 0));
    assert(!runtime_exec("<bad>", "open('bytes.tns', 'bad')", 0));
    assert(!runtime_exec("<bad>", "open('missing.tns')", 0));
    assert(!runtime_exec("<bad>", "f.read()", 0));
    runtime_end();
    runtime_start(stack);
    assert(runtime_exec("<io>", "import os\nassert os.getcwd() == '/'\n"
        "with open('data/pending.tns') as f:\n  assert f.read() == 'persist'\n", 0));
    runtime_end();
    puts("PASS: JSON across launches and project move, text and binary I/O, virtual directories, stream cleanup and file errors.");
    const char *files[] = {"storage.py.tns", "Run008_counter.json.tns", "data/bytes.tns",
        "data/note.tns", "data/pending.tns"};
    for (size_t i=0; i<sizeof files/sizeof files[0]; i++) {
        snprintf(path, sizeof path, "%s/%s", moved, files[i]);
        assert(!unlink(path));
    }
    snprintf(path, sizeof path, "%s/data", moved);
    assert(!rmdir(path) && !rmdir(moved));
}

static void workbench_checks(const char *root, void *stack) {
    char folder[PATH_CAP], path[PATH_CAP + NAME_CAP];
    snprintf(folder,sizeof folder,"%s/Workbench Test",root);
    assert(!mkdir(folder,0700));
    write_file(folder,"Demo Script.py.tns",
        "# marker\n'''\ndef ignored():\n'''\n"
        "def original():\n    return 'marker'\n"
        "class Widget:\n    pass\nprint(original())\n");
    app_init(root,stack);
    choose_command(OPEN_SHELL);
    type(":cd Workbench Test");app_key(ENTER);
    type(":pwd");app_key(ENTER);assert(output_has("/Workbench Test"));
    type(":ls");app_key(ENTER);assert(output_has("Demo Script.py.tns"));
    type(":cat Demo Script.py.tns");app_key(ENTER);assert(output_has("class Widget:"));
    type(":edit missing.py.tns");app_key(ENTER);assert(app.page==SHELL && output_has("Unreadable"));
    type(":clear");app_key(ENTER);assert(app.rows==1 && !app.output[0][0]);
    type(":python Demo Script.py.tns");app_key(ENTER);
    assert(app.page==OUTPUT && !app.error && output_has("marker"));
    app_key(ESC);
    assert(!strcmp(app.root,folder));
    choose_command(SHOW_OUTLINE);assert(app.outline_line==5);preview("outline");
    app_key(DOWN);assert(app.outline_line==7);app_key(ENTER);
    assert(line_number(app.doc.text,app.doc.cursor)==6);
    choose_command(GO_LINE);type("9000");app_key(ENTER);assert(app.page==GOTO_LINE);
    for(int i=0;i<4;i++)app_key(8);
    type("6");app_key(ENTER);
    assert(app.page==EDITOR && line_number(app.doc.text,app.doc.cursor)==5);
    choose_command(REPLACE);type("marker");app_key(ENTER);
    type("updated");preview("replace");app_key(ENTER);
    assert(app.doc.dirty && !strstr(app.doc.text,"marker"));
    app_key(UNDO);assert(!app.doc.dirty && strstr(app.doc.text,"marker"));
    app_key(REDO);assert(app.doc.dirty);
    app_key(RUN);assert(!app.error && output_has("updated"));
    app_key(ESC);app_key(ESC);
    assert(app.page==FILES);app_key(ENTER);
    assert(app.page==OUTPUT && output_has("updated"));
    app_key(ESC);app_key(ESC);choose_command(OPEN_SHELL);
    assert(app.page==SHELL);
    type(":edit Demo Script.py.tns");app_key(ENTER);assert(app.page==EDITOR);
    choose_command(OPEN_CATALOG);type("json");app_key(ENTER);type("load");
    assert(!strcmp(catalog_signature(),"json.load(file)"));
    preview("catalog_signatures");
    app_key(INDENT);assert(app.page==EDITOR && !strncmp(app.doc.text,"import json",11));
    snprintf(path,sizeof path,"%s/Demo Script.py.tns",folder);assert(!unlink(path));
    assert(!rmdir(folder));
    puts("PASS: direct run and edit, outline, line jump, undoable replace, terminal commands and catalog imports.");
}

static void numerics_checks(const char *root, void *stack) {
    char path[PATH_CAP + NAME_CAP], source[SOURCE_CAP + 1];
    FILE *fixture = fopen("tests/Run013_Numerics_Test.py.tns", "rb");
    assert(fixture);
    size_t bytes = fread(source, 1, sizeof source, fixture);
    assert(bytes < sizeof source && !ferror(fixture) && !fclose(fixture));
    source[bytes] = 0;
    write_file(root,"Run013_Numerics_Test.py.tns",source);
    app_init(root,stack);
    for(int i=0;i<app.count;i++){
        if(!strcmp(app.entries[i].name,"Run013_Numerics_Test.py.tns"))app.selected=i;
    }
    assert(!strcmp(app.entries[app.selected].name,"Run013_Numerics_Test.py.tns"));
    app_key(ENTER);
    if(app.error){
        for(int i=0;i<app.rows;i++)fprintf(stderr,"%s\n",app.output[(app.first+i)%OUT_LINES]);
    }
    assert(app.page==OUTPUT && !app.error && output_has("Decimal checks passed") && output_has("24 x 24 multiply passed") && output_has("Free heap:"));
    preview("numerics");
    for(int i=0;i<app.rows;i++)puts(app.output[(app.first+i)%OUT_LINES]);
    app_key(ESC);app_key(ESC);choose_command(OPEN_SHELL);
    assert(app.page==SHELL);
    choose_command(LIST_MODULES);assert(output_has("numpy"));
    assert(runtime_exec("<test>","import numpy as np\n"
        "assert np.array([1, 2, 3])[1:].tolist() == [2, 3]\n"
        "assert (2 * np.matrix([[1, 2]])).tolist() == [[2, 4]]\n"
        "assert (np.matrix([[1, 2]]) @ np.matrix([[3], [4]])).tolist() == [[11]]",0));
    assert(!runtime_exec("<test>","np.dot([1], [1, 2])",0));
    assert(!runtime_exec("<test>","np.normalize([0, 0])",0));
    assert(!runtime_exec("<test>","np.matrix([[1, 2],[2, 4]]).I",0));
    app_key(ESC);
    snprintf(path,sizeof path,"%s/Run013_Numerics_Test.py.tns",root);
    assert(!unlink(path));
}

static uint16_t script_pixels[EP_WIDTH * EP_HEIGHT];
static uint32_t raw_ticks;
static int frames, control_fixture;

void device_present(const uint16_t *pixels) {
    memcpy(script_pixels, pixels, sizeof script_pixels);
    frames++;
}
int device_key_held(int key) {
    if (!control_fixture) return 0;
    return (key == RIGHT && frames == 0) || (key == ESC && frames >= 2);
}
uint32_t device_ticks(void) { return raw_ticks; }
static int timer_stalled;
int device_delay(unsigned ms) {
    if (timer_stalled) return 0;
    raw_ticks += (ms * EP_TICKS_PER_SECOND + 500) / 1000;
    return 1;
}

static void graphics_checks(const char *root, void *stack) {
    char source[SOURCE_CAP + 1], path[PATH_CAP + NAME_CAP];
    FILE *fixture = fopen("tests/Run012_Graphics_Test.py.tns", "rb");
    assert(fixture);
    size_t len = fread(source, 1, sizeof source, fixture);
    assert(len < sizeof source && !ferror(fixture) && !fclose(fixture));
    source[len] = 0;
    const char *name = "Run012_Graphics_Test.py.tns";
    write_file(root,name,source);
    app_init(root,stack);
    assert(load_script(name));
    app.page=EDITOR;
    control_fixture=1;
    for(int i=0;i<2;i++){
        frames=0;
        app_key(RUN);
        assert(app.page==OUTPUT && !app.error && frames==2);
        assert(output_has("Frames: 2"));
        assert(script_pixels[109*EP_WIDTH+149] != script_pixels[109*EP_WIDTH+148]);
        app_key(ESC);
        assert(app.page==EDITOR);
    }
    preview("graphics_return");
    FILE *ppm = fopen("build/graphics.ppm","wb");
    assert(ppm);
    fprintf(ppm,"P6\n%d %d\n255\n",EP_WIDTH,EP_HEIGHT);
    for(size_t i=0;i<EP_WIDTH*EP_HEIGHT;i++){
        unsigned c = script_pixels[i];
        fputc(((c>>11)&31)*255/31,ppm);
        fputc(((c>>5)&63)*255/63,ppm);
        fputc((c&31)*255/31,ppm);
    }
    assert(!fclose(ppm));
    control_fixture=0;
    raw_ticks=UINT32_MAX-20;
    runtime_start(stack);
    assert(runtime_exec("<graphics>",
        "import display as d, time, keys, kandinsky, gint, ion\n"
        "assert d.get_pixel(0,0)==(255,255,255)\n"
        "assert d.get_pixel(-1,0) is None\n"
        "d.fill_rect(-3,-2,4,3,(255,0,0))\n"
        "assert d.get_pixel(0,0)==(255,0,0)\n"
        "assert d.get_pixel(1,0)==(255,255,255)\n"
        "assert not ion.keydown(keys.KEY_UP)\n"
        "gint.dpixel(1,0,0x07e0)\n"
        "assert d.get_pixel(1,0)==(0,255,0)\n"
        "start=time.ticks_ms()\n"
        "time.sleep_ms(15)\n"
        "assert 14 <= time.ticks_diff(time.ticks_ms(),start) <= 16\n"
        "assert time.ticks_diff(2,1073741823)==3\n"
        "assert time.ticks_diff(1073741823,2)==-3",0));
    timer_stalled=1;
    assert(!runtime_exec("<graphics>", "time.sleep_ms(18)", 0));
    assert(output_has("calculator timer stopped"));
    timer_stalled=0;
    assert(runtime_exec("<graphics>", "time.sleep_ms(18)", 0));
    runtime_end();
    snprintf(path,sizeof path,"%s/%s",root,name);
    assert(!unlink(path));

    fixture = fopen("tests/Run012_Frame_Probe.py.tns", "rb");
    assert(fixture);
    len = fread(source, 1, sizeof source, fixture);
    assert(len < sizeof source && !ferror(fixture) && !fclose(fixture));
    source[len] = 0;
    frames = 0;
    runtime_start(stack);
    assert(runtime_exec("<frame-probe>", source, 0));
    runtime_end();
    assert(frames == 1 && output_has("Frame returned") && output_has("Delay returned:"));
    puts("PASS: canvas, held arrows and release, Esc and rerun, compatibility adapters and wrap-safe timing.");
}

static void pygame_checks(const char *root, void *stack) {
    char source[SOURCE_CAP + 1], path[PATH_CAP + NAME_CAP];
    const char *name = "Run014_Pygame_Test.py.tns";
    FILE *f = fopen("tests/Run014_Pygame_Test.py.tns", "rb");
    assert(f);
    size_t n = fread(source, 1, sizeof source, f);
    assert(n < sizeof source && !ferror(f) && !fclose(f));
    source[n] = 0;
    write_file(root, name, source);
    app_init(root, stack);
    assert(load_script(name));
    app.page = EDITOR;
    frames = 0;
    control_fixture = 1;
    app_key(RUN);
    control_fixture = 0;
    if (app.error) {
        for (int i = 0; i < app.rows; i++) puts(app.output[(app.first + i) % OUT_LINES]);
        fflush(stdout);
    }
    assert(app.page == OUTPUT && !app.error && frames == 2 && output_has("Pygame frames: 2"));
    preview("pygame_exit");
    f = fopen("build/pygame.ppm", "wb");
    assert(f);
    fprintf(f, "P6\n%d %d\n255\n", EP_WIDTH, EP_HEIGHT);
    for (int i = 0; i < EP_WIDTH * EP_HEIGHT; i++) {
        unsigned c = script_pixels[i];
        fputc(((c >> 11) & 31) * 255 / 31, f);
        fputc(((c >> 5) & 63) * 255 / 63, f);
        fputc((c & 31) * 255 / 31, f);
    }
    assert(!fclose(f));
    runtime_start(stack);
    assert(runtime_exec("<pygame>",
        "import pygame as p\n"
        "screen=p.display.set_mode()\n"
        "screen.fill('blue')\n"
        "s=p.Surface((4,3))\n"
        "s.fill('magenta')\n"
        "s.set_colorkey('magenta')\n"
        "s.set_at((1,1),'red')\n"
        "assert screen.blit(s,(-1,3))==p.Rect(0,3,3,3)\n"
        "assert screen.get_at((0,4))==p.Color('red')\n"
        "assert screen.get_at((1,4))==p.Color('blue')\n"
        "s.blit(s,(2,1),(1,1,2,1))\n"
        "assert s.get_at((2,1))==p.Color('red')\n"
        "a=p.sprite.Sprite()\n"
        "a.rect=p.Rect(0,0,6,6)\n"
        "b=p.sprite.Sprite()\n"
        "b.rect=p.Rect(4,4,4,4)\n"
        "g=p.sprite.Group(b)\n"
        "assert p.sprite.spritecollide(a,g,True)==[b] and not b.alive()\n"
        "p.event.post(p.event.Event(p.KEYUP,key=p.K_RIGHT))\n"
        "assert p.event.poll().key==p.K_RIGHT\n"
        "assert p.time.Clock().tick(30)>=33", 0));
    runtime_end();
    snprintf(path, sizeof path, "%s/%s", root, name);
    assert(!unlink(path));
    puts("PASS: Pygame surfaces, clipping, color-key blit, overlap, sprites, events and clock.");
}

static void assets_checks(const char *root, void *stack) {
    char path[PATH_CAP + NAME_CAP], source[SOURCE_CAP + 1];
    unsigned char bytes[256];
    FILE *f = fopen("build/Run015_Badge.bmp.tns", "rb");
    assert(f);
    snprintf(path,sizeof path,"%s/Run015_Badge.bmp.tns",root);
    FILE *dest=fopen(path,"wb");
    assert(dest);
    size_t n;
    while((n=fread(bytes,1,sizeof bytes,f))>0)assert(fwrite(bytes,1,n,dest)==n);
    assert(!ferror(f) && !fclose(f) && !fclose(dest));
    f=fopen("tests/Run015_Assets_Test.py.tns","rb");
    assert(f);
    n=fread(source,1,sizeof source,f);
    assert(n<sizeof source && !ferror(f) && !fclose(f));
    source[n]=0;
    app_init(root,stack);
    runtime_start(stack);
    frames=0;
    if(!runtime_exec("<assets>",source,0)){
        for(int i=0;i<app.rows;i++)fprintf(stderr,"%s\n",app.output[(app.first+i)%OUT_LINES]);
        assert(0);
    }
    assert(frames==1 && output_has("Bitmap fonts rendered"));
    assert(script_pixels[101*EP_WIDTH+160]!=script_pixels[78*EP_WIDTH+137]);
    f=fopen("build/pygame_assets.ppm","wb");
    assert(f);
    fprintf(f,"P6\n%d %d\n255\n",EP_WIDTH,EP_HEIGHT);
    for(int i=0;i<EP_WIDTH*EP_HEIGHT;i++){
        unsigned c=script_pixels[i];
        fputc(((c>>11)&31)*255/31,f);
        fputc(((c>>5)&63)*255/63,f);
        fputc((c&31)*255/31,f);
    }
    assert(!fclose(f));
    assert(runtime_exec("<assets-checks>",
        "from io import BytesIO\n"
        "import pygame as p\n"
        "ppm=p.image.load(BytesIO(b'P6\\n# test\\n1 1\\n255\\n \\n\\xff'))\n"
        "assert ppm.get_at((0,0)).b==255 and ppm.get_at((0,0)).r>0\n"
        "assert p.image.load(BytesIO(b'P6\\n1 1\\n15\\n\\x0f\\x00\\x00')).get_at((0,0))==p.Color('red')\n"
        "f=p.font.Font(None,14)\n"
        "assert f.render('é',False,'white').get_size()==f.size('é')\n"
        "assert f.render('I',True,'white','blue').get_colorkey() is None\n"
        "assert f.render('I',False,'black').get_colorkey()==p.Color('white')",0));
    assert(!runtime_exec("<truncated>","p.image.load(BytesIO(b'P6\\n1 1\\n255\\n\\x00'))",0));
    assert(!runtime_exec("<oversized>","p.image.load(BytesIO(b'P6\\n1000 1000\\n255\\n'))",0));
    assert(!runtime_exec("<truncated-bmp>","p.image.load(BytesIO(b'BM'))",0));
    runtime_end();
    assert(!unlink(path));
    puts("PASS: Pygame loaded a BMP from project storage, rendered fonts, preserved PPM bytes, and rejected malformed images.");
}

static void file_manager_checks(const char *home, void *stack) {
    char folder[PATH_CAP], path[PATH_CAP + NAME_CAP];
    snprintf(folder,sizeof folder,"%s/Examples",home);
    assert(!mkdir(folder,0700));
    write_file(home,"Game_Demo.py.tns","print('ready')\n");
    write_file(home,"Matrix_Test.py.tns","print(32)\n");
    app_init(home,stack);
    choose_command(NEW_TEXT);
    type("Notes");
    app_key(ENTER);
    assert(app.page==EDITOR && !strcmp(app.doc.name,"Notes.txt.tns"));
    type("Plain text stays in the editor.");
    app_key(SAVE);
    app_key(RUN);
    assert(app.page==EDITOR && strstr(app.status,"Python script"));
    app_key(ESC);
    assert(app.page==FILES && app.entries[0].folder);
    app_key(DOWN);
    preview("file_manager");
    app_key(MENU);
    app_key(DOWN);
    preview("file_menu");
    app_key(ESC);
    app_key(ENTER);
    assert(app.page==OUTPUT && output_has("ready"));
    app_key(ESC);app_key(ESC);
    for(int i=0;i<app.count;i++){
        if(!strcmp(app.entries[i].name,"Notes.txt.tns"))app.selected=i;
    }
    app_key(ENTER);
    assert(app.page==EDITOR && !strcmp(app.doc.text,"Plain text stays in the editor."));
    app_key(ESC);
    app.selected=0;
    app_key(RIGHT);
    assert(!strcmp(app.root,folder));
    app_key(ESC);
    assert(!strcmp(app.root,home));
    assert(!rmdir(folder));
    const char *files[]={"Game_Demo.py.tns","Matrix_Test.py.tns","Notes.txt.tns"};
    for(int i=0;i<3;i++){
        snprintf(path,sizeof path,"%s/%s",home,files[i]);
        assert(!unlink(path));
    }
    puts("PASS: Files-first startup, open and run, text create and save, and folder navigation.");
}

static void assert_enter(const char *name,const char *before,const char *after){
    memset(&app.doc,0,sizeof app.doc);
    strcpy(app.doc.name,name);
    strcpy(app.doc.text,before);
    app.doc.cursor=(int)strlen(before);
    app.page=EDITOR;
    edit_reset_history();
    app_key(ENTER);
    assert(!strcmp(app.doc.text,after));
    app_key(UNDO);
    assert(!strcmp(app.doc.text,before));
}

static void indentation_checks(const char *root,void *stack){
    app_init(root,stack);
    const char *python="Loop_Demo.py.tns";
    assert_enter(python,"for i in range(1, 10):","for i in range(1, 10):\n  ");
    assert_enter(python,"def total():\n  if True: # comment","def total():\n  if True: # comment\n    ");
    assert_enter(python,"def total(\n    values\n):","def total(\n    values\n):\n  ");
    assert_enter(python,"  items = {'one':","  items = {'one':\n  ");
    assert_enter(python,"  text = 'colon:'","  text = 'colon:'\n  ");
    assert_enter(python,"  # if True:","  # if True:\n  ");
    assert_enter(python,"if True: print(4)","if True: print(4)\n");
    assert_enter(python,"text = \"\"\"\nif True:","text = \"\"\"\nif True:\n");
    assert_enter("Notes.txt.tns","for i in range(1, 10):","for i in range(1, 10):\n");
    assert_enter(python,"\tif True:","\tif True:\n\t  ");
    memset(&app.doc,0,sizeof app.doc);
    strcpy(app.doc.name,python);
    app.page=EDITOR;
    edit_reset_history();
    type("def print_numbers():");app_key(ENTER);
    type("for i in range(1, 10):");app_key(ENTER);
    type("print(i)");app_key(ENTER);
    assert(!strcmp(app.doc.text,"def print_numbers():\n  for i in range(1, 10):\n    print(i)\n    "));
    preview("workspace");
    app_key(MENU);preview("workspace_tools");app_key(ESC);
    for(int i=0;i<4;i++)app_key(8);
    type("print_numbers()");
    app_key(RUN);
    assert(app.page==OUTPUT && !app.error && output_has("9"));
    preview("workspace_run");
    char path[PATH_CAP+NAME_CAP];
    snprintf(path,sizeof path,"%s/%s",root,python);
    assert(!unlink(path));
    puts("PASS: Python block indentation, comments, strings, brackets, plain text, undo and run.");
}

int main(void){
    int stack;
    char root[]="/tmp/endrapython-XXXXXX";assert(mkdtemp(root));
    char scripts[150];snprintf(scripts,sizeof scripts,"%s/EndraPython",root);
    app_init(scripts,&stack);assert(!app.count && access(scripts,F_OK));
    choose_command(NEW_SCRIPT);assert(app.page==NAME && access(scripts,F_OK));
    type("Hello_World");app_key(ENTER);assert(app.page==EDITOR && app.doc.existing && !app.doc.text[0]);
    type("print('Hello from EndraPython')");app_key(ENTER);type("print(sum(range(10)))");app_key(ENTER);app_key(SAVE);
    app.doc.cursor=0;
    assert(!strcmp(app.doc.name,"Hello_World.py.tns"));
    app_key(RUN);assert(app.page==OUTPUT && !app.error && output_has("Hello from EndraPython") && output_has("45"));
    preview("console");app_key(ESC);app_key(ESC);assert(app.count==1);
    choose_command(NEW_SCRIPT);type("Hello_World");app_key(ENTER);assert(app.page==NAME && strstr(app.status,"in use"));app_key(ESC);
    app_key(RIGHT);assert(app.page==EDITOR);
    assert(app.doc.cursor==0);app_key(RIGHT);app_key(RIGHT);preview("editor");
    app_key(LEFT);app_key(LEFT);type("#");app_key(RUN);assert(!app.error&&!output_has("Hello from EndraPython")&&output_has("45"));
    app_key(ESC);assert(app.doc.cursor==1);app_key(8);app_key(SAVE);app_key(ESC);preview("files");
    choose_command(NEW_SCRIPT);type("Broken");app_key(ENTER);type("print(42)");app_key(ENTER);type("if :");app_key(RUN);
    assert(app.error && output_has("SyntaxError") && output_has("line 2"));preview("error");
    app_key(ESC);app_key(8);app_key(ESC);assert(app.page==DISCARD);app_key(ESC);assert(app.page==EDITOR);
    app_key(ESC);app_key(ENTER);assert(app.page==FILES);
    choose_command(OPEN_SHELL);assert(app.page==SHELL);
    type("x = 7");app_key(ENTER);type("sum(range(x))");app_key(ENTER);
    assert(output_has("21"));app_key(UP);assert(!strcmp(app.command,"sum(range(x))"));preview("repl");
    assert(runtime_exec("<test>","import gc, math, collections\nprint(math.sqrt(81), 2**40)",0));
    assert(output_has("1099511627776"));
    assert(!runtime_exec("<test>","a = [0] * 2000000",0));assert(output_has("MemoryError"));
    cancel=1;assert(!runtime_exec("<test>","while True:\n pass\n",0));cancel=0;assert(output_has("KeyboardInterrupt"));
    assert(runtime_exec("<test>","print(6*7)",0));assert(output_has("42"));
    assert(runtime_exec("<test>","import os",0));
    assert(runtime_exec("<test>","import json, io",0));
    assert(runtime_exec("<test>","import time",0));
    app_key(ESC);assert(app.page==FILES);
    for(int i=0;i<24;i++){assert(load_script("Hello_World.py.tns"));app.page=EDITOR;app_key(RUN);assert(!app.error && output_has("45"));app_key(ESC);}
    assert(!valid_script("../x.py")&&!valid_script("x/abc.py")&&!valid_script("system.tns"));
    write_file(scripts,"foreign.py","print('\xc3\xa9')\n");assert(load_script("foreign.py"));
    write_file(scripts,"foreign.py","\xc0\xaf");assert(!load_script("foreign.py"));
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
    assert(app.doc.top==101-EDIT_ROWS && app.doc.left==81-EDIT_COLS);edit_key(&app.doc,UP);assert(app.doc.left==1);
    app.page=FILES;app_key(ESC);assert(app.quit);
    const char *names[]={"Hello_World.py.tns","Broken.py.tns","Windows.py","foreign.py","large.py"};
    for(int i=1;i<5;i++){char path[256];snprintf(path,sizeof path,"%s/%s",scripts,names[i]);assert(!unlink(path));}
    file_manager_checks(scripts,&stack);
    pygame_checks(scripts,&stack);
    assets_checks(scripts,&stack);
    indentation_checks(scripts,&stack);
    graphics_checks(scripts,&stack);
    numerics_checks(scripts,&stack);
    workbench_checks(root,&stack);
    storage_io_checks(root,&stack);
    project_import_checks(root,&stack);
    catalog_checks(scripts,&stack);
    ide_checks(scripts,&stack);
    browse_checks(scripts,&stack);
    char hello[256];snprintf(hello,sizeof hello,"%s/%s",scripts,names[0]);assert(!unlink(hello));
    assert(!rmdir(scripts)&&!rmdir(root));
    test_save_folder(&stack);
    puts("PASS: script lifecycle, edit/run, console, syntax locations, REPL, modules, heap exhaustion, interrupt, 24 reruns, storage failures, file boundaries, 100-line navigation and 35 framebuffer previews, two-space stops, contextual help, command catalog and input, punctuation picker, UTF-16/UTF-8, nested browse/edit/run, clipboard, find, undo/redo, error jumps and module discovery.");
}
