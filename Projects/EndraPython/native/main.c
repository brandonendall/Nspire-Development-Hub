#include "app.h"
#include "platform.h"
#include "text_input.h"
#include <libndls.h>
static int consumed_escape;
int cancel_pressed(void){
    if(!isKeyPressed(KEY_NSPIRE_ESC))return 0;
    consumed_escape=1;
    return 1;
}
int main(void){
    volatile int stack_marker=0;
    app_init("/documents/EndraPython",(void *)&stack_marker);
    if(!eh_platform_start())return 1;
    const int keys[]={0,UP,DOWN,LEFT,RIGHT,ENTER,ESC,MENU};
    int redraw=1;
    /* Consume the launch press before offering the first Files action. */
    eh_platform_readkey();eh16_textkey();
    while(!app.quit){
        if(redraw){render(eh_framebuffer());eh_platform_flip();}
        int k=keys[eh_platform_readkey()],ch=eh16_textkey();
        if(consumed_escape){
            if(k==ESC)k=0;
            if(!isKeyPressed(KEY_NSPIRE_ESC))consumed_escape=0;
        }
        if(app.page==EDITOR && k>=UP && k<=RIGHT && isKeyPressed(KEY_NSPIRE_SHIFT))
            k=EXT_UP+(k-UP);
        if(!k){
            if(isKeyPressed(KEY_NSPIRE_CTRL))
                switch(ch){
                case 's': k=SAVE; break; case 'r': k=RUN; break;
                case 'f': k=SEARCH; break; case 'z': k=UNDO; break;
                case 'y': k=REDO; break; case 'a': k=SELECT_ALL; break;
                case 'c': k=COPY; break; case 'x': k=CUT; break;
                case 'v': k=PASTE; break; default: k=0; break;
                }
            else k=ch;
        }
        app_key(k);redraw=k!=0;
        eh_platform_sleep();
    }
    eh_platform_stop();
    return 0;
}
