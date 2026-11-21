#ifndef ENDRAHOME_APP_TARGET_H
#define ENDRAHOME_APP_TARGET_H

typedef enum {
    EH_APP_READY, EH_APP_INVALID, EH_APP_SELF, EH_APP_MISSING,
    EH_APP_UNREADABLE, EH_APP_DOCUMENT
} EhAppStatus;

int eh_tns_suffix(const char *name);
int eh_same_path(const char *left,const char *right);
EhAppStatus eh_app_file(const char *path);
EhAppStatus eh_app_target(const char *path,const char *launcher);
const char *eh_app_status(EhAppStatus status);

#endif
