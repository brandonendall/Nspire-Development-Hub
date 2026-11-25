/* Ndless physical QWERTY mapping, adapted from the project's working
 * UltimateCommander nx_platform.c. Separate from proven LCD/timer code.
 */
#include "text_input.h"
#include "app.h"
#include "device.h"
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
unsigned eh16_script_text_keys(int *keys){
    static const struct { t_key physical; int code; } other[] = {
        {KEY_NSPIRE_SHIFT, EP_KEY_SHIFT}, {KEY_NSPIRE_CTRL, EP_KEY_CTRL},
        {KEY_NSPIRE_SPACE, ' '}, {KEY_NSPIRE_PERIOD, '.'},
        {KEY_NSPIRE_MINUS, '-'}, {KEY_NSPIRE_PLUS, '+'},
        {KEY_NSPIRE_COMMA, ','}, {KEY_NSPIRE_APOSTROPHE, 39},
        {KEY_NSPIRE_DEL, 8}, {KEY_NSPIRE_LP, '('}, {KEY_NSPIRE_RP, ')'},
        {KEY_NSPIRE_MULTIPLY, '*'}, {KEY_NSPIRE_DIVIDE, '/'}, {KEY_NSPIRE_EQU, '='},
    };
    unsigned count = 0;
    for(int i=0;i<26;i++)if(isKeyPressed(letters[i]))keys[count++]='a'+i;
    for(int i=0;i<10;i++)if(isKeyPressed(digits[i]))keys[count++]='0'+i;
    for(unsigned i=0;i<sizeof other/sizeof other[0];i++)
        if(isKeyPressed(other[i].physical))keys[count++]=other[i].code;
    return count;
}
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
 if(!scan&&isKeyPressed(KEY_NSPIRE_COMMA)){scan=41;ascii=shift?':':',';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_APOSTROPHE)){scan=42;ascii=shift?'"':'\'';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_DEL)){scan=43;ascii=8;}
 if(!scan&&isKeyPressed(KEY_NSPIRE_LP)){scan=44;ascii=shift?'[':'(';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_RP)){scan=45;ascii=shift?']':')';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_MULTIPLY)){scan=46;ascii='*';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_DIVIDE)){scan=47;ascii=shift?92:'/';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_EQU)){scan=48;ascii='=';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_TAB)){scan=49;ascii=INDENT;}
 if(!scan&&isKeyPressed(KEY_NSPIRE_RET)){scan=50;ascii=ENTER;}
 if(!scan&&isKeyPressed(KEY_NSPIRE_COLON)){scan=51;ascii=':';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_QUOTE)){scan=52;ascii='"';}
 if(!scan&&isKeyPressed(KEY_NSPIRE_CAT)){scan=53;ascii=CATALOG_KEY;}
 if(!scan&&isKeyPressed(KEY_NSPIRE_QUESEXCL)){scan=54;ascii=MARKS;}
 if(!scan&&isKeyPressed(KEY_NSPIRE_PI)){scan=55;ascii=TI_INPUT;}
 if(!scan){last_scan=0;return 0;}
 if(last_scan==scan)return 0;
 last_scan=scan;
 return ascii;
}

int eh16_key_event(int k,int ch,int ctrl){
    static int ctrl_was_down,ctrl_latched;
    if(ctrl && !ctrl_was_down)ctrl_latched=1;
    ctrl_was_down=ctrl;
    int modified=ctrl || ctrl_latched;
    if(k || ch)ctrl_latched=0;
    if(!k){
        if(ch==CATALOG_KEY)k=modified?0:CATALOG_KEY;
        else if(ch==TI_INPUT || ch==MARKS)k=ch;
        else if(modified)
            switch(ch){
            case '=': k=TI_INPUT; break;
            case '(': k='['; break; case ')': k=']'; break;
            case 39: k=34; break;
            case 's': k=SAVE; break; case 'r': k=RUN; break;
            case 'f': k=SEARCH; break; case 'z': k=UNDO; break;
            case 'y': k=REDO; break; case 'a': k=SELECT_ALL; break;
            case 'c': k=COPY; break; case 'x': k=CUT; break;
            case 'v': k=PASTE; break; default: k=0; break;
            }
        else k=ch;
    }
    return k;
}
