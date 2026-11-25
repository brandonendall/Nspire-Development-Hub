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
#define HISTORY_DEPTH 12
typedef struct {char text[SOURCE_CAP+1]; int pos,anchor,selecting;} Revision;
static Revision history[HISTORY_DEPTH],exchange;
static int slot,undos,redos;
static char clipboard[SOURCE_CAP+1];

void edit_reset_history(void){slot=undos=redos=0;}
static void record(Revision *r,const Editor *d){
    strcpy(r->text,d->text);r->pos=d->cursor;r->anchor=d->anchor;r->selecting=d->selecting;
}
static void reveal(Editor *d){
    int row=line_number(d->text,d->cursor), start=line_start(d->text,d->cursor), col=utf_columns(d->text+start,d->cursor-start);
    if(row<d->top)d->top=row;
    if(row>=d->top+EDIT_ROWS)d->top=row-EDIT_ROWS+1;
    if(col<d->left)d->left=col;
    if(col>=d->left+EDIT_COLS)d->left=col-EDIT_COLS+1;
}
static int changed(const Editor *d){
    const char *src=d->original,*text=d->text;
    if(!d->existing)return 1;
    while(*src){
        char c=*src++;
        if(c=='\r'){if(*src=='\n')++src;c='\n';}
        if(*text++!=c)return 1;
    }
    return *text!=0;
}
static void restore(Editor *d,int back){
    if(back?!undos:!redos){strcpy(app.status,back?"Nothing to undo":"Nothing to redo");return;}
    if(back)slot=(slot+HISTORY_DEPTH-1)%HISTORY_DEPTH;
    Revision *r=&history[slot];record(&exchange,d);
    strcpy(d->text,r->text);d->cursor=r->pos;d->anchor=r->anchor;d->selecting=r->selecting;
    *r=exchange;
    if(back){--undos;++redos;}else{slot=(slot+1)%HISTORY_DEPTH;++undos;--redos;}
    d->dirty=changed(d);reveal(d);
}
void edit_find(Editor *d,const char *query){
    if(!*query)return;
    const char *found=strstr(d->text+d->cursor,query);
    if(!found)found=strstr(d->text,query);
    if(!found){strcpy(app.status,"No match");return;}
    d->anchor=(int)(found-d->text);d->cursor=d->anchor+(int)strlen(query);
    d->selecting=1;reveal(d);
}
static void replace_text(Editor *d,int a,int b,const char *data,int n){
    int size=strlen(d->text);
    if(size-(b-a)+n>SOURCE_CAP){strcpy(app.status,"8 KiB limit reached");return;}
    record(&history[slot],d);slot=(slot+1)%HISTORY_DEPTH;if(undos<HISTORY_DEPTH)undos++;redos=0;
    memmove(d->text+a+n,d->text+b,size-b+1);memcpy(d->text+a,data,n);
    d->cursor=a+n;d->selecting=0;d->dirty=changed(d);reveal(d);
}
void edit_insert(Editor *d,const char *value){
    int a=d->cursor,b=a;
    if(d->selecting){if(d->anchor<a)a=d->anchor;else b=d->anchor;}
    replace_text(d,a,b,value,(int)strlen(value));
}
static int identifier(char c){
    return (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_' || (unsigned char)c>=128;
}

static int header_at(const char *text,int at,int end){
    static const char *const keywords[]={"def","class","if","elif","else","for","while",
        "try","except","finally","with","async","match","case"};
    for(size_t i=0;i<sizeof keywords/sizeof keywords[0];i++){
        int len=(int)strlen(keywords[i]);
        if(end-at>=len && !memcmp(text+at,keywords[i],len) &&
           (at+len==end || !identifier(text[at+len])))return 1;
    }
    return 0;
}

/* Return the start of a completed compound header, ignoring literals,
 * comments, and colons within brackets. */
static int block_header(const char *s,int end){
    int start=0, depth=0, quote=0, triple=0, last=0;
    for(int i=0;i<end;i++){
        char c=s[i];
        if(quote){
            if(c=='\\' && i+1<end){i++;continue;}
            if(c==quote && (!triple || (i+2<end && s[i+1]==c && s[i+2]==c))){
                if(triple)i+=2;
                quote=triple=0;
            }else if(c=='\n' && !triple){quote=0;start=i+1;last=0;}
            continue;
        }
        if(c=='#'){
            while(i+1<end && s[i+1]!='\n')i++;
        }else if(c=='\'' || c=='"'){
            quote=c;
            triple=i+2<end && s[i+1]==c && s[i+2]==c;
            if(triple)i+=2;
            last=c;
        }else if(c=='\\' && i+1<end && s[i+1]=='\n'){
            i++;
        }else if(c=='\n'){
            if(!depth){start=i+1;last=0;}
        }else if(c!=' ' && c!='\t'){
            if(c=='(' || c=='[' || c=='{')depth++;
            else if((c==')' || c==']' || c=='}') && depth)depth--;
            last=c;
        }
    }
    if(depth || quote || last!=':')return -1;
    int first=start;
    while(first<end && (s[first]==' ' || s[first]=='\t'))first++;
    return header_at(s,first,end)?start:-1;
}

void edit_key(Editor *d,int k){
    int pos=d->cursor,size=strlen(d->text),start=line_start(d->text,pos);
    int extend=k>=EXT_UP && k<=EXT_RIGHT;
    if(extend)k=UP+(k-EXT_UP);
    if(k==UNDO || k==REDO){restore(d,k==UNDO);return;}
    if(k==SELECT_ALL){d->selecting=1;d->anchor=0;d->cursor=size;reveal(d);return;}
    int a=pos,b=pos;
    if(d->selecting){if(d->anchor<pos)a=d->anchor;else b=d->anchor;}
    if(k==UP||k==DOWN||k==LEFT||k==RIGHT){
        if(extend && !d->selecting){d->selecting=1;d->anchor=pos;}
        if(!extend)d->selecting=0;
        if(k==LEFT)pos=(!extend && a<b)?a:utf_prev(d->text,pos);
        else if(k==RIGHT)pos=(!extend && a<b)?b:utf_advance(d->text,pos,size,1);
        else {
            int dest=k==UP?(start?line_start(d->text,start-1):start):end_of_line(d->text,pos);
            if(k==DOWN)dest=dest<size?dest+1:start;
            int end=end_of_line(d->text,dest);
            pos=utf_advance(d->text,dest,end,utf_columns(d->text+start,pos-start));
        }
        d->cursor=pos;reveal(d);return;
    }
    if(k==COPY || k==CUT){
        if(a==b){strcpy(app.status,"Hold Shift and use arrows to select");return;}
        memcpy(clipboard,d->text+a,b-a);clipboard[b-a]=0;
        if(k==COPY){strcpy(app.status,"Copied");return;}
    }
    static char insert[SOURCE_CAP+1];
    const char *data=insert;int n=0;
    if(k==PASTE){data=clipboard;n=strlen(data);if(!n){strcpy(app.status,"Clipboard empty");return;}}
    else if(k==8 || k==CUT){if(a==b){if(!a)return;a=utf_prev(d->text,a);}}
    else if(k==ENTER){
        insert[n++]='\n';
        int block=valid_script(d->name)?block_header(d->text,a):-1;
        start=block>=0?block:line_start(d->text,a);
        while(start<a && (d->text[start]==' ' || d->text[start]=='\t'))
            insert[n++]=d->text[start++];
        if(block>=0 && n<=SOURCE_CAP-1){
            insert[n++]=' ';
            insert[n++]=' ';
        }
    }else if(k==INDENT){
        const int indent_width=2;
        start=line_start(d->text,a);
        n=indent_width-utf_columns(d->text+start,a-start)%indent_width;
        memset(insert,' ',n);
    }
    else if(k>=32 && k<127)insert[n++]=(char)k;
    else return;
    replace_text(d,a,b,data,n);
}

void edit_goto_line(Editor *d,int line){
    int p=0;
    while(line>1 && d->text[p])if(d->text[p++]=='\n')line--;
    d->cursor=p;d->selecting=0;reveal(d);
}

int edit_replace_all(Editor *d, const char *query, const char *replacement) {
    if (!*query) return 0;
    static char result[SOURCE_CAP + 1];
    size_t needle = strlen(query), substitute = strlen(replacement), used = 0;
    int count = 0;
    const char *start = d->text;
    for (;;) {
        const char *hit = strstr(start, query);
        size_t span = hit ? (size_t)(hit - start) : strlen(start);
        if (used + span + (hit ? substitute : 0) > SOURCE_CAP) return -1;
        memcpy(result + used, start, span);
        used += span;
        if (!hit) break;
        memcpy(result + used, replacement, substitute);
        used += substitute;
        start = hit + needle;
        count++;
    }
    result[used] = 0;
    if (count && strcmp(result, d->text)) {
        int line = line_number(d->text, d->cursor) + 1;
        replace_text(d, 0, (int)strlen(d->text), result, (int)used);
        edit_goto_line(d, line);
    }
    return count;
}

/* Outline recognizes declarations at the start of a physical line.
 * Skip quoted strings, including multiline docstrings, and comments. */
int edit_outline(const Editor *d, int after, int direction, char *label, size_t size) {
    int line = 1, chosen = 0;
    const char *entry = NULL, *entry_end = NULL;
    char quote = 0;
    int triple = 0;
    const char *start = d->text;
    while (*start) {
        const char *end = start + end_of_line(start, 0);
        const char *p = start;
        while (*p == ' ' || *p == '\t') p++;
        if (!quote && (direction > 0 ? line > after : line < after) &&
            (!strncmp(p, "def ", 4) || !strncmp(p, "class ", 6) || !strncmp(p, "async def ", 10))) {
            chosen = line;
            entry = p;
            entry_end = end;
            if (direction > 0) break;
        }
        int continued = 0;
        for (p = start; p < end; p++) {
            char c = *p;
            if (quote && c == '\\') {
                if (p + 1 < end) p++;
                else continued = 1;
            } else if (quote && c == quote) {
                if (!triple || (p + 2 < end && p[1] == c && p[2] == c)) {
                    if (triple) p += 2;
                    quote = 0;
                }
            } else if (!quote && c == '#') break;
            else if (!quote && (c == '\'' || c == '"')) {
                quote = c;
                triple = p + 2 < end && p[1] == c && p[2] == c;
                if (triple) p += 2;
            }
        }
        if (!triple && !continued) quote = 0;
        start = *end ? end + 1 : end;
        line++;
    }
    if (label && size) {
        size_t n = entry ? (size_t)(entry_end - entry) : 0;
        if (n >= size) {
            n = size - 1;
            while (n && ((unsigned char)entry[n] & 0xc0) == 0x80) n--;
        }
        if (n) memcpy(label, entry, n);
        label[n] = 0;
    }
    return chosen;
}
