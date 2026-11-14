#include "caption.h"
#include <string.h>
static void copy6(char out[7],const char *s,size_t n){
 if(n>6)n=6;
 memcpy(out,s,n);
 out[n]=0;
}
void eh8_caption(const char *name,char first[7],char second[7]){
 if(!first||!second)return;
 first[0]=second[0]=0;
 if(!name||!name[0]){strcpy(first,"App");return;}
 /* Full labels remain visible on details screen; these are dock abbreviations. */
 const char *short_name=0;
 if(!strcmp(name,"EndraGBA")) short_name="GBA";
 else if(!strcmp(name,"EndraZIP"))short_name="ZIP";
 else if(!strcmp(name,"EndraCmd"))short_name="Cmd";
 else if(!strcmp(name,"Favorites"))short_name="Favs";
 if(short_name){strcpy(first,short_name);return;}
 size_t n=strlen(name);
 if(n<=6){copy6(first,name,n);return;}
 /* Prefer a natural word break when one fits the 6-character tile. */
 size_t split=0;
 for(size_t i=1;i<=6&&i<n;i++){
  if(name[i]==' '||name[i]=='_'||name[i]=='-')split=i;
 }
 if(split){
  copy6(first,name,split);
  const char *next=name+split;
  while(*next==' '||*next=='_'||*next=='-')next++;
  size_t rem=strlen(next);
  copy6(second,next,rem);
  if(rem>6)second[5]='.';
  return;
 }
 copy6(first,name,6);
 size_t rem=n-6;
 copy6(second,name+6,rem);
 if(rem>6)second[5]='.';
}
