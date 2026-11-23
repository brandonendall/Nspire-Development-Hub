#include "clock.h"
#include <assert.h>
int main(void){
 const uint32_t hz=EH_TICKS_PER_SECOND;
 EhClock clock={0};
 assert(eh_clock_sample(&clock,0,0)==0);
 assert(eh_clock_sample(&clock,1,hz)==1);
 assert(eh_clock_sample(&clock,1,3*hz)==3);
 assert(eh_clock_sample(&clock,UINT32_MAX,4*hz)==4);
 assert(eh_clock_sample(&clock,86399,5*hz)==86399);
 assert(eh_clock_sample(&clock,86400,6*hz)==0);
 EhClock missing={0};
 assert(eh_clock_sample(&missing,UINT32_MAX,UINT32_MAX-hz)==0);
 assert(eh_clock_sample(&missing,UINT32_MAX,hz-1)==2);
 assert(eh_clock_sample(&missing,1325376000u,2*hz)==1325376000u%86400u);
 return 0;
}
