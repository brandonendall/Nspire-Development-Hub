#include "edit.h"
#include <string.h>
#include <errno.h>
static int reject(Eh14SaveError *e){if(e){e->stage=EH14_INVALID;e->os_error=EINVAL;}return 0;}
int eh15_update_shortcut(EhRegistry *apps,int slot,const char *label,const char *path,
                         const char *cfg,Eh14SaveError *err){
 if(!apps||slot<0||slot>=EH7_SLOT_COUNT||!cfg||!cfg[0])return reject(err);
 EhRegistry next=*apps;
 if(label){
  size_t n=strlen(label);
  if(!n||n>=EH7_LABEL_CAP)return reject(err);
  for(size_t i=0;i<n;i++){
   unsigned char c=(unsigned char)label[i];
   if(c<32||c>126||c=='|')return reject(err);
  }
  strcpy(next.slots[slot].label,label);
 }
 if(path&&path[0]&&!eh7_valid_path(path))return reject(err);
 /* Use an independent buffer when renaming: strcpy() in the transactional
  * save routine must never read/write the exact same backing array. */
 char value[EH7_PATH_CAP];
 if(path)strcpy(value,path);
 else strcpy(value,next.slots[slot].path);
 if(!eh14_assign_save(&next,slot,value,cfg,err))return 0;
 *apps=next;return 1;
}
