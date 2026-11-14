#include "caption.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
int main(void){
 char a[7],b[7];
 eh8_caption("EndraGBA",a,b);assert(!strcmp(a,"GBA")&&!b[0]);
 eh8_caption("EndraZIP",a,b);assert(!strcmp(a,"ZIP")&&!b[0]);
 eh8_caption("Favorites",a,b);assert(!strcmp(a,"Favs")&&!b[0]);
 eh8_caption("Files",a,b);assert(!strcmp(a,"Files")&&!b[0]);
 eh8_caption("Some App",a,b);assert(!strcmp(a,"Some")&&!strcmp(b,"App"));
 eh8_caption("ReallyLongApplicationName",a,b);assert(strlen(a)<=6&&strlen(b)<=6&&b[5]=='.');
 eh8_caption("My_Photos",a,b);assert(!strcmp(a,"My")&&!strcmp(b,"Photos"));
 eh8_caption("",a,b);assert(!strcmp(a,"App"));
 puts("Run011 caption layout tests passed");
 return 0;
}
