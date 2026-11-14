/* Companion app for a strictly isolated launch experiment.
 * Run directly once to confirm the OS can display this message. */
#include <libndls.h>
void __attribute__((weak)) _fini(void){}
int main(void){
 show_msgbox("EndraHome Test 006","SUCCESS: the test program started!\nPress Enter to return.");
 return 0;
}
