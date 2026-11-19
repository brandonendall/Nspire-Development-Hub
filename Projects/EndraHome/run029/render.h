#ifndef EH010_RENDER_H
#define EH010_RENDER_H
/* Include local registry first: imported Run007 renderer header otherwise
 * selects obsolete Apps007 filename on the Help screen.
 */
#include "registry.h"
#include "../run005/logic7.h"
#include <stdint.h>
void eh7_render(uint16_t *frame,const EhUI7 *ui,const EhRegistry *apps);
#endif
