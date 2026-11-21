#ifndef EH015_EDIT_H
#define EH015_EDIT_H
#include "registry.h"
#include "config_save.h"
int eh_assign_app(EhRegistry *apps,int slot,const char *path,int adding,
                  const char *cfg,Eh14SaveError *err);
int eh15_update_shortcut(EhRegistry *apps,int slot,const char *label,const char *path,
                         const char *cfg,Eh14SaveError *err);
#endif
