#include "nx_platform.h"
#include <stdio.h>
#ifdef _TINSPIRE
#include <libndls.h>
#include <string.h>
extern void platform_trace_event(int key);
/* Stock GNU Arm newlib expects the legacy .fini hook. This C app has
 * no legacy .fini section; Ndless runs its init/fini arrays itself. */
void __attribute__((weak)) _fini(void) {}
int platform_init(void) { return lcd_init(SCR_320x240_565); }
void platform_close(void) { lcd_init(SCR_TYPE_INVALID); refresh_osscr(); }
void platform_present(uint16_t *pixels) { lcd_blit(pixels,SCR_320x240_565); }
int platform_cancel(void) { return isKeyPressed(KEY_NSPIRE_ESC); }
static int scan(void) {
    static const t_key letters[]={KEY_NSPIRE_A,KEY_NSPIRE_B,KEY_NSPIRE_C,KEY_NSPIRE_D,KEY_NSPIRE_E,KEY_NSPIRE_F,KEY_NSPIRE_G,KEY_NSPIRE_H,KEY_NSPIRE_I,KEY_NSPIRE_J,KEY_NSPIRE_K,KEY_NSPIRE_L,KEY_NSPIRE_M,KEY_NSPIRE_N,KEY_NSPIRE_O,KEY_NSPIRE_P,KEY_NSPIRE_Q,KEY_NSPIRE_R,KEY_NSPIRE_S,KEY_NSPIRE_T,KEY_NSPIRE_U,KEY_NSPIRE_V,KEY_NSPIRE_W,KEY_NSPIRE_X,KEY_NSPIRE_Y,KEY_NSPIRE_Z};
    static const t_key digits[]={KEY_NSPIRE_0,KEY_NSPIRE_1,KEY_NSPIRE_2,KEY_NSPIRE_3,KEY_NSPIRE_4,KEY_NSPIRE_5,KEY_NSPIRE_6,KEY_NSPIRE_7,KEY_NSPIRE_8,KEY_NSPIRE_9};
    if(isKeyPressed(KEY_NSPIRE_ESC))return K_ESC;
    if(isKeyPressed(KEY_NSPIRE_ENTER)||isKeyPressed(KEY_NSPIRE_RET)||isKeyPressed(KEY_NSPIRE_CLICK))return K_ENTER;
    if(isKeyPressed(KEY_NSPIRE_UP))return K_UP;
    if(isKeyPressed(KEY_NSPIRE_DOWN))return K_DOWN;
    if(isKeyPressed(KEY_NSPIRE_LEFT))return K_LEFT;
    if(isKeyPressed(KEY_NSPIRE_RIGHT))return K_RIGHT;
    if(isKeyPressed(KEY_NSPIRE_TAB))return K_TAB;
    if(isKeyPressed(KEY_NSPIRE_DEL))return K_DELETE;
    if(isKeyPressed(KEY_NSPIRE_MENU))return K_MENU;
    for(int i=0;i<26;++i)if(isKeyPressed(letters[i]))return (isKeyPressed(KEY_NSPIRE_SHIFT)?'A':'a')+i;
    for(int i=0;i<10;++i)if(isKeyPressed(digits[i]))return '0'+i;
    if(isKeyPressed(KEY_NSPIRE_SPACE))return ' ';
    if(isKeyPressed(KEY_NSPIRE_PERIOD))return '.';
    if(isKeyPressed(KEY_NSPIRE_MINUS))return isKeyPressed(KEY_NSPIRE_SHIFT)?'_':'-';
    if(isKeyPressed(KEY_NSPIRE_DIVIDE))return '/';
    if(isKeyPressed(KEY_NSPIRE_PLUS))return '+';
    if(isKeyPressed(KEY_NSPIRE_LP))return '(';
    if(isKeyPressed(KEY_NSPIRE_RP))return ')';
    if(isKeyPressed(KEY_NSPIRE_COMMA))return ',';
    if(isKeyPressed(KEY_NSPIRE_APOSTROPHE))return '\'';
    return 0;
}
/* Run004 diagnostics: yield to the UI every ~1.5 s so an idle heartbeat
 * distinguishes input polling from an application/OS lock. Keep key-release
 * state across those yields; the timer never synthesizes a key press. */
int platform_key(void) {
    static int old=0,held=0;
    unsigned ticks=0;
    for(;;){
        int k=scan();
        if(!k){old=held=0;}
        else if(k!=old){old=k;held=0;platform_trace_event(k);return k;}
        else if(++held>=18&&(k==K_UP||k==K_DOWN||k==K_DELETE)){
            held=14;platform_trace_event(k);return k;
        }
        if(++ticks>=75){platform_trace_event(0);return 0;}
        msleep(20);
    }
}
int platform_launch(const char *path) {
    char *args[2]={(char*)path,NULL};
    wait_no_key_pressed();lcd_init(SCR_TYPE_INVALID);
    int result=nl_exec(path,1,args);
    lcd_init(SCR_320x240_565);wait_no_key_pressed();return result;
}
void platform_info(char *out,unsigned length) { snprintf(out,length,"EndraCommander NX\nRun 006 | ESC and stack isolation\n\nNdless revision: %u\nHardware subtype: %u\nCX II: %s\nDisplay API: lcd_blit\n\nNative nSonic hook: OFF\nOS 6.4 hook needs validation.\n\nNo CPU overclock or resident hook.",(unsigned)nl_ndless_rev(),(unsigned)nl_hwsubtype(),is_cx2?"yes":"no"); }
#else
int platform_init(void){return 1;}
void platform_close(void){}
void platform_present(uint16_t *pixels){(void)pixels;}
int platform_key(void){return K_ESC;}
int platform_cancel(void){return 0;}
int platform_launch(const char *path){(void)path;return -1;}
void platform_info(char *out,unsigned length){snprintf(out,length,"Host preview only; no calculator execution.");}
#endif
