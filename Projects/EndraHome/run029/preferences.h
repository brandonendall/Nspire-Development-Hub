#ifndef ENDRAHOME_PREFERENCES_H
#define ENDRAHOME_PREFERENCES_H
#include "display.h"
#define EH_PREFS_FILE "EndraHome_Appearance4.cfg.tns"
#define EH_RUN038_PREFS_FILE "EndraHome_Appearance3.cfg.tns"
#define EH_LEGACY_PREFS_FILE "EndraHome_Appearance.cfg.tns"
int eh_prefs_load(const char *app_config, EhAppearance *appearance);
int eh_prefs_save(const char *app_config, EhAppearance appearance);
#endif
