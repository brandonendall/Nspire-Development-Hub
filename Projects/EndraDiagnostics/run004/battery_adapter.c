/*
 * EndraDiagnostics Run004: read-only validated OS battery-state monitor.
 * Uses vetted nMedia battery-resolver logic under GPL-3.0.
 * NO power-controller register writes, OS getter calls or calibration writes.
 */
#include "vendor/battery_native.h"
#include "vendor/arm926_ram_span.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct {PortableView view;uint32_t table_alias;int valid;} Reader;
static Reader reader;
static uint32_t mmu_word(uint32_t at,void *unused) {
    (void)unused;
    return *(const volatile uint32_t *)(uintptr_t)at;
}
static unsigned enter_critical(void) {
    unsigned old,masked;
    __asm__ volatile("mrs %0, cpsr\n\torr %1, %0, #0xc0\n\tmsr cpsr_c, %1"
        :"=&r"(old),"=&r"(masked)::"memory","cc");
    return old & 0xc0u;
}
static void leave_critical(unsigned previous) {
    unsigned curr,restore;
    __asm__ volatile("mrs %0, cpsr":"=r"(curr));
    restore=(curr & ~0xc0u) | (previous & 0xc0u);
    __asm__ volatile("msr cpsr_c, %0"::"r"(restore):"memory","cc");
}
static int admissible(void *ctx,uint32_t at,uint32_t bytes,unsigned kind) {
    Reader *r=(Reader *)ctx;
    (void)kind;
    return r->valid && arm926_ram_span(at,bytes,r->table_alias,mmu_word,NULL,NULL);
}
static int read_checked(void *ctx,uint32_t at,uint32_t *out) {
    if(!out || !admissible(ctx,at,4u,PORTABLE_DATA))return 0;
    *out=mmu_word(at,NULL);
    return 1;
}
static const uint32_t *code_checked(void *ctx,uint32_t at,uint32_t bytes) {
    if(!admissible(ctx,at,bytes,PORTABLE_CODE))return NULL;
    if((at&3u)||(bytes&3u))return NULL;
    return (const uint32_t *)(uintptr_t)at;
}
static int refresh_reader(void) {
    uint32_t control,ttbr,dacr,alias,self;
    reader.valid=0;
    __asm__ volatile("mrc p15, 0, %0, c1, c0, 0":"=r"(control));
    __asm__ volatile("mrc p15, 0, %0, c2, c0, 0":"=r"(ttbr));
    __asm__ volatile("mrc p15, 0, %0, c3, c0, 0":"=r"(dacr));
    ttbr &= 0xffffc000u;
    if(!(control&1u) || ((dacr&3u)!=1u && (dacr&3u)!=3u))return 0;
    /* Reject unknown MMU layout or a relocated/nonidentity table. */
    if(!((ttbr>=0x10000000u && ttbr<=0x13ffc000u) ||
         (ttbr>=0xa4000000u && ttbr<=0xa400c000u)))return 0;
    alias=ttbr;
    self=mmu_word(alias+(ttbr>>20)*4u,NULL);
    if((self&0x1e3u)!=2u || !(self&0xc00u) ||
       (self&0xfff00000u)!=(ttbr&0xfff00000u))return 0;
    reader.table_alias=alias;
    reader.valid=1;
    reader.view=(PortableView){&reader,admissible,read_checked,
        0x10000000u,0x12000000u,0x10000000u,0x14000000u,code_checked};
    return 1;
}
bool battery_native_default(BatteryNative *b,BatteryNativeSnapshot *s,unsigned budget) {
    if(!b || !s)return false;
    memset(s,0,sizeof(*s));
    if(budget>16384u)budget=16384u;
    unsigned saved=enter_critical();
    bool result=false;
    if(refresh_reader()){
        while(budget && !b->complete){
            unsigned slice=budget>4096u?4096u:budget;
            battery_native_resolve_step(b,&reader.view,slice);
            budget-=slice;
        }
        result=battery_native_snapshot(b,&reader.view,s);
    }
    reader.valid=0;
    leave_critical(saved);
    return result;
}
