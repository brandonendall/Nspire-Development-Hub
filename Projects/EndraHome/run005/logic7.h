#ifndef EH_LOGIC7_H
#define EH_LOGIC7_H
#include "../gba-ui/logic.h"
#define EH_DOCK_COUNT 7
typedef struct {EhPage page; int selected; int should_exit;} EhUI7;
void eh7_init(EhUI7 *s);
int eh7_dispatch(EhUI7 *s, EhKey key);
#endif
