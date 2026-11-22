#include "app.h"

/* Source offsets are bytes for MicroPython and disk I/O. The UI counts code points. */
int utf_decode(const char *s,int *bytes){
    const unsigned char *p=(const unsigned char *)s;
    int c=*p;*bytes=1;
    if(c<128)return c;
    int n=c>=0xf0?4:c>=0xe0?3:2;
    if(c<0xc2 || c>0xf4)return -1;
    c&=(1<<(7-n))-1;
    for(int i=1;i<n;i++){
        if((p[i]&0xc0)!=0x80)return -1;
        c=(c<<6)|(p[i]&63);
    }
    if((n==3 && c<0x800)||(n==4 && c<0x10000)||
       (c>=0xd800 && c<=0xdfff)||c>0x10ffff)return -1;
    *bytes=n;return c;
}
int utf_prev(const char *s,int pos){
    if(pos)while(--pos && ((unsigned char)s[pos]&0xc0)==0x80){}
    return pos;
}
int utf_columns(const char *s,int bytes){
    int cols=0;
    for(int i=0;i<bytes;i++)if(((unsigned char)s[i]&0xc0)!=0x80)cols++;
    return cols;
}
int utf_advance(const char *s,int pos,int end,int cols){
    while(pos<end && cols-->0){int n;utf_decode(s+pos,&n);pos+=n;}
    return pos;
}
/* The OS text field is UTF-16. Accept a whole entry or leave the caller unchanged. */
int utf16_input(const uint16_t *s,size_t units,char *out,size_t cap){
    size_t at=0;
    for(size_t i=0;i<units;i++){
        uint32_t c=s[i];
        if(c>=0xd800 && c<=0xdbff){
            if(++i==units || s[i]<0xdc00 || s[i]>0xdfff)return 0;
            c=0x10000+((c-0xd800)<<10)+s[i]-0xdc00;
        }else if(c>=0xdc00 && c<=0xdfff)return 0;
        if(c<32 || c==127)return 0;
        int n=c<128?1:c<0x800?2:c<0x10000?3:4;
        if(at+n>=cap)return 0;
        for(int j=n-1;j>0;j--){out[at+j]=(char)(0x80|(c&63));c>>=6;}
        out[at]=(char)(c|(n==1?0:n==2?0xc0:n==3?0xe0:0xf0));at+=n;
    }
    out[at]=0;return 1;
}
