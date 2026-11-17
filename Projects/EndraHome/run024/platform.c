/*
 * GBA-inspired Nspire platform for EndraHome.
 *
 * Adapted from vendor/upstream-gpsp/nspire/nspire.c, nspire_input.c and
 * nspire/nspire_hw.h (andymcca/lr-gpsp-nspire @ 2ddeb23...).
 * Original gpSP developers; GPL-2.0-or-later. See README.md.
 *
 * Unlike Run001/Run002, this doesn't use SDL or lcd_init: it saves/restores
 * the native LCD registers and uses the same portrait triple-buffer concept
 * as the known-working gpSP frontend. Hardware safety remains UNVERIFIED.
 */
#include "platform.h"
#include <libndls.h>
#include <os.h>
#include <string.h>
#include <stdint.h>

#define READ(r) (*(volatile uint32_t *)(r))
#define WRITE(r,v) (*(volatile uint32_t *)(r)=(uint32_t)(v))
#define LCD_TIM0 0xC0000000u
#define LCD_TIM1 0xC0000004u
#define LCD_TIM2 0xC0000008u
#define LCD_BASE 0xC0000010u
#define LCD_CTRL 0xC0000018u
#define LCD_CUR  0xC000002Cu
#define LCD_IRQ_RAW 0xC0000020u
#define LCD_IRQ_CLEAR 0xC0000028u
#define KEY_BASE 0x900E0010u
#define TIMER_LOAD 0x900C0000u
#define TIMER_VALUE 0x900C0004u
#define TIMER_CTRL 0x900C0008u
#define TIMER_CLOCK 0x900C0080u
#define TICKS 32768u
#define WPIX (EH_W*EH_H)
#define KEY_ENTER_RAW 2
#define KEY_CLICK_RAW 98
#define KEY_MENU_RAW 102
#define KEY_ESC_RAW 104
#define KEY_UP_RAW 113
#define KEY_RIGHT_RAW 115
#define KEY_DOWN_RAW 117
#define KEY_LEFT_RAW 119
#define TOUCHPAD_MASK 0xFF00FFFDu

static uint16_t lcd_screens[3][WPIX] __attribute__((aligned(32)));
static uint16_t landscape[WPIX] __attribute__((aligned(32)));
static int frame_index,portrait,started;
static uintptr_t original_base;
static uint32_t original_tim0,original_tim1,original_ctrl;
static uint32_t original_timer_load,original_timer_clock,original_timer_ctrl;
static int timer_ok;
static uint32_t fallback_ticks;
static uint32_t key_curr[4],key_rising[4];
static int touch_dx,touch_dy,touch_contact;
static int last_touch_x,last_touch_y,last_contact;
static EhKey held_direction;
static uint32_t repeat_at;

