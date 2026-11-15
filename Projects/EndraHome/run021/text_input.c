/* Ndless physical QWERTY mapping, adapted from the project's working
 * UltimateCommander nx_platform.c. Separate from proven LCD/timer code.
 */
#include "text_input.h"
#include <libndls.h>
static const t_key letters[26]={
 KEY_NSPIRE_A,KEY_NSPIRE_B,KEY_NSPIRE_C,KEY_NSPIRE_D,
 KEY_NSPIRE_E,KEY_NSPIRE_F,KEY_NSPIRE_G,KEY_NSPIRE_H,
 KEY_NSPIRE_I,KEY_NSPIRE_J,KEY_NSPIRE_K,KEY_NSPIRE_L,
 KEY_NSPIRE_M,KEY_NSPIRE_N,KEY_NSPIRE_O,KEY_NSPIRE_P,
 KEY_NSPIRE_Q,KEY_NSPIRE_R,KEY_NSPIRE_S,KEY_NSPIRE_T,
 KEY_NSPIRE_U,KEY_NSPIRE_V,KEY_NSPIRE_W,KEY_NSPIRE_X,
 KEY_NSPIRE_Y,KEY_NSPIRE_Z
};
static const t_key digits[10]={
 KEY_NSPIRE_0,KEY_NSPIRE_1,KEY_NSPIRE_2,KEY_NSPIRE_3,
 KEY_NSPIRE_4,KEY_NSPIRE_5,KEY_NSPIRE_6,KEY_NSPIRE_7,
 KEY_NSPIRE_8,KEY_NSPIRE_9
};
int eh16_textkey(void){
 static int last_scan=0;
 int scan=0,ascii=0;
 int shift=isKeyPressed(KEY_NSPIRE_SHIFT);
 /* Return one edge per physical keypress. Shift modifies the character
  * but never produces a keypress by itself. */
 for(int i=0;i<26;i++)if(isKeyPressed(letters[i])){
  scan=1+i;ascii=(shift?'A':'a')+i;break;
 }
 if(!scan)for(int i=0;i<10;i++)if(isKeyPressed(digits[i])){
  scan=27+i;ascii='0'+i;break;
 }
 if(!scan&&isKeyPressed(KEY_NSPIRE_SPACE)){scan=37;ascii=' ';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_PERIOD)){scan=38;ascii='.';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_MINUS)){scan=39;ascii=shift?'_':'-';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_PLUS)){scan=40;ascii='+';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_COMMA)){scan=41;ascii=',';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_APOSTROPHE)){scan=42;ascii='\'';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_DEL)){scan=43;ascii=8;}
 if(!scan){last_scan=0;return 0;}
 if(last_scan==scan)return 0;
 last_scan=scan;
 return ascii;
}
