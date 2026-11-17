#include "file_ops.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
int main(void){
 char base[]="/tmp/eh25XXXXXX";assert(mkdtemp(base));
 char a[512],b[512],c[512],f[512],g[512];
 snprintf(a,sizeof(a),"%s/alpha",base);snprintf(b,sizeof(b),"%s/beta",base);
 snprintf(c,sizeof(c),"%s/gamma",base);assert(!mkdir(a,0700));
 snprintf(f,sizeof(f),"%s/large.bin",a);
 FILE *w=fopen(f,"wb");assert(w);
 char buf[8192];memset(buf,0x5a,sizeof(buf));
 for(int i=0;i<4224;i++)assert(fwrite(buf,1,sizeof(buf),w)==sizeof(buf));
 assert(!fclose(w)); /* 33 MiB streamed without 33 MiB RAM */
 assert(!eh25_copy(a,b));assert(eh25_copy(a,b));
 assert(eh25_copy(a,f));assert(eh25_copy(a,a));
 snprintf(g,sizeof(g),"%s/large.bin",b);
 struct stat st;assert(!stat(g,&st)&&st.st_size==34603008);
 assert(!eh25_move(b,c));assert(access(b,F_OK));
 assert(!eh25_remove(c));assert(!eh25_remove(a));assert(!rmdir(base));
 puts("Run025 folder copy/move/delete and 33 MiB streaming: PASS");return 0;
}
