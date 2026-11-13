#include "nx.h"
#include "miniz.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"FAIL line %d: %s (%s)\n",__LINE__,#x,nx_error);exit(1);} } while(0)
static void write_text(const char *path,const char *text){FILE *f=fopen(path,"wb");CHECK(f);CHECK(fwrite(text,1,strlen(text),f)==strlen(text));CHECK(fclose(f)==0);}
static int equal_text(const char *path,const char *text){char buf[128]={0};FILE *f=fopen(path,"rb");if(!f)return 0;size_t n=fread(buf,1,sizeof(buf)-1,f);fclose(f);return n==strlen(text)&&!strcmp(buf,text);}
static void malicious_zip(const char *path,const char *name){mz_zip_archive z={0};CHECK(mz_zip_writer_init_file(&z,path,0));CHECK(mz_zip_writer_add_mem(&z,name,"bad",3,0));CHECK(mz_zip_writer_finalize_archive(&z));CHECK(mz_zip_writer_end(&z));}
static int cancel_copy(const char *p,uint64_t n){(void)p;(void)n;return 0;}
static char concurrent_destination[NX_PATH];
static int create_destination(const char *p,uint64_t n){(void)p;(void)n;write_text(concurrent_destination,"keep this file");return 1;}
int main(void){
 char base[]="/tmp/ultracommander-test-XXXXXX",a[NX_PATH],b[NX_PATH],c[NX_PATH],d[NX_PATH],zip[NX_PATH],out[NX_PATH];CHECK(mkdtemp(base));strcpy(nx_write_root,base);
 CHECK(nx_normalize(a,"/"));CHECK(!strcmp(a,"/"));
 CHECK(nx_normalize(a,"/a/b/../../"));CHECK(!strcmp(a,"/"));
 CHECK(nx_normalize(a,"/a///b/../c/"));CHECK(!strcmp(a,"/a/c"));
 strcpy(a,"/x/y/../z");CHECK(nx_normalize(a,a));CHECK(!strcmp(a,"/x/z"));
 CHECK(nx_normalize(a,"/documents/a/../b//c/./"));CHECK(!strcmp(a,"/documents/b/c"));CHECK(!nx_normalize(a,"/../../escape"));CHECK(!nx_normalize(a,"relative/file"));CHECK(!nx_name_valid("../x"));CHECK(!nx_name_valid("bad/name"));CHECK(!nx_name_valid("."));CHECK(!nx_name_valid("x "));CHECK(nx_name_valid("My notes.txt.tns"));CHECK(!nx_writable("/system/file"));CHECK(!nx_writable(base));
 CHECK(nx_join(a,base,"source"));CHECK(nx_mkdir(a));CHECK(nx_join(b,a,"notes.txt.tns"));write_text(b,"hello\nworld\n");CHECK(nx_kind(b)==1);
 CHECK(nx_join(c,base,"copy.txt.tns"));CHECK(nx_copy(b,c));CHECK(equal_text(c,"hello\nworld\n"));CHECK(!nx_copy(b,c));CHECK(equal_text(c,"hello\nworld\n"));CHECK(!nx_copy(b,b));
 CHECK(nx_join(d,base,"moved.txt.tns"));CHECK(nx_move(c,d));CHECK(!nx_exists(c));CHECK(equal_text(d,"hello\nworld\n"));CHECK(!nx_move(b,d));CHECK(nx_exists(b));
 CHECK(nx_join(c,a,"nested"));CHECK(nx_mkdir(c));CHECK(nx_join(c,a,"nested/data.bin" )==0);
 CHECK(nx_join(c,a,"nested"));CHECK(nx_join(c,c,"data.bin"));write_text(c,"binary");CHECK(nx_join(c,base,"treecopy"));CHECK(nx_copy(a,c));CHECK(nx_join(d,c,"nested"));CHECK(nx_join(d,d,"data.bin"));CHECK(equal_text(d,"binary"));CHECK(!nx_delete(c));CHECK(nx_exists(d));
 CHECK(nx_join(c,a,"badchild"));CHECK(!nx_copy(a,c));CHECK(!nx_exists(c));
 CHECK(nx_join(c,base,"cancelled.txt"));nx_progress=cancel_copy;CHECK(!nx_copy(b,c));nx_progress=NULL;CHECK(!nx_exists(c));CHECK(strlen(c)+8<sizeof(d));strcpy(d,c);strcat(d,".nxpart");CHECK(!nx_exists(d));
 CHECK(nx_join(c,base,"cancelled-tree"));nx_progress=cancel_copy;CHECK(!nx_copy(a,c));nx_progress=NULL;CHECK(!nx_exists(c));
 CHECK(nx_join(c,base,"concurrent.txt"));strcpy(concurrent_destination,c);nx_progress=create_destination;CHECK(!nx_copy(b,c));nx_progress=NULL;CHECK(equal_text(c,"keep this file"));CHECK(unlink(c)==0);
 CHECK(snprintf(c,sizeof(c),"%s/missing/../normalized",base)<(int)sizeof(c));CHECK(nx_mkdir(c));CHECK(nx_delete(c));
 CHECK(nx_join(c,base,"link"));CHECK(symlink("/etc/passwd",c)==0);CHECK(nx_join(d,base,"linked-copy"));CHECK(!nx_copy(c,d));CHECK(!nx_exists(d));CHECK(!nx_writable(c));CHECK(unlink(c)==0);
 NxPane forbidden={0};strcpy(forbidden.path,"/");CHECK(!nx_scan(&forbidden));CHECK(strstr(nx_error,"Documents")!=NULL);
 NxPane pane={0};strcpy(pane.path,base);CHECK(nx_scan(&pane));CHECK(pane.count==3);CHECK(pane.items[0].directory);pane.folders_only=1;CHECK(nx_scan(&pane));for(size_t i=0;i<pane.count;++i)CHECK(pane.items[i].directory);strcpy(pane.filter,"source");CHECK(nx_scan(&pane));CHECK(pane.count==1);CHECK(!strcmp(pane.items[0].name,"source"));nx_release(&pane);
 CHECK(nx_join(zip,base,"backup.zip.tns"));CHECK(nx_zip_create(a,zip));CHECK(nx_kind(zip)==3);CHECK(!nx_zip_create(a,zip));CHECK(nx_join(out,base,"unpacked"));CHECK(nx_zip_extract(zip,out));CHECK(nx_join(c,out,"source"));CHECK(nx_join(c,c,"notes.txt.tns"));CHECK(equal_text(c,"hello\nworld\n"));CHECK(!nx_zip_extract(zip,out));
 char listing[4096];CHECK(nx_zip_list(zip,listing,sizeof(listing)));CHECK(strstr(listing,"notes.txt.tns")!=NULL);
 CHECK(nx_zip_entry_valid("a/b.txt.tns"));CHECK(nx_zip_entry_valid("folder/"));const char *bad[]={"../escape","/absolute","a/../../x","a//x","a\\x","C:/x","x/./a","a/..","a. /x","a./x"};for(size_t i=0;i<sizeof(bad)/sizeof(bad[0]);++i)CHECK(!nx_zip_entry_valid(bad[i]));
 CHECK(nx_join(zip,base,"unsafe.zip"));malicious_zip(zip,"../escaped.txt");CHECK(nx_join(out,base,"unsafe-output"));CHECK(!nx_zip_extract(zip,out));CHECK(!nx_exists(out));CHECK(strlen(out)+8<sizeof(c));strcpy(c,out);strcat(c,".nxpart");CHECK(!nx_exists(c));
 CHECK(nx_join(zip,base,"duplicate.zip"));{mz_zip_archive z={0};CHECK(mz_zip_writer_init_file(&z,zip,0));CHECK(mz_zip_writer_add_mem(&z,"same.txt","first",5,0));CHECK(mz_zip_writer_add_mem(&z,"same.txt","second",6,0));CHECK(mz_zip_writer_finalize_archive(&z));mz_zip_writer_end(&z);}CHECK(nx_join(out,base,"duplicate-output"));CHECK(!nx_zip_extract(zip,out));CHECK(!nx_exists(out));CHECK(strlen(out)+8<sizeof(c));strcpy(c,out);strcat(c,".nxpart");CHECK(!nx_exists(c));
 CHECK(nx_delete(b));CHECK(!nx_exists(b));printf("PASS: %u checks. Fixtures: %s\n",checks,base);return 0;
}
