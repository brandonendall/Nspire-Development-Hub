#include "preview.h"
#include "safe_paths.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
    EsPreview p;
    unsigned char binary[]={'c','f','g',0,'x'};
    char test[ES_PREVIEW_BYTES+1];
    assert(es_preview_supported_name("nclock.cfg"));
    assert(es_preview_supported_name("gpsp_qol.cfg.tns"));
    assert(es_preview_supported_name("notes.TXT.TNS"));
    assert(!es_preview_supported_name("rom.gba.tns"));
    assert(!es_preview_supported_name("EndraSDL_Run001.tns"));
    assert(!es_preview_supported_name("../secret.cfg"));
    assert(es_preview_decode((const unsigned char*)"A\nB\n",4,&p));
    assert(p.count==3&&p.bytes==4);
    assert(!strcmp(p.lines[0],"A")&&!strcmp(p.lines[1],"B"));
    assert(es_preview_decode((const unsigned char*)"",0,&p));
    assert(p.count==1&&p.lines[0][0]==0);
    assert(!es_preview_decode(binary,sizeof(binary),&p));
    assert(!es_preview_decode(NULL,1,&p));
    assert(!es_preview_decode((const unsigned char*)"x",ES_PREVIEW_BYTES+1,&p));
    memset(test,'X',sizeof(test));
    assert(es_preview_decode((const unsigned char*)test,ES_PREVIEW_BYTES,&p));
    assert(p.count<=ES_PREVIEW_ROWS);
    assert(p.count>1);
    assert(!es_preview_read("/../etc/passwd",&p));
    puts("Endra SDL 002: bounded text decoding PASS");
    return 0;
}
