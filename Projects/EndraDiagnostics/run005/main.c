/*
 * EndraDiagnostics Run005: finish the existing Run004 battery-state
 * signature scan earlier and report precise discovery/validation status.
 * Reuses unchanged Run004 battery adapter and Run002 platform.
 * OS data is optional: no guessed battery percent.
 */
#include "../run002/platform.h"
#include "../run004/vendor/battery_native.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "font8x8_basic.h"

#define RGB(r,g,b) ((uint16_t)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3)))
#define EXPECTED_WORDS 1048576UL
#define LIMIT_STEPS 800U
static uint16_t *fb;
static BatteryNative battery;
static BatteryNativeSnapshot sample;
static int readable, mapping_ok;
void __attribute__((weak)) _fini(void) {}

static void rect(int x,int y,int w,int h,uint16_t c) {
    if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
    if(x>=EH_W||y>=EH_H||w<=0||h<=0)return;
    if(x+w>EH_W)w=EH_W-x;
    if(y+h>EH_H)h=EH_H-y;
    for(int j=y;j<y+h;j++)
        for(int i=x;i<x+w;i++)fb[j*EH_W+i]=c;
}
static void print8(int x,int y,const char *s,int n,uint16_t c) {
    for(int i=0;s && s[i] && i<n;i++) {
        unsigned char ch=(unsigned char)s[i];
        if(ch>127)ch='?';
        for(int row=0;row<8;row++) {
            unsigned char bits=font8x8_basic[ch][row];
            for(int col=0;col<8;col++)
                if(bits&(1u<<col))rect(x+i*8+col,y+row,1,1,c);
        }
    }
}
static void display(unsigned step) {
    const uint16_t bg=RGB(9,17,29),panel=RGB(25,41,58),
        white=RGB(238,244,249),teal=RGB(66,224,199),dim=RGB(161,182,196);
    char line[44];
    fb=eh_framebuffer();
    rect(0,0,EH_W,EH_H,bg);
    rect(0,0,EH_W,34,panel);
    rect(0,0,4,34,teal);
    print8(10,12,"ENDRA DIAGNOSTICS",18,teal);
    print8(275,12,"005",3,white);
    print8(12,47,"BATTERY / OS DISCOVERY",36,white);
    if(readable && (sample.capabilities & BATTERY_NATIVE_PERCENT)) {
        snprintf(line,sizeof(line),"OS BATTERY: %lu%%",(unsigned long)sample.percent);
        print8(12,69,line,36,teal);
        print8(12,88,"COARSE OS LEVEL, NOT VOLTAGE",36,dim);
    } else {
        print8(12,69,"OS BATTERY: --",36,teal);
        if(battery.complete)
            print8(12,88,"SIGNATURE NOT RECOGNIZED",36,dim);
        else if(!mapping_ok)
            print8(12,88,"OS MAPPING UNAVAILABLE",36,dim);
        else print8(12,88,"SCANNING KNOWN OS PATTERNS",36,dim);
    }
    unsigned long progress=(unsigned long)battery.scanned_words;
    if(progress>EXPECTED_WORDS)progress=EXPECTED_WORDS;
    snprintf(line,sizeof(line),"SCAN: %lu%% (%lu/1024K)",
             progress*100UL/EXPECTED_WORDS,progress/1024UL);
    print8(12,110,line,36,white);
    snprintf(line,sizeof(line),"MATCH P:%u C:%u A:%u V:%u",
             battery.percent_matches,battery.charge_matches,
             battery.adc_matches,battery.converter_matches);
    print8(12,131,line,36,white);
    snprintf(line,sizeof(line),"FLAGS:%02lX ERR:%02lX",
             (unsigned long)(sample.capabilities & 255u),
             (unsigned long)(sample.errors & 255u));
    print8(12,152,line,36,dim);
    if(battery.complete)
        print8(12,172,"SCAN COMPLETE",36,teal);
    else print8(12,172,"READ-ONLY; NO ADC WRITES",36,dim);
    rect(9,192,302,36,panel);
    print8(16,200,"ESC EXIT / AUTO ~12 SEC",36,teal);
    rect(15,219,(int)(step*290u/LIMIT_STEPS),3,teal);
    eh_platform_flip();
}
int main(void) {
    memset(&sample,0,sizeof(sample));
    battery_native_init(&battery);
    if(!eh_platform_start())return 1;
    display(0);
    for(unsigned i=0;i<LIMIT_STEPS;i++) {
        EhKey key=eh_platform_readkey();
        if(key==EH_KEY_ESC)break;
        /* The exact same validated read-only scan as Run004, in
         * 16K word budgets every 5 frames rather than every 10.
         * Full 4 MiB search typically needs 64 calls.
         */
        if((i%5u)==0u) {
            readable=battery_native_default(&battery,&sample,
                                             battery.complete?0u:16384u);
            if(battery.scanned_words)mapping_ok=1;
        }
        if((i%40u)==0u || (battery.complete && i%65u==0u))
            display(i+1);
        eh_platform_sleep();
    }
    eh_platform_stop();
    return 0;
}
