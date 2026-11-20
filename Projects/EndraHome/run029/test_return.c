#include "session.h"
#include "desktop.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    EhRegistry apps;
    eh7_registry_init(&apps);
    eh7_registry_parse(&apps,"5=Tools|/documents/Test006.tns");
    eh7_registry_parse(&apps,"3=Files|/documents/FileManager.tns");
    EhUniversalUI s;
    eh7_ui_init(&s,&apps);
    /* Unconfigured shortcuts are hidden; Files is the first desktop tile. */
    assert(s.page.selected==EH_FILES_SLOT);
    const int errors[] = {-1, 0xDEAD, 0xBEEF};
    for (unsigned i=0; i<sizeof(errors)/sizeof(errors[0]); ++i) {
        eh7_ui_handle(&s,EH_KEY_ENTER);
        assert(s.request==EH_FILES_SLOT+1);
        eh_return_from_launch(&s,errors[i]);
        assert(s.open_files && !s.request);
        assert(!strcmp(apps.slots[EH_FILES_SLOT].path,"/documents/FileManager.tns"));
        s.open_files=0;
    }
    eh7_ui_handle(&s,EH_KEY_ENTER);
    eh_return_from_launch(&s,1);
    assert(!s.open_files && !s.request && s.page.selected==EH_FILES_SLOT);
    eh7_ui_handle(&s,EH_KEY_RIGHT);
    eh7_ui_handle(&s,EH_KEY_ENTER);
    assert(s.request==6);
    eh_return_from_launch(&s,0);
    assert(s.page.selected==5 && s.page.page==EH_PAGE_HOME && !s.request);
    eh7_ui_handle(&s,EH_KEY_ENTER);
    eh_return_from_launch(&s,0xDEAD);
    assert(s.notice.text[0] && !s.open_files);
    eh7_ui_handle(&s,EH_KEY_ESC);
    assert(s.page.should_exit);
    puts("PASS launcher return, Files fallback and single handoff");
    return 0;
}
