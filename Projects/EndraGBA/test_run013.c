#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static struct { char cheat_name[17]; unsigned cheat_active; } cheats[16];
static unsigned num_cheats, writes, last_width;
static uint32_t last_address, last_value;
static void record(unsigned width,uint32_t a,uint32_t b) {
  ++writes; last_width=width; last_address=a; last_value=b;
}
#define write_memory8(a,b) record(1,a,b)
#define write_memory16(a,b) record(2,a,b)
#define write_memory32(a,b) record(4,a,b)
#include "run013_cheats.c"
int main(void) {
  FILE *f=fopen("Game.cht.tns","w"); assert(f);
  fputs("GAME AU2E\n!Power Ring=47E97FBD E485E4F3\nAlways=CBD43A43 720490C7\n%bad=CBD43A43 720490C7\nMaster=B7E107A8 DBBDA17A\n",f);
  fclose(f);
  load_ar3("Game.gbz.tns","AU2E");
  assert(num_cheats==3 && !strcmp(cheats[1].cheat_name,"Power Ring"));
  apply_ar3(); assert(!writes);
  cheats[0].cheat_active=1; apply_ar3(); apply_ar3();
  assert(writes==1 && last_width==4 && last_address==0x02003d6c && last_value==0xc0006);
  cheats[2].cheat_active=1; apply_ar3(); apply_ar3(); assert(writes==3);
  cheats[2].cheat_active=0; apply_ar3(); assert(writes==3);
  load_ar3("Game.gba.tns","AU2J"); assert(num_cheats==0);
  apply_ar3(); assert(writes==3);
  load_ar3("Game.gba.tns","AU2E"); assert(num_cheats==3);
  load_ar3(current_rom,"AU2E"); assert(num_cheats==3 && !cheats[2].cheat_active);
  return 0;
}
