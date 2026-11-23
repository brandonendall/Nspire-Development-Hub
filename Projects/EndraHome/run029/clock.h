#ifndef ENDRAHOME_CLOCK_H
#define ENDRAHOME_CLOCK_H
#include "toast.h"
typedef struct {
 uint32_t last_rtc, anchor;
 unsigned second;
 int started, seen_rtc;
} EhClock;

static inline unsigned eh_clock_sample(EhClock *clock,uint32_t rtc,uint32_t ticks){
 if(!clock->started){
  clock->anchor=ticks;
  clock->started=1;
 }
 if(rtc!=UINT32_MAX && (!clock->seen_rtc || rtc!=clock->last_rtc)){
  clock->seen_rtc=1;
  clock->last_rtc=rtc;
  clock->second=rtc%86400u;
  clock->anchor=ticks;
 }else{
  unsigned elapsed=(uint32_t)(ticks-clock->anchor)/EH_TICKS_PER_SECOND;
  clock->anchor+=elapsed*EH_TICKS_PER_SECOND;
  clock->second=(clock->second+elapsed)%86400u;
 }
 return clock->second;
}
#endif
