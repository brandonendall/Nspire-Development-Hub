#ifndef EH013_PICKER_H
#define EH013_PICKER_H
#include "registry.h"
#include "../gba-ui/logic.h"
#include <stddef.h>
#include <stdint.h>
#define EH13_PICKER_MAX 768
#define EH13_NAME_MAX 104
#define EH13_VIEW_ROWS 8
typedef struct {
 char name[EH13_NAME_MAX];
 unsigned char directory;
 unsigned long long bytes;
} Eh13PickEntry;
typedef struct {
 char dir[EH7_PATH_CAP];
 char root[EH7_PATH_CAP];
 Eh13PickEntry *items;
 int count,selected,scroll,slot,active,overflow,show_all;
} Eh13Picker;
/* root is /documents on hardware; injectable tmp path for host tests. */
int eh13_picker_open(Eh13Picker *p,int slot,const char *root);
int eh17_picker_open(Eh13Picker *p,int slot,const char *root,int show_all);
void eh13_picker_close(Eh13Picker *p);
/* -1 cancel, 0 no input, 1 redraw, 2 selected file (not yet saved). */
int eh13_picker_handle(Eh13Picker *p,EhKey key,char *chosen,size_t cap);
void eh13_picker_render(uint16_t *frame,const Eh13Picker *p,const EhRegistry *apps);
#endif
