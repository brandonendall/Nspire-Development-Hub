#ifndef EH012_SESSION_H
#define EH012_SESSION_H
#include "registry.h"
#include "ui.h"
void eh_assignment_complete(EhUniversalUI *ui,int slot);
void eh11_resume_home(EhUniversalUI *ui);
int eh_launch_failed(int result);
void eh_return_from_launch(EhUniversalUI *ui,int result);
#endif