static inline void drain_writes(void) {
  __asm__ volatile ("mcr p15, 0, %0, c7, c10, 4"::"r"(0):"memory");
}
static void cache_flush(void *start,unsigned size){
  uintptr_t at=(uintptr_t)start&~(uintptr_t)31;
  uintptr_t end=(uintptr_t)start+size;
  for(;at<end;at+=32)
    __asm__ volatile ("mcr p15, 0, %0, c7, c14, 1"::"r"(at));
  drain_writes();
}
static void cache_clean(void *start,unsigned size){
  uintptr_t at=(uintptr_t)start&~(uintptr_t)31;
  uintptr_t end=(uintptr_t)start+size;
  for(;at<end;at+=32)
    __asm__ volatile ("mcr p15, 0, %0, c7, c10, 1"::"r"(at));
  drain_writes();
}
static uint32_t ticks(void){
  if(timer_ok)return ~READ(TIMER_VALUE);
  return fallback_ticks;
}
static void initialize_timer(void){
  original_timer_load=READ(TIMER_LOAD);
  original_timer_clock=READ(TIMER_CLOCK);
  original_timer_ctrl=READ(TIMER_CTRL);
  WRITE(TIMER_CTRL,0);
  WRITE(TIMER_CLOCK,0xA);
  WRITE(TIMER_LOAD,0xFFFFFFFFu);
  WRITE(TIMER_CTRL,0x82);
  uint32_t first=READ(TIMER_VALUE);
  timer_ok=0;fallback_ticks=0;
  for(int n=0;n<400000&&!timer_ok;n++)
    timer_ok=(READ(TIMER_VALUE)!=first);
}
static void restore_timer(void){
  WRITE(TIMER_CTRL,0);
  WRITE(TIMER_CLOCK,original_timer_clock);
  WRITE(TIMER_LOAD,original_timer_load);
  WRITE(TIMER_CTRL,original_timer_ctrl);
}
int eh_platform_start(void){
  if(started)return 1;
  original_base=(uintptr_t)READ(LCD_BASE);
  original_tim0=READ(LCD_TIM0);
  original_tim1=READ(LCD_TIM1);
  original_ctrl=READ(LCD_CTRL);
  uint32_t per_line=((READ(LCD_TIM2)>>16)&0x3FFu)+1;
  uint32_t lines=(READ(LCD_TIM1)&0x3FFu)+1;
  uint32_t bpp=(READ(LCD_CTRL)>>1)&7;
  portrait=(per_line==240&&lines==320&&bpp==6);
  initialize_timer();
  memset(lcd_screens,0,sizeof(lcd_screens));
  for(int i=0;i<3;i++)cache_flush(lcd_screens[i],sizeof(lcd_screens[i]));
  frame_index=1;
  WRITE(LCD_BASE,(uintptr_t)lcd_screens[0]);
  if(!portrait){
    WRITE(LCD_CTRL,0);
    WRITE(LCD_TIM0,0x251A004C);
    WRITE(LCD_TIM1,0x110300EF);
    uint32_t val=(original_ctrl&~0x90Eu)|0x008u;
    WRITE(LCD_CTRL,val);
    WRITE(LCD_CTRL,val|0x800u);
  }
  memset(key_curr,0,sizeof(key_curr));
  memset(key_rising,0,sizeof(key_rising));
  held_direction=EH_KEY_NONE;
  repeat_at=0;
  started=1;
  return 1;
}
uint16_t *eh_framebuffer(void){return landscape;}
void eh_platform_flip(void){
  if(!started)return;
  uint16_t *dst=lcd_screens[frame_index];
  if(portrait){
    /* Landscape (x,y) -> portrait [x][239-y] in 5:6:5, like gpSP. */
    for(int y=0;y<EH_H;y++)
      for(int x=0;x<EH_W;x++)
        dst[x*EH_H+(EH_H-1-y)]=landscape[y*EH_W+x];
    drain_writes();
  }else{
    memcpy(dst,landscape,sizeof(landscape));
    cache_clean(dst,sizeof(lcd_screens[0]));
  }
  WRITE(LCD_BASE,(uintptr_t)dst);
  frame_index=(frame_index+1)%3;
  uintptr_t next=(uintptr_t)lcd_screens[frame_index];
  uint32_t start_tick=ticks();
  for(int polls=0;polls<1000000;polls++){
    uintptr_t reader=(uintptr_t)READ(LCD_CUR);
    if(reader<next||reader>=next+sizeof(lcd_screens[0])||
       (uint32_t)(ticks()-start_tick)>TICKS/25)break;
  }
}
void eh_platform_stop(void){
  if(!started)return;
  if(portrait){
    WRITE(LCD_BASE,original_base);
  }else{
    WRITE(LCD_CTRL,0);
    WRITE(LCD_BASE,original_base);
    WRITE(LCD_TIM0,original_tim0);
    WRITE(LCD_TIM1,original_tim1);
    WRITE(LCD_CTRL,original_ctrl&~0x800u);
    WRITE(LCD_CTRL,original_ctrl);
  }
  restore_timer();
  /* Experiment Run010: avoid refresh_osscr() which refreshes the TI
   * document browser (slow on file-heavy calculators). The launcher
   * only READS its configuration and does not change document entries.
   * Keep all hardware LCD/timer register restoration unchanged. */
  started=0;
}
static void poll_keys(void){
  touchpad_report_t tp;
  uint32_t arrows=0;
  memset(&tp,0,sizeof(tp));
  touchpad_scan(&tp);
  touch_contact=!!tp.contact;
  touch_dx=touch_dy=0;
  if(touch_contact&&last_contact){
    int dx=(int)tp.x-last_touch_x,dy=(int)tp.y-last_touch_y;
    /* Ignore discontinuities and suppress jitter. */
    if(dx>-50&&dx<50&&dy>-50&&dy<50){
      touch_dx=dx;touch_dy=dy;
    }
  }
  last_touch_x=tp.x;last_touch_y=tp.y;last_contact=touch_contact;
  switch((tpad_arrow_t)tp.arrow){
    case TPAD_ARROW_UP:arrows=0x00010000u;break;
    case TPAD_ARROW_UPRIGHT:arrows=0x00050000u;break;
    case TPAD_ARROW_RIGHT:arrows=0x00040000u;break;
    case TPAD_ARROW_RIGHTDOWN:arrows=0x00140000u;break;
    case TPAD_ARROW_DOWN:arrows=0x00100000u;break;
    case TPAD_ARROW_DOWNLEFT:arrows=0x00500000u;break;
    case TPAD_ARROW_LEFT:arrows=0x00400000u;break;
    case TPAD_ARROW_LEFTUP:arrows=0x00410000u;break;
    case TPAD_ARROW_CLICK:arrows=0x00000002u;break;
    default:break;
  }
  uint32_t next[4]={
    READ(KEY_BASE)|(on_key_pressed()?0x200u:0u),
    READ(KEY_BASE+4),
    READ(KEY_BASE+8),
    (READ(KEY_BASE+12)&TOUCHPAD_MASK)|arrows
  };
  for(int i=0;i<4;i++){
    key_rising[i]=(~key_curr[i])&next[i];
    key_curr[i]=next[i];
  }
}
static int key_active(int key,int rising){
  uint32_t word=(uint32_t)(key-1)>>5;
  uint32_t bit=1u<<((key-1)&31);
  return !!((rising?key_rising[word]:key_curr[word])&bit);
}
EhKey eh_platform_readkey(void){
  poll_keys();
  if(key_active(KEY_ESC_RAW,1))return EH_KEY_ESC;
  if(key_active(KEY_MENU_RAW,1))return EH_KEY_MENU;
  if(key_active(KEY_CLICK_RAW,1)||key_active(KEY_ENTER_RAW,1))return EH_KEY_ENTER;
  EhKey direction=EH_KEY_NONE;
  if(key_active(KEY_UP_RAW,0))direction=EH_KEY_UP;
  else if(key_active(KEY_DOWN_RAW,0))direction=EH_KEY_DOWN;
  else if(key_active(KEY_RIGHT_RAW,0))direction=EH_KEY_RIGHT;
  else if(key_active(KEY_LEFT_RAW,0))direction=EH_KEY_LEFT;
  uint32_t now=ticks();
  if(direction!=held_direction){
    held_direction=direction;
    repeat_at=now+TICKS*2/5;
    return direction;
  }
  if(direction!=EH_KEY_NONE&&(int32_t)(now-repeat_at)>=0){
    repeat_at=now+TICKS/15;
    return direction;
  }
  return EH_KEY_NONE;
}
void eh_platform_sleep(void){
  /* Bounded pause, unlike unbounded wait-no-key-pressed loops. */
  msleep(15);
  if(!timer_ok)fallback_ticks+=TICKS*15/1000;
}

int eh_platform_touch_pan(int *dx,int *dy){
 if(!dx||!dy)return 0;
 *dx=touch_dx;*dy=touch_dy;
 touch_dx=touch_dy=0;
 return touch_contact;
}
