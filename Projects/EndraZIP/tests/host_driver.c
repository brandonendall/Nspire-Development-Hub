/* Exercise the real application's file operations and event loop. */
#define main endrazip_app_main
#include "../src/endrazip.c"
#undef main

typedef struct { int key, menu_open, listing_open; const char *folder; } step_t;
static const step_t browse_steps[] = {
 {KEY_NSPIRE_ENTER,0,0,"child"},
 {KEY_NSPIRE_LEFT,0,0,"browse"},
 {KEY_NSPIRE_RIGHT,0,0,"child"},
 {KEY_NSPIRE_LEFT,0,0,"browse"},
 {KEY_NSPIRE_MENU,1,0,"browse"},
 {KEY_NSPIRE_MENU,0,0,"browse"},
 {KEY_NSPIRE_MENU,1,0,"browse"},
 {KEY_NSPIRE_ESC,0,0,"browse"},
 {KEY_NSPIRE_MENU,1,0,"browse"},
 {KEY_NSPIRE_ENTER,0,0,"child"},
 {KEY_NSPIRE_ENTER,0,0,"child"}, /* empty folder */
 {KEY_NSPIRE_MENU,0,0,"child"},
 {KEY_NSPIRE_LEFT,0,0,"browse"},
 {KEY_NSPIRE_DOWN,0,0,"browse"},
 {KEY_NSPIRE_ENTER,0,1,"browse"}, /* ZIP contents */
 {KEY_NSPIRE_ESC,0,0,"browse"},
 {KEY_NSPIRE_DOWN,0,0,"browse"},
 {KEY_NSPIRE_ENTER,0,0,"browse"}, /* ordinary file */
 {KEY_NSPIRE_MENU,1,0,"browse"},
 {KEY_NSPIRE_DOWN,1,0,"browse"},
 {KEY_NSPIRE_DOWN,1,0,"browse"},
 {KEY_NSPIRE_ENTER,0,0,"browse"}, /* Create ZIP - Best */
 {KEY_NSPIRE_ESC,0,0,"browse"}
};
static size_t step_index;
static int key_now, pending, checked;
static const char *screen_dir;
static unsigned int screens_saved;

int lcd_init(int type){(void)type;return 1;}
void lcd_blit(const void *buffer,int type){
 (void)buffer;(void)type;
 if(!screen_dir)return;
 unsigned int bit=listing?4U:(menu?2U:1U);
 if(screens_saved&bit)return;
 char path[1024];
 snprintf(path,sizeof(path),"%s/%s.ppm",screen_dir,listing?"contents":(menu?"menu":"browser"));
 FILE *f=fopen(path,"wb");if(!f){perror(path);exit(90);}
 fprintf(f,"P6\n%d %d\n255\n",W,H);
 for(int i=0;i<W*H;i++){
  unsigned short p=pixels[i];
  unsigned char rgb[3]={(unsigned char)(((p>>11)&31)*255/31),
   (unsigned char)(((p>>5)&63)*255/63),(unsigned char)((p&31)*255/31)};
  fwrite(rgb,1,3,f);
 }
 fclose(f);screens_saved|=bit;
}
void wait_key_pressed(void){
 if(step_index>=sizeof(browse_steps)/sizeof(browse_steps[0])){
  fputs("UI did not exit at the expected key\n",stderr);exit(91);
 }
 key_now=browse_steps[step_index].key;pending=1;
}
int isKeyPressed(int key){return key_now==key;}
void wait_no_key_pressed(void){
 if(!pending)return;
 const step_t *s=&browse_steps[step_index];
 const char *base=strrchr(cwd,'/');base=base?base+1:cwd;
 if(menu!=s->menu_open||listing!=s->listing_open||strcmp(base,s->folder)){
  fprintf(stderr,"UI step %lu failed: key=%d menu=%d listing=%d cwd=%s\n",
          (unsigned long)step_index,key_now,menu,listing,cwd);exit(92);
 }
 checked++;step_index++;pending=0;
}
static int fast_create(const char *archive,const char *source){
 mz_zip_archive z={0};const char *base=strrchr(source,'/');base=base?base+1:source;
 if(!mz_zip_writer_init_file(&z,archive,0))return 0;
 int ok=mz_zip_writer_add_file(&z,base,source,NULL,0,MZ_BEST_SPEED)&&mz_zip_writer_finalize_archive(&z);
 mz_zip_writer_end(&z);return ok;
}
int main(int argc,char **argv){
 if(argc!=4)return 2;
 if(!strcmp(argv[1],"create"))return create(argv[3],argv[2])?0:1;
 if(!strcmp(argv[1],"fast"))return fast_create(argv[3],argv[2])?0:1;
 if(!strcmp(argv[1],"extract"))return extract(argv[2],argv[3])?0:1;
 if(!strcmp(argv[1],"ui")){
  if(strlen(argv[2])>=sizeof(cwd))return 2;
  strcpy(cwd,argv[2]);screen_dir=getenv("ENDRAZIP_SCREEN_DIR");
  int result=endrazip_app_main();
  if(step_index!=sizeof(browse_steps)/sizeof(browse_steps[0])-1)return 93;
  if(strncmp(feedback,"ZIP ready - saved ",18))return 94;
  printf("Control states checked: %d; Esc exits\n",checked);
  return result;
 }
 return 2;
}
