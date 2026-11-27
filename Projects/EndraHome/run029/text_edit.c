#include "text_edit.h"
#include "app_target.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int eh_text_editable(const char *path,const char *data,unsigned length){
    static const char *const endings[]={".txt",".txt.tns",".md",".md.tns",".csv",".csv.tns",".json",".json.tns"};
    if(!path||!data||length>EH_TEXT_CAP)return 0;
    size_t n=strlen(path);
    int supported=0;
    for(unsigned i=0;i<sizeof(endings)/sizeof(endings[0]);++i){
        size_t tail=strlen(endings[i]);
        if(n>tail&&eh_same_path(path+n-tail,endings[i]))supported=1;
    }
    if(!supported)return 0;
    for(unsigned i=0;i<length;++i){
        unsigned char c=(unsigned char)data[i];
        if((c<32&&c!='\n'&&c!='\r'&&c!='\t')||c>=127)return 0;
    }
    return 1;
}

void eh_text_begin(EhTextEdit *edit,const char *data,unsigned length){
    memset(edit,0,sizeof(*edit));
    memcpy(edit->original,data,length+1);
    edit->original_len=length;
    edit->crlf=strstr(data,"\r\n")!=NULL;
}

EhTextRow eh_text_row(const char *data,unsigned length,unsigned start){
    unsigned end=start;
    while(end<length&&end-start<EH_TEXT_COLUMNS&&data[end]!='\r'&&data[end]!='\n')++end;
    unsigned next=end;
    if(next<length&&(data[next]=='\n'||data[next]=='\r')){
        if(data[next++]=='\r'&&next<length&&data[next]=='\n')++next;
    }
    return (EhTextRow){start,end,next,end<length};
}

unsigned eh_text_cursor_row(const char *data,unsigned length,unsigned cursor){
    unsigned row=0,start=0;
    for(;;++row){
        EhTextRow line=eh_text_row(data,length,start);
        if(!line.more||cursor<line.next||(cursor==line.end&&line.next>line.end))return row;
        start=line.next;
    }
}

static void reveal(EhTextEdit *edit,const char *data,unsigned length){
    unsigned row=eh_text_cursor_row(data,length,edit->cursor);
    if(row<edit->top)edit->top=row;
    else if(row>=edit->top+EH_TEXT_ROWS)edit->top=row-EH_TEXT_ROWS+1;
}

static unsigned left(const char *data,unsigned cursor){
    if(cursor>=2&&data[cursor-1]=='\n'&&data[cursor-2]=='\r')return cursor-2;
    return cursor?cursor-1:0;
}

void eh_text_move(EhTextEdit *edit,const char *data,unsigned length,EhKey key){
    if(key==EH_KEY_LEFT)edit->cursor=left(data,edit->cursor);
    else if(key==EH_KEY_RIGHT&&edit->cursor<length){
        unsigned i=edit->cursor++;
        if(data[i]=='\r'&&edit->cursor<length&&data[edit->cursor]=='\n')++edit->cursor;
    }else if(key==EH_KEY_UP||key==EH_KEY_DOWN){
        unsigned row=eh_text_cursor_row(data,length,edit->cursor);
        unsigned start=0,prior=0;
        for(unsigned i=0;i<row;++i){
            prior=start;
            start=eh_text_row(data,length,start).next;
        }
        EhTextRow current=eh_text_row(data,length,start);
        if((key==EH_KEY_UP&&row)||(key==EH_KEY_DOWN&&current.more)){
            EhTextRow target=eh_text_row(data,length,key==EH_KEY_UP?prior:current.next);
            unsigned col=edit->cursor-start, width=target.end-target.start;
            if(width&&target.more&&target.next==target.end)--width;
            edit->cursor=target.start+(col<width?col:width);
        }
    }
    reveal(edit,data,length);
}

