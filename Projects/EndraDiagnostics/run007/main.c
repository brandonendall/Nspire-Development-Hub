/*
 * EndraDiagnostics Run007: coarse OS battery monitor + validated charge task.
 * Uses Run005's proven read-only resolver, and preserved Run003 graphics/input.
 * No ADC hardware sampling, charger writes, file modifications or OS calls.
 */
#include "../run002/platform.h"
#include "../run004/vendor/battery_native.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "font8x8_basic.h"

#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
#define MAX_STEPS 2000u /* ~30 seconds at 15ms per cycle */
#define TOTAL_WORDS 1048576ul
static uint16_t *fb;
static BatteryNative state;
static BatteryNativeSnapshot sample;
static int valid;
static unsigned updates;
void __attribute__((weak)) _fini(void) {}

static void fill(int x,int y,int w,int h,uint16_t c) {
    if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;}
    if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
    if(x+w>EH_W)w=EH_W-x;
    if(y+h>EH_H)h=EH_H-y;
    for(int row=y;row<y+h;row++) for(int col=x;col<x+w;col++)
        fb[row*EH_W+col]=c;
}
static void write8(int x,int y,const char *str,int max,uint16_t c) {
    for(int n=0;str && str[n] && n<max;n++) {
        unsigned char ch=(unsigned char)str[n];
        if(ch>127)ch='?';
        for(int row=0;row<8;row++) {
            unsigned char bits=font8x8_basic[ch][row];
            for(int col=0;col<8;col++)if(bits&(1u<<col))
                fill(x+n*8+col,y+row,1,1,c);
        }
    }
}
static const char *mode_name(void) {
    if(!(valid && (sample.capabilities & BATTERY_NATIVE_CHARGE_MODE)))
        return "NOT AVAILABLE";
    /* Charge mode refers only to validated OS task metadata. We do not
     * claim independently verified USB power or charging current. */
    switch(sample.charge_mode) {
        case 1:return "DISCHARGE (OS)";
        case 2:case 3:case 4:case 5:return "CHARGING (OS)";
        case 6:return "CHECKING (OS)";
        default:return "UNKNOWN MODE";
    }
}
static void screen(unsigned step) {
    const uint16_t dark=RGB(9,17,29),panel=RGB(25,41,58);
    const uint16_t white=RGB(236,244,249),teal=RGB(62,224,197),muted=RGB(160,180,195);
    char buf[46];
    fb=eh_framebuffer();
    fill(0,0,320,240,dark);
    fill(0,0,320,32,panel);
    fill(0,0,4,32,teal);
    write8(11,11,"ENDRA DIAGNOSTICS",18,teal);
    write8(274,11,"007",3,white);
    write8(12,42,"BATTERY STATUS / LIVE",32,white);

    if(valid && (sample.capabilities & BATTERY_NATIVE_PERCENT) && sample.percent<=100u) {
        snprintf(buf,sizeof(buf),"OS BATTERY: %lu%%",(unsigned long)sample.percent);
        write8(12,63,buf,35,teal);
        fill(12,85,295,20,panel);
        fill(15,88,(int)(289u*sample.percent/100u),14,teal);
    } else {
        write8(12,63,"OS BATTERY: --",35,teal);
        fill(12,85,295,20,panel);
    }

    write8(12,115,"OS CHARGE MODE:",32,white);
    write8(12,132,mode_name(),36,teal);

    if(!state.complete) {
        unsigned long n=state.scanned_words;
        if(n>TOTAL_WORDS)n=TOTAL_WORDS;
        snprintf(buf,sizeof(buf),"SCANNING OS: %lu%%",n*100ul/TOTAL_WORDS);
        write8(12,153,buf,36,muted);
    } else if(!valid) {
        write8(12,153,"OS SIGNATURE UNAVAILABLE",36,muted);
    } else {
        snprintf(buf,sizeof(buf),"DATA RECHECKS: %u",updates);
        write8(12,153,buf,36,muted);
    }
    write8(12,173,"COARSE OS DATA / NO VOLTAGE",36,muted);
    fill(9,196,302,32,panel);
    write8(16,199,"ESC FAST / ENTER SAFE",36,teal);
    write8(16,211,"AUTO 30S = SAFE",36,muted);
    fill(15,219,(int)(290u*step/MAX_STEPS),3,teal);
    eh_platform_flip();
}
/* Fast vs reference exit both restore LCD and timer; only
 * redundant TI screen refresh differs. */
void eh_platform_stop_mode(int refresh_ti_screen);
int main(void) {
    int fast_exit=0;
    memset(&sample,0,sizeof(sample));
    battery_native_init(&state);
    if(!eh_platform_start())return 1;
    screen(0);
    for(unsigned i=0;i<MAX_STEPS;i++) {
        EhKey key=eh_platform_readkey();
        if(key==EH_KEY_ESC){fast_exit=1;break;}
        if(key==EH_KEY_ENTER){fast_exit=0;break;}
        /* Run005 detection rate, then a conservative ~1Hz OS snapshot.
         * Unchanged memory validation + IRQ preservation from Run004.
         */
        if((!state.complete && i%5u==0u) ||
           (state.complete && i%67u==0u)) {
            valid=battery_native_default(&state,&sample,
                                        state.complete?0u:16384u);
            if(state.complete)updates++;
        }
        if(i%65u==0u || (state.complete && i%67u==0u))
            screen(i+1);
        eh_platform_sleep();
    }
    eh_platform_stop_mode(fast_exit?0:1);
    return 0;
}
