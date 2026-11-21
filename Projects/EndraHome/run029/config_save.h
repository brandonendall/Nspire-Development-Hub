#ifndef EH014_CONFIG_SAVE_H
#define EH014_CONFIG_SAVE_H
#include "registry.h"
/* Show the hardware-tested failing stage and errno rather than one generic
 * failure. File-specific errors are intentionally never swallowed.
 * Existing configuration is never removed before a safe backup rename. */
typedef enum {
 EH14_OK=0, EH14_INVALID=1, EH14_PATHS=2,
 EH14_TEMP_BUSY=3, EH14_TEMP_OPEN=4, EH14_TEMP_WRITE=5,
 EH14_TEMP_VERIFY=6, EH14_BACKUP_BUSY=7, EH14_BACKUP_MOVE=8,
 EH14_INSTALL_MOVE=9, EH14_ROLLBACK=10, EH14_INSTALL_VERIFY=11,
 EH14_BACKUP_RETAINED=12
} Eh14SaveStage;
typedef struct {Eh14SaveStage stage; int os_error;} Eh14SaveError;
int eh14_assign_save(EhRegistry *apps,int slot,const char *path,const char *cfg,Eh14SaveError *error);
/* Return 1 if no recovery is needed or interrupted swap was recovered.
 * Return 0 if existing backup needs manual recovery; never delete it. */
int eh_save_clock(EhRegistry *apps,int offset,const char *cfg,Eh14SaveError *error);
int eh_save_appearance(EhRegistry *apps,EhAppearance appearance,const char *cfg,Eh14SaveError *error);
int eh14_recover_config(const char *cfg,Eh14SaveError *error);
const char *eh14_save_stage_name(Eh14SaveStage stage);
#endif
