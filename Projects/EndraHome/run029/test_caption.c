#include "caption.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char first[EH7_LABEL_CAP], second[EH7_LABEL_CAP];
    eh_caption("EndraGBA",8,first,second);
    assert(!strcmp(first,"EndraGBA") && !second[0]);
    eh_caption("Folder Pictures",8,first,second);
    assert(!strcmp(first,"Folder") && !strcmp(second,"Pictures"));
    eh_caption("123456789012345",8,first,second);
    assert(!strcmp(first,"12345678") && !strcmp(second,"9012345"));
    eh_caption("Long Longlonglong",5,first,second);
    assert(!strcmp(first,"Long") && !strcmp(second,"Lo..."));
    eh_caption("",8,first,second);
    assert(!strcmp(first,"App"));
    puts("PASS full-label wrapping within tile bounds");
    return 0;
}
