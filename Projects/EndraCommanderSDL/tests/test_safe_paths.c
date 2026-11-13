#include "safe_paths.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    char buf[ES_PATH_MAX]="/documents";
    assert(es_path_allowed("/documents"));
    assert(es_path_allowed("/documents/GBA"));
    assert(!es_path_allowed("/"));
    assert(!es_path_allowed("/documents/"));
    assert(!es_path_allowed("/documents/.."));
    assert(!es_path_allowed("/documents/./other"));
    assert(!es_path_allowed("/documents/abc/../../system"));
    assert(!es_path_allowed("/documents//test"));
    assert(!es_path_allowed("/documents2"));
    assert(!es_name_allowed(".."));
    assert(!es_name_allowed("bad/name"));
    assert(!es_name_allowed("bad\\name"));
    assert(es_path_join(buf,sizeof(buf),buf,"GBA"));
    assert(!strcmp(buf,"/documents/GBA"));
    assert(es_path_parent(buf)&&!strcmp(buf,"/documents"));
    assert(!es_path_parent(buf));
    assert(!es_path_join(buf,sizeof(buf),buf,".."));
    assert(!es_path_join(buf,sizeof(buf),"/","forbidden"));
    puts("Endra SDL Run001: safe paths PASS");
    return 0;
}
