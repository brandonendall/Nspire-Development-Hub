/* EndraHome Run010: improve dense labels, show config status, same RGB565 UI.
 * Graphics only. Hardware/input remain Run003's unchanged platform.c.
 * Public-domain font8x8_basic.h in UltimateCommander/vendor.
 */
#include "render.h"
#include "font8x8_basic.h"
#include "registry.h"
#include "caption.h"
#include <stdint.h>
#include <string.h>
#define W 320
#define H 240
#define P(r,g,b) ((uint16_t)(((((r)&255)>>3)<<11) | ((((g)&255)>>2)<<5) | (((b)&255)>>3)))
static uint16_t *px;
static const char *defaults[7]={"EndraGBA","EndraZIP","EndraCmd","Files","Images","Tools","Favorites"};
static const EhRegistry *active_apps;
static const char *name_of(int slot){return active_apps?active_apps->slots[slot].label:defaults[slot];}
static int ready(int slot){return active_apps&&active_apps->slots[slot].path[0];}
static const char *descs[7]={"Game Boy Advance","ZIP and archive tools","Ndless command tools","Your files and folders","Image viewer","Nspire utilities","Pinned apps"};
static const int main_rgb[7][3]={{103,61,231},{253,164,13},{20,84,243},{5,192,241},{157,81,211},{32,191,100},{245,72,141}};
static const uint16_t white=P(241,248,255),muted=P(168,183,205),cyan=P(39,199,250);
static void rect(int x,int y,int w,int h,uint16_t c){
 if(x<0){w+=x;x=0;}
 if(y<0){h+=y;y=0;}
 if(x>=W||y>=H||w<=0||h<=0)return;
 if(x+w>W)w=W-x;
 if(y+h>H)h=H-y;
 for(int j=y;j<y+h;j++){uint16_t *row=px+j*W;for(int i=x;i<x+w;i++)row[i]=c;}
}
static void dot(int x,int y,uint16_t c){if((unsigned)x<W&&(unsigned)y<H)px[y*W+x]=c;}
static void line(int x0,int y0,int x1,int y1,uint16_t c){
 int dx=x1>x0?x1-x0:x0-x1,sx=x0<x1?1:-1;
 int dy=-(y1>y0?y1-y0:y0-y1),sy=y0<y1?1:-1,e=dx+dy;
 for(;;){dot(x0,y0,c);if(x0==x1&&y0==y1)break;int e2=2*e;if(e2>=dy){e+=dy;x0+=sx;}if(e2<=dx){e+=dx;y0+=sy;}}
}
static void rr(int x,int y,int w,int h,int r,uint16_t c){
 if(r<1){rect(x,y,w,h,c);return;}
 if(r>w/2)r=w/2;
 if(r>h/2)r=h/2;
 for(int yy=0;yy<h;yy++){
  int t=yy<r?r-yy-1:yy>=h-r?yy-(h-r):0;
  int inset=0;while(inset<r&&(r-inset)*(r-inset)+t*t>r*r)inset++;
  rect(x+inset,y+yy,w-2*inset,1,c);
 }
}
static void border(int x,int y,int w,int h,int r,uint16_t edge,uint16_t fill){
 rr(x,y,w,h,r,edge);rr(x+1,y+1,w-2,h-2,r>1?r-1:1,fill);
}
static void circle(int cx,int cy,int radius,uint16_t c){
 for(int y=-radius;y<=radius;y++)for(int x=-radius;x<=radius;x++)
  if(x*x+y*y<=radius*radius)dot(cx+x,cy+y,c);
}
static void text(int x,int y,const char *s,uint16_t c,int scale,int max){
 for(int i=0;s&&s[i]&&i<max;i++){
  unsigned char ch=(unsigned char)s[i];if(ch>127)ch='?';
  for(int gy=0;gy<8;gy++){
   unsigned char bits=(unsigned char)font8x8_basic[ch][gy];
   for(int gx=0;gx<8;gx++)if(bits&(1u<<gx))
     rect(x+8*scale*i+gx*scale,y+gy*scale,scale,scale,c);
  }
 }
}
/* More compact 5x7 for small icon labels on the physical 320px LCD. */
static void small(int x,int y,const char *s,uint16_t c,int max){
 for(int i=0;s&&s[i]&&i<max;i++){
  unsigned char ch=(unsigned char)s[i];if(ch>127)ch='?';
  for(int gy=0;gy<7;gy++){
   unsigned char bits=(unsigned char)font8x8_basic[ch][gy];
   for(int gx=0;gx<5;gx++)
    if((bits&(1u<<(gx+1)))||(gx==4&&(bits&128)))dot(x+i*5+gx,y+gy,c);
  }
 }
}
static void bg(void){
 for(int y=0;y<H;y++)rect(0,y,W,1,P(9+y/80,12+y/55,22+y/36));
 for(int j=0;j<8;j++)rect(0,31+j,W,1,P(12,19+j/2,29+j));
}
static void tri(int x0,int y0,int x1,int y1,int x2,int y2,uint16_t c){
 int minx=x0,maxx=x0,miny=y0,maxy=y0;
 if(x1<minx)minx=x1;
 if(x2<minx)minx=x2;
 if(x1>maxx)maxx=x1;
 if(x2>maxx)maxx=x2;
 if(y1<miny)miny=y1;
 if(y2<miny)miny=y2;
 if(y1>maxy)maxy=y1;
 if(y2>maxy)maxy=y2;
 if(minx<0)minx=0;
 if(miny<0)miny=0;
 if(maxx>=W)maxx=W-1;
 if(maxy>=H)maxy=H-1;
 int a=(x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);if(!a)return;
 for(int y=miny;y<=maxy;y++)for(int x=minx;x<=maxx;x++){
  int e1=(x1-x0)*(y-y0)-(y1-y0)*(x-x0);
  int e2=(x2-x1)*(y-y1)-(y2-y1)*(x-x1);
  int e3=(x0-x2)*(y-y2)-(y0-y2)*(x-x2);
  if((e1>=0&&e2>=0&&e3>=0)||(e1<=0&&e2<=0&&e3<=0))dot(x,y,c);
 }
}
static void quad(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3,uint16_t c){
 tri(x0,y0,x1,y1,x2,y2,c);tri(x0,y0,x2,y2,x3,y3,c);
}
static void cube(int x,int y){
 quad(x+17,y,x+34,y+10,x+17,y+20,x,y+10,P(28,194,254));
 quad(x,y+12,x+16,y+22,x+16,y+39,x,y+29,P(22,115,248));
 quad(x+18,y+22,x+34,y+12,x+34,y+29,x+18,y+39,P(30,91,224));
 line(x+17,y+20,x+17,y+37,P(8,26,54));
}
static void header(void){
 rr(0,0,W,30,0,P(33,40,57));
 line(0,30,W-1,30,P(17,110,245));line(0,31,W-1,31,P(15,86,186));
 line(11,13,20,6,white);line(20,6,29,13,white);
 line(14,11,14,25,white);line(14,25,26,25,white);line(26,25,26,11,white);
 rect(19,18,3,7,white);
 text(124,10,"EndraHome",white,1,10);
 /* Decorative battery glyph; charge is NOT measured. */
 border(294,7,11,18,1,white,P(32,47,62));rect(297,5,5,2,white);
 for(int i=0;i<4;i++)rect(296,11+i*3,7,2,P(61,197,91));
}
static void title(void){
 cube(51,55);
 text(97,65,"Endra",white,2,5);
 text(177,65,"Home",cyan,2,4);
 text(111,89,"Custom Launcher",muted,1,15);
 line(31,118,105,118,P(29,96,175));
 line(232,118,288,118,P(29,96,175));
 text(112,112,"Choose an app.",white,1,14);
}
static void shell(int i,int x,int y){
 int red=main_rgb[i][0],gr=main_rgb[i][1],bl=main_rgb[i][2];
 rr(x-1,y+2,36,40,7,P(red/4,gr/4,bl/4));
 for(int row=0;row<39;row++){
  int r=red+(39-row)/4,g=gr+(39-row)/4,b=bl+(39-row)/6;
  if(r>255)r=255;
  if(g>255)g=255;
  if(b>255)b=255;
  int inset=0;
  if(row<6)inset=(5-row)/2;
  if(row>32)inset=(row-32)/2;
  rect(x+inset,y+row,34-inset*2,1,P(r,g,b));
 }
 rr(x+3,y+3,28,1,3,P(233,233,255));
}
static void gamepad(int x,int y){
 rr(x+5,y+13,25,15,5,P(232,242,255));
 rr(x+7,y+15,21,11,4,P(95,55,194));
 rect(x+11,y+19,8,2,white);rect(x+14,y+16,2,8,white);
 circle(x+23,y+18,2,P(242,183,245));circle(x+26,y+23,2,P(224,199,255));
}
static void zip(int x,int y){
 rr(x+10,y+7,16,24,3,P(255,239,192));
 rr(x+12,y+10,12,19,2,P(240,185,70));
 rect(x+17,y+12,3,14,P(248,250,249));
 for(int j=0;j<4;j++)rect(x+17+(j%2)*2,y+12+j*3,2,2,P(156,98,20));
 rr(x+15,y+25,7,5,2,white);
}
static void terminal(int x,int y){
 line(x+6,y+13,x+16,y+20,white);
 line(x+16,y+20,x+6,y+27,white);
 rect(x+18,y+28,11,2,white);
}
static void folder(int x,int y){
 rr(x+5,y+14,25,17,3,P(227,245,255));
 rr(x+5,y+11,12,7,3,P(217,246,255));
 rr(x+7,y+17,21,12,2,P(67,185,229));
}
static void picture(int x,int y){
 rr(x+5,y+10,25,23,2,white);
 rr(x+8,y+13,19,17,1,P(137,90,219));
 circle(x+23,y+18,3,P(235,210,255));
 tri(x+10,y+28,x+17,y+19,x+24,y+28,P(228,234,255));
}
static void tool(int x,int y){
 for(int j=-2;j<=2;j++){
  line(x+9+j,y+28,x+26+j,y+11,white);
  line(x+9+j,y+12,x+26+j,y+29,P(228,249,237));
 }
 circle(x+9,y+29,3,P(241,255,246));
 circle(x+26,y+11,3,P(241,255,246));
}
static void star(int x,int y){
 int vx[]={17,21,31,24,26,17,8,10,3,13};
 int vy[]={9,18,18,24,33,28,33,24,18,18};
 for(int i=0;i<10;i++)
   line(x+vx[i],y+vy[i],x+vx[(i+1)%10],y+vy[(i+1)%10],white);
}
static void icon(int index,int x,int y){
 shell(index,x,y);
 if(index==0)gamepad(x,y);
 if(index==1)zip(x,y);
 if(index==2)terminal(x,y);
 if(index==3)folder(x,y);
 if(index==4)picture(x,y);
 if(index==5)tool(x,y);
 if(index==6)star(x,y);
}
static void item(int index,int chosen){
 int x=8+index*44, y=143;
 if(chosen){
  border(x-3,y-4,42,47,10,P(16,63,81),P(12,32,49));
  border(x-2,y-3,40,45,9,P(24,126,196),P(13,33,52));
  border(x-1,y-2,38,43,8,P(118,235,255),P(22,50,76));
 }
 icon(index,x+1,y);
 /* Captions are limited to six glyphs per row; complete names on details. */
 char first[7],second[7];
 eh8_caption(name_of(index),first,second);
 int n=(int)strlen(first);
 small(x+18-n*5/2,190,first,white,n);
 if(second[0]){
  int m=(int)strlen(second);
  small(x+18-m*5/2,199,second,white,m);
 }
 /* Quiet cyan dot: shortcut is configured; gray dot: not configured. */
 rect(x+16,209,4,2,ready(index)?P(80,234,202):P(81,94,109));
}
static void foot(void){
 rr(11,213,298,22,9,P(34,41,59));
 border(12,214,296,20,8,P(46,66,88),P(33,39,56));
 border(52,217,36,15,5,P(65,201,248),P(20,45,62));
 small(57,222,"Enter",P(111,221,248),5);
 text(92,221,"Open",white,1,4);
 rect(164,217,1,13,P(35,139,207));
 border(185,217,36,15,5,P(65,201,248),P(20,45,62));
 small(191,222,"Menu",P(111,221,248),4);
 text(226,221,"Options",white,1,7);
}
static void home(const EhUI7 *s){
 bg();header();title();
 if(active_apps){
  if(!active_apps->file_loaded)
   text(14,127,"CONFIG FILE MISSING",P(255,164,111),1,24);
  else if(active_apps->errors)
   text(14,127,"CHECK CONFIG FILE",P(255,164,111),1,24);
  else {
   char status[]="0 / 7 READY";
   int count=active_apps->enabled;
   if(count<0)count=0;
   if(count>7)count=7;
   status[0]=(char)('0'+count);
   text(14,127,status,P(111,228,208),1,11);
  }
 }
 for(int i=0;i<7;i++)item(i,i==s->selected);
 foot();
}
static void subpage(const EhUI7 *s){
 bg();header();
 rr(14,43,292,159,11,P(25,39,61));
 border(17,46,286,153,10,P(48,83,112),P(25,39,61));
 if(s->page==EH_PAGE_HELP){
  text(25,55,"SHORTCUT SETUP",cyan,2,14);
  if(!active_apps||!active_apps->file_loaded)
   text(25,81,"CONFIG: FILE NOT FOUND",P(255,175,109),1,28);
  else if(active_apps->errors)
   text(25,81,"CONFIG: INVALID LINE(S)",P(255,175,109),1,28);
  else
   text(25,81,"CONFIG: FILE LOADED",P(110,231,207),1,28);
  text(25,101,EH7_CONFIG_FILE,white,1,31);
  text(25,119,"Beside EndraHome Run010",muted,1,30);
  text(25,145,"ARROWS MOVE / ENTER OPEN",white,1,31);
  text(25,165,"ESC BACK / MENU HELP",white,1,30);
  text(25,184,"Apps exit to TI Documents",muted,1,30);
 }else{
  icon(s->selected,29,62);
  text(77,71,name_of(s->selected),white,1,14);
  text(29,112,ready(s->selected)?"Shortcut configured":descs[s->selected],muted,1,29);
  if(s->page==EH_PAGE_DETAIL){
   text(29,142,ready(s->selected)?"READY - ENTER TO LAUNCH":"SET PATH IN CONFIG FILE",cyan,1,29);
   text(29,165,"ENTER SELECT  ESC BACK",white,1,27);
  }else{
   text(29,142,"GBA-based navigation core",cyan,1,28);
   text(29,165,"ENTER / ESC  Back",white,1,22);
  }
 }
 foot();
}
void eh7_render(uint16_t *frame,const EhUI7 *s,const EhRegistry *apps){
 if(!frame||!s||!apps)return;
 px=frame;active_apps=apps;
 if(s->page==EH_PAGE_HOME)home(s);
 else subpage(s);
}
