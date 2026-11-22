#ifndef ENDRAHOME_ARCHIVE_H
#define ENDRAHOME_ARCHIVE_H
#include "picker.h"
int eh_zip_create(const char *source,const char *output);
int eh_zip_extract(const char *source,const char *output);
/* Prefix is empty for root or ends with /. Replaces the listing on success. */
int eh_zip_list(const char *source,const char *prefix,Eh13Picker *view);
#endif
