#ifndef EH013_CONFIG_SAVE_H
#define EH013_CONFIG_SAVE_H
#include "registry.h"
/* Commit one shortcut via temporary file + rename. Existing file is preserved
 * on failure; live registry only updates after successful rename. */
int eh13_assign_save(EhRegistry *apps,int slot,const char *path,const char *cfg);
#endif
