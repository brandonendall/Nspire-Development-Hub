#ifndef ENDRAHOME_PREFERENCES_H
#define ENDRAHOME_PREFERENCES_H
#include "display.h"
#define EH_PREFS_FILE "EndraHome_Appearance4.cfg.tns"
#define EH_RUN038_PREFS_FILE "EndraHome_Appearance3.cfg.tns"
#define EH_LEGACY_PREFS_FILE "EndraHome_Appearance.cfg.tns"
int eh_prefs_load(const char *app_config, EhAppearance *appearance);
#define EH_ICON_COLOR_LINE "# icon-color "
#define EH_APPEARANCE_LINE "# appearance "
int eh_prefs_valid(EhAppearance appearance);
int eh_prefs_parse(const char *record, EhAppearance *appearance, unsigned version);
#endif