int eh_text_type(EhTextEdit *edit,char *data,unsigned *length,int ch){
    unsigned at=edit->cursor;
    if(ch==8||ch==127){
        if(!at)return 0;
        unsigned before=left(data,at);
        memmove(data+before,data+at,*length-at+1);
        *length-=at-before;
        edit->cursor=before;
    }else{
        if(ch!='\n'&&(ch<32||ch>126))return 0;
        unsigned count=ch=='\n'&&edit->crlf?2:1;
        if(*length+count>EH_TEXT_CAP)return 0;
        memmove(data+at+count,data+at,*length-at+1);
        if(count==2)data[at++]='\r';
        data[at]=(char)ch;
        edit->cursor+=count;
        *length+=count;
    }
    edit->dirty=*length!=edit->original_len||memcmp(data,edit->original,*length)!=0;
    reveal(edit,data,*length);
    return 1;
}

static int same_file(const char *path,const char *data,unsigned length){
    struct stat st;
    if(stat(path,&st)!=0||!S_ISREG(st.st_mode)||st.st_size!=(long)length)return 0;
    FILE *f=fopen(path,"rb");
    if(!f)return 0;
    unsigned pos=0;
    char chunk[256];
    int same=1;
    while(pos<length){
        unsigned size=length-pos;
        if(size>sizeof(chunk))size=sizeof(chunk);
        if(fread(chunk,1,size,f)!=size||memcmp(chunk,data+pos,size)){same=0;break;}
        pos+=size;
    }
    if(same&&(fgetc(f)!=EOF||ferror(f)))same=0;
    return fclose(f)==0&&same;
}

static int absent(const char *path){
    struct stat st;
    return stat(path,&st)!=0&&errno==ENOENT;
}

const char *eh_text_save(EhTextEdit *edit,const char *path,const char *data,unsigned length){
    if(!edit->dirty)return NULL;
    if(!eh_text_editable(path,data,length))return "TEXT NOT EDITABLE";
    char stage[EH7_PATH_CAP],backup[EH7_PATH_CAP];
    const char *name=strrchr(path,'/');
    if(!name||strlen(name+1)+sizeof(".old.tns")>EH13_NAME_MAX||
       strlen(path)+sizeof(".old.tns")>sizeof(stage))return "FILE PATH TOO LONG TO SAVE";
    snprintf(stage,sizeof(stage),"%s.new.tns",path);
    snprintf(backup,sizeof(backup),"%s.old.tns",path);
    if(!absent(stage)||!absent(backup))return "SAVE BACKUP OR TEMP ALREADY EXISTS";
    if(!same_file(path,edit->original,edit->original_len))return "ORIGINAL CHANGED - SAVE STOPPED";
    FILE *f=fopen(stage,"wb");
    if(!f)return "CANNOT CREATE SAVE TEMP";
    int ok=fwrite(data,1,length,f)==length;
    if(fclose(f)!=0||!ok||!same_file(stage,data,length)){
        remove(stage);
        return "CANNOT VERIFY SAVE TEMP";
    }
    if(!same_file(path,edit->original,edit->original_len)||!absent(backup)){
        remove(stage);
        return "ORIGINAL CHANGED - SAVE STOPPED";
    }
    if(rename(path,backup)!=0){
        remove(stage);
        return "CANNOT BACK UP ORIGINAL";
    }
    if(!same_file(backup,edit->original,edit->original_len)){
        if(absent(path)&&rename(backup,path)==0){
            remove(stage);
            return "ORIGINAL CHANGED - SAVE STOPPED";
        }
        return "ORIGINAL CHANGED - KEEP OLD.TNS";
    }
    if(!absent(path)||rename(stage,path)!=0){
        if(absent(path)&&rename(backup,path)==0){
            remove(stage);
            return "SAVE FAILED - ORIGINAL RESTORED";
        }
        return "SAVE FAILED - KEEP OLD.TNS BACKUP";
    }
    if(!same_file(path,data,length))return "VERIFY FAILED - KEEP OLD.TNS BACKUP";
    memcpy(edit->original,data,length+1);
    edit->original_len=length;
    edit->dirty=0;
    return remove(backup)==0?NULL:"SAVED - OLD.TNS BACKUP KEPT";
}
