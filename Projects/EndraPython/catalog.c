#include "app.h"
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

CatalogEntry catalog_entries[CATALOG_CAP];
int catalog_count,catalog_index,catalog_group;
char catalog_filter[40];
const char *const catalog_groups[]={"Common","builtins","math","random","cmath","array","struct","collections","gc","os","numpy","display","keys","time","casioplot","kandinsky","ion","gint","json","io"};
static const struct {const char *name,*syntax;} common[]={
    {"print","print(value, ...)"},{"input","input([prompt])"},{"open","open(file[, mode])"},{"len","len(obj)"},
    {"range","range(start, stop[, step])"},{"int","int(value)"},{"float","float(value)"},
    {"str","str(value)"},{"list","list(iterable)"},{"dict","dict(...)"},{"sum","sum(iterable)"},
    {"min","min(iterable)"},{"max","max(iterable)"},{"abs","abs(number)"},
    {"type","type(obj)"},{"dir","dir([obj])"},{"help","help([obj])"}
};
static int compare_names(const void *a,const void *b){
    const CatalogEntry *x=a,*y=b;return strcasecmp(x->name,y->name);
}
static void load_group(void){
    catalog_index=0;catalog_filter[0]=0;
    if(catalog_group==0){
        catalog_count=sizeof common/sizeof common[0];
        for(int i=0;i<catalog_count;i++){
            strcpy(catalog_entries[i].name,common[i].name);catalog_entries[i].callable=1;
        }
        return;
    }
    int temporary=app.origin==EDITOR;
    if(temporary)runtime_start(app.stack);
    catalog_count=runtime_catalog(catalog_groups[catalog_group]);
    if(temporary)runtime_end();
    qsort(catalog_entries,catalog_count,sizeof catalog_entries[0],compare_names);
}
void catalog_open(void){
    app.origin=app.page;app.page=CATALOG;catalog_group=0;load_group();
}
void catalog_name(char *out,size_t size){
    const char *name=catalog_entries[catalog_index].name;
    if(catalog_group<2)snprintf(out,size,"%s",name);
    else snprintf(out,size,"%s.%s",catalog_groups[catalog_group],name);
}
const char *catalog_signature(void){
    if(catalog_group<2)for(size_t i=0;i<sizeof common/sizeof common[0];i++)
        if(!strcmp(catalog_entries[catalog_index].name,common[i].name))return common[i].syntax;
    static const struct { const char *name, *signature; } signatures[] = {
        {"display.fill_rect", "display.fill_rect(x,y,w,h,rgb)"},
        {"display.draw_string", "display.draw_string(x,y,text[,rgb])"},
        {"keys.keydown", "keys.keydown(key)"}, {"time.sleep_ms", "time.sleep_ms(ms)"},
        {"time.ticks_diff", "time.ticks_diff(a, b)"},
        {"numpy.array", "numpy.array(value[, dtype])"}, {"numpy.dot", "numpy.dot(a, b)"},
        {"numpy.matmul", "numpy.matmul(a, b)"}, {"numpy.transpose", "numpy.transpose(a)"},
        {"numpy.norm", "numpy.norm(vector)"}, {"numpy.normalize", "numpy.normalize(vector)"},
        {"os.getcwd", "os.getcwd()"}, {"os.chdir", "os.chdir(path)"},
        {"os.listdir", "os.listdir([path])"}, {"os.mkdir", "os.mkdir(path)"},
        {"json.dump", "json.dump(obj, file)"}, {"json.load", "json.load(file)"},
        {"json.dumps", "json.dumps(obj)"}, {"json.loads", "json.loads(text)"},
        {"math.sqrt", "math.sqrt(x)"}, {"math.sin", "math.sin(radians)"},
        {"random.randint", "random.randint(a, b)"}, {"random.choice", "random.choice(seq)"},
        {"struct.pack", "struct.pack(fmt, values...)"}, {"struct.unpack", "struct.unpack(fmt, data)"},
    };
    char name[80];
    catalog_name(name, sizeof name);
    for (size_t i = 0; i < sizeof signatures / sizeof signatures[0]; i++)
        if (!strcmp(name, signatures[i].name)) return signatures[i].signature;
    return NULL;
}
static void find_prefix(void){
    for(int i=0;i<catalog_count;i++)if(!strncasecmp(catalog_entries[i].name,catalog_filter,strlen(catalog_filter))){
        catalog_index=i;return;
    }
    strcpy(app.status,"No matching name");
}
void catalog_key(int k){
    int groups=sizeof catalog_groups/sizeof catalog_groups[0];
    if(k==ESC){app.page=app.origin;return;}
    if(k==LEFT || k==RIGHT){
        catalog_group=(catalog_group+groups+(k==LEFT?-1:1))%groups;load_group();return;
    }
    if (k == INDENT && catalog_group >= 2) {
        char statement[80];
        snprintf(statement, sizeof statement, "import %s", catalog_groups[catalog_group]);
        app.page = app.origin;
        app_insert_text(statement);
        return;
    }
    if(!catalog_count)return;
    size_t n=strlen(catalog_filter);
    if(k==8 && n){catalog_filter[n-1]=0;find_prefix();return;}
    if((k>='a' && k<='z') || (k>='A' && k<='Z') || k=='_'){
        if(n+1<sizeof catalog_filter){catalog_filter[n]=(char)k;catalog_filter[n+1]=0;find_prefix();}
        return;
    }
    if(k==UP || k==DOWN){
        catalog_filter[0]=0;
        catalog_index=(catalog_index+catalog_count+(k==UP?-1:1))%catalog_count;
    }else if(k==ENTER){
        if(n && strncasecmp(catalog_entries[catalog_index].name,catalog_filter,n)){find_prefix();return;}
        char value[80];catalog_name(value,sizeof value);
        if(catalog_entries[catalog_index].callable)strcat(value,"(");
        app.page=app.origin;app_insert_text(value);
    }
}
