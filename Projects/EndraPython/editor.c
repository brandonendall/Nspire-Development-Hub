#include "app.h"
#include <string.h>

int line_start(const char *s,int pos) {
    while(pos && s[pos-1]!='\n') --pos;
    return pos;
}
int line_number(const char *s,int pos) {
    int row=0;
    for(int i=0;i<pos;i++) row += s[i]=='\n';
    return row;
}
static int end_of_line(const char *s,int i) {
    while(s[i] && s[i]!='\n') ++i;
    return i;
}
void edit_key(Editor *d,int k) {
    char *s=d->text;
    int pos=d->cursor, size=(int)strlen(s), start=line_start(s,pos), col=pos-start;
    if(k==LEFT && pos) --pos;
    else if(k==RIGHT && pos<size) ++pos;
    else if(k==UP || k==DOWN) {
        int dest=k==UP ? (start ? line_start(s,start-1) : start) : end_of_line(s,pos);
        if(k==DOWN) dest=dest<size ? dest+1 : start;
        int end=end_of_line(s,dest);
        pos=dest+col<end ? dest+col : end;
    } else if(k==8 && pos) {
        memmove(s+pos-1,s+pos,size-pos+1);
        --pos;
        d->dirty=1;
    } else if(k==ENTER || k==INDENT || (k>=32 && k<127)) {
        static char insert[SOURCE_CAP+1];
        int n=1;
        insert[0]=k==ENTER ? '\n' : (char)k;
        if(k==ENTER) {
            while(start+n-1<pos && s[start+n-1]==' ') {insert[n]=' '; ++n;}
        } else if(k==INDENT) { n=4-col%4; memset(insert,' ',n); }
        if(size+n>SOURCE_CAP) {strcpy(app.status,"8 KiB limit reached");return;}
        memmove(s+pos+n,s+pos,size-pos+1);
        memcpy(s+pos,insert,n);
        pos+=n;
        d->dirty=1;
    }
    d->cursor=pos;
    int row=line_number(s,pos);
    col=pos-line_start(s,pos);
    if(row<d->top)d->top=row;
    if(row>=d->top+13)d->top=row-12;
    if(col<d->left)d->left=col;
    if(col>=d->left+34)d->left=col-33;
}
