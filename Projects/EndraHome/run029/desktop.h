#ifndef ENDRAHOME_DESKTOP_H
#define ENDRAHOME_DESKTOP_H
#include "registry.h"
#include "../gba-ui/logic.h"

typedef struct { int slot, row, center, top; } EhTile;
int eh_desktop_layout(const EhRegistry *apps, EhTile tiles[EH7_SLOT_COUNT]);
int eh_desktop_select(const EhRegistry *apps, int selected, EhKey direction);
#endif
