#include "app.h"
#include "platform.h"
#include <libndls.h>
#include <os.h>
#include <stdio.h>

/* Use the same exported TI Request dialog and parameter layout as the SDK's
 * show_msg_user_input. Read its UTF-16 result before the SDK ASCII conversion. */
void ti_text_input(void){
    static char result[SOURCE_CAP+1];
    eh_platform_stop();
    wait_no_key_pressed();
    String title=string_new(), prompt=string_new(), value=string_new();
    if(title && prompt && value){
        string_set_ascii(title,"TI symbols");
        string_set_ascii(prompt,"Ctrl + book: symbols. Choose, then OK.");
        string_set_ascii(value,"");
        String fields[]={prompt,value};
        if(_show_msgUserInput(0,fields,title->str,prompt->str) && value->str && value->len>0){
            const uint16_t *s=(const uint16_t *)value->str;
            size_t units=0;while(units<SOURCE_CAP && s[units])units++;
            if(!s[units] && utf16_input(s,units,result,sizeof result))app_insert_text(result);
            else snprintf(app.status,sizeof app.status,"Text too long or unsupported control");
        }
    }else snprintf(app.status,sizeof app.status,"Unable to open TI text field");
    if(value)string_free(value);
    if(prompt)string_free(prompt);
    if(title)string_free(title);
    wait_no_key_pressed();
    if(!eh_platform_start())app.quit=1;
}
