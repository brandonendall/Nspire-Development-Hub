#include "image_viewer.h"
#include <libndls.h>
int eh18_zoomkey(void){
 static int last=0;
 int now=0;
 if(isKeyPressed(KEY_NSPIRE_MULTIPLY)||isKeyPressed(KEY_NSPIRE_PLUS))now=1;
 else if(isKeyPressed(KEY_NSPIRE_DIVIDE)||isKeyPressed(KEY_NSPIRE_MINUS))now=-1;
 if(now==last)return 0;
 last=now;
 return now;
}
