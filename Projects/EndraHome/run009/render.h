#ifndef EH009_RENDER_H
#define EH009_RENDER_H
/* Keep the versioned config header first. Including run007/render.h first
 * pulled in the old Apps007 constant and made the help screen lie about
 * which configuration is actually loaded. */
#include "registry.h"
#include "../run005/logic7.h"
#include <stdint.h>
void eh7_render(uint16_t *frame,const EhUI7 *ui,const EhRegistry *apps);
#endif
