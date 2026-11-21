/* Stateless child-return reset. No hooks or hardware I/O. */
#include "session.h"
#include "desktop.h"
void eh_assignment_complete(EhUniversalUI *ui,int slot){
 ui->page.page=EH_PAGE_HOME;
 ui->page.selected=eh_desktop_select(ui->apps,slot,EH_KEY_NONE);
 ui->assign_slot=ui->adding_app=0;
}

void eh11_resume_home(EhUniversalUI *ui) {
 if(!ui)return;
 int tile=ui->page.selected;
 const EhRegistry *apps=ui->apps;
 eh7_ui_init(ui,apps);
 if(tile>=0&&tile<EH7_SLOT_COUNT)ui->page.selected=tile;
}


/* Ndless ld_exec_with_args uses these two sentinels for loader failures.
 * An ordinary positive child return value is an application result. */
int eh_launch_failed(int result){
 return result < 0 || result == 0xDEAD || result == 0xBEEF;
}

void eh_return_from_launch(EhUniversalUI *ui,int result){
 eh11_resume_home(ui);
 if(!eh_launch_failed(result))return;
 eh_toast_set(&ui->notice,result==0xBEEF?"NDLESS REJECTED APP FORMAT":"NDLESS COULD NOT START APP");
}
