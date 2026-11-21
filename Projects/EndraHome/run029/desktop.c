#include "desktop.h"
#include <stdlib.h>

int eh_desktop_layout(const EhRegistry *apps, EhTile tiles[EH7_SLOT_COUNT]) {
    int count = 0;
    for (int slot = 0; slot < EH7_SLOT_COUNT; ++slot) {
        if (slot == EH_FILES_SLOT || (apps && apps->slots[slot].path[0]))
            tiles[count++].slot = slot;
    }
    int first = count <= 4 ? count : (count + 1) / 2;
    int spacing = first <= 2 ? 108 : first == 3 ? 94 : 76;
    for (int i = 0; i < count; ++i) {
        int row = i >= first;
        int cols = row ? count - first : first;
        int col = row ? i - first : i;
        tiles[i].row = row;
        tiles[i].center = 160 + (2 * col + 1 - cols) * spacing / 2;
        /* Keep both 48-pixel icon rows and their captions clear of the header. */
        tiles[i].top = count <= 4 ? 106 : (row ? 146 : 57);
    }
    return count;
}

int eh_desktop_select(const EhRegistry *apps, int selected, EhKey key) {
    EhTile tiles[EH7_SLOT_COUNT];
    int count = eh_desktop_layout(apps, tiles);
    int at = 0;
    while (at < count && tiles[at].slot != selected) ++at;
    if (at == count) return tiles[0].slot;
    if (key == EH_KEY_LEFT || key == EH_KEY_RIGHT) {
        int adjacent = at + (key == EH_KEY_RIGHT ? 1 : -1);
        if (adjacent >= 0 && adjacent < count && tiles[adjacent].row == tiles[at].row)
            return tiles[adjacent].slot;
    } else if (key == EH_KEY_UP || key == EH_KEY_DOWN) {
        int row = tiles[at].row + (key == EH_KEY_DOWN ? 1 : -1);
        int nearest = at, best = 320;
        for (int i = 0; i < count; ++i) {
            int distance = abs(tiles[i].center - tiles[at].center);
            if (tiles[i].row == row && distance < best) {
                nearest = i;
                best = distance;
            }
        }
        return tiles[nearest].slot;
    }
    return selected;
}
