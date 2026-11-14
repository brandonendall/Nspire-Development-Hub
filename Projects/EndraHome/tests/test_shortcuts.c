#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "shortcuts.h"
int main(void){
    EhShortcut s[EH_SLOTS];eh_defaults(s);
    assert(!strcmp(s[0].name,"EndraGBA"));
    assert(!s[0].path[0]);
    assert(eh_safe_path("/documents/Apps/GBA.tns"));
    assert(!eh_safe_path("/root/a.tns"));
    assert(!eh_safe_path("/documents/../etc/a.tns"));
    assert(!eh_safe_path("/documents//a.tns"));
    assert(!eh_safe_path("/documents/a.txt"));
    assert(!eh_parse_line(s,"4=X|/documents/X.tns"));
    assert(!eh_parse_line(s,"1=X|/documents/../X.tns"));
    assert(eh_parse_line(s,"0=GBA|/documents/Apps/GBA.tns\r\n"));
    assert(!strcmp(s[0].name,"GBA"));
    assert(!strcmp(s[0].path,"/documents/Apps/GBA.tns"));
    assert(eh_parse_line(s,"1=Files|"));
    assert(!s[1].path[0]);
    assert(!eh_parse_line(s,"# comment"));
    assert(!eh_parse_line(s,"1=bad"));
    assert(!strcmp(s[2].name,"ZIP"));
    puts("EndraHome shortcuts: tests passed");
    return 0;
}
