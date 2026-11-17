/* Stateless child-return reset. No hooks or hardware I/O. */
#include "session.h"
void eh11_resume_home(EhUniversalUI *ui) {
 if(!ui)return;
 int tile=ui->page.selected;
 const EhRegistry *apps=ui->apps;
 eh7_ui_init(ui,apps);
 if(tile>=0&&tile<EH7_SLOT_COUNT)ui->page.selected=tile;
}
