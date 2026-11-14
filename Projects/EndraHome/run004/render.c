/*
 * EndraHome Run004 -- visual-only replacement for Run003 renderer.
 * No hardware, input, filesystem, external app or OS calls here.
 * Uses public-domain font8x8_basic.h (in UltimateCommander/vendor).
 */
#include "render.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "font8x8_basic.h"
#define W 320
#define H 240
#define C(r,g,b) ((uint16_t)((((r)&255)>>3)<<11 | (((g)&255)>>2)<<5 | (((b)&255)>>3)))
typedef uint16_t pixel;
static pixel *screen;
static const char *names[4]={"GBA","Files","ZIP","Apps"};
static const char *long_names[4]={"EndraGBA","EndraFiles","EndraZIP","Applications"};
static const char *descs[4]={
  "Your Game Boy Advance library",
  "Browse and manage files",
  "Archives and compression",
  "Your Ndless application hub"
};
static void rect(int x,int y,int w,int h,pixel c){
  if(x<0){w+=x;x=0;}
  if(y<0){h+=y;y=0;}
  if(x>=W||y>=H||w<=0||h<=0)return;
  if(x+w>W)w=W-x;
  if(y+h>H)h=H-y;
  for(int yy=y;yy<y+h;yy++){
    pixel *row=screen+yy*W;
    for(int xx=x;xx<x+w;xx++)row[xx]=c;
  }
}
static void rr(int x,int y,int w,int h,int r,pixel color){
  if(r<1){rect(x,y,w,h,color);return;}
  for(int yy=y;yy<y+h;yy++){
    int left=x,right=x+w-1;
    int dy=0;
    if(yy<y+r)dy=y+r-1-yy;
    else if(yy>=y+h-r)dy=yy-(y+h-r);
    if(dy){
      int inset=0;
      while(inset<r && (r-inset)*(r-inset)+dy*dy>r*r)inset++;
      left+=inset;right-=inset;
    }
    rect(left,yy,right-left+1,1,color);
  }
}
static void outline(int x,int y,int w,int h,int r,pixel border,pixel fill){
  rr(x,y,w,h,r,border);
  rr(x+2,y+2,w-4,h-4,r>2?r-2:1,fill);
}
static void glyph(int x,int y,unsigned char ch,pixel col,int scale){
  if(ch>127)ch='?';
  for(int yy=0;yy<8;yy++){
    unsigned char mask=(unsigned char)font8x8_basic[ch][yy];
    for(int xx=0;xx<8;xx++)if(mask&(1u<<xx))
      rect(x+xx*scale,y+yy*scale,scale,scale,col);
  }
}
static void label(int x,int y,const char *s,pixel col,int scale,int max){
  if(!s)return;
  for(int i=0;s[i]&&i<max;i++){
    int xx=x+i*8*scale;
    if(xx+8*scale>W)break;
    glyph(xx,y,(unsigned char)s[i],col,scale);
  }
}
static pixel muted(void){return C(139,163,188);}
static pixel white(void){return C(238,247,251);}
static pixel mint(void){return C(85,238,214);}
static void background(void){
  /* Deliberately subtle vertical RGB565 gradient; no allocations or libraries. */
  for(int y=0;y<H;y++){
    int r=8+y/48,g=15+y/32,b=28+y/20;
    rect(0,y,W,1,C(r,g,b));
  }
  for(int k=0;k<9;k++){
    pixel col=C(12+k,30+2*k,47+2*k);
    rr(226-k*3,43-k,95+k*4,48+k*3,20,col);
  }
  rect(0,0,W,35,C(14,26,43));
  rect(0,34,W,1,C(39,63,80));
}
static void brand(void){
  rr(12,7,23,22,6,C(42,192,185));
  rr(15,10,17,16,4,C(13,43,53));
  label(19,14,"E",mint(),1,1);
  label(43,15,"ENDRA",white(),1,5);
  label(85,15,"HOME",mint(),1,4);
  rr(267,9,40,19,6,C(30,56,74));
  label(272,14,"R004",muted(),1,4);
}
static void gamepad(int x,int y){
  rr(x+5,y+12,40,25,8,C(24,146,161));
  rr(x+7,y+13,36,22,7,C(77,214,209));
  rr(x+10,y+16,30,16,5,C(29,73,87));
  rect(x+14,y+21,11,3,C(202,252,245));
  rect(x+18,y+17,3,11,C(202,252,245));
  rr(x+32,y+19,5,5,2,C(239,145,180));
  rr(x+38,y+24,5,5,2,C(244,203,111));
}
static void folder_icon(int x,int y){
  rr(x+8,y+16,35,27,5,C(60,144,226));
  rr(x+8,y+13,17,9,4,C(93,180,239));
  rr(x+11,y+20,30,19,3,C(43,117,208));
  rect(x+15,y+26,22,2,C(160,223,255));
  rect(x+15,y+31,16,2,C(160,223,255));
}
static void zip_icon(int x,int y){
  rr(x+12,y+11,29,33,5,C(151,109,220));
  rr(x+15,y+13,23,28,3,C(103,76,159));
  rect(x+24,y+15,4,22,C(209,190,253));
  for(int t=0;t<4;t++)rect(x+24+(t%2)*2,y+17+t*5,3,3,C(99,77,149));
  rr(x+21,y+32,10,7,2,C(226,210,255));
}
static void apps_icon(int x,int y){
  const pixel colors[]={C(78,213,205),C(115,156,243),C(246,181,92),C(163,113,235)};
  for(int i=0;i<4;i++){
    int xx=x+11+(i%2)*19,yy=y+11+(i/2)*19;
    rr(xx,yy,15,15,4,colors[i]);
    rr(xx+3,yy+3,9,9,2,C(28,51,78));
  }
}
static void icon(int index,int x,int y){
  if(index==0)gamepad(x,y);
  else if(index==1)folder_icon(x,y);
  else if(index==2)zip_icon(x,y);
  else apps_icon(x,y);
}
static void dock_tile(int i,int selected){
  int x=11+i*77,y=135;
  rr(x+2,y+4,68,75,10,C(5,13,23));
  outline(x,y,71,74,10,selected?mint():C(45,71,91),selected?C(23,63,74):C(23,38,57));
  rr(x+12,y+8,47,44,9,selected?C(23,82,88):C(33,52,76));
  icon(i,x+12,y+7);
  int n=(int)strlen(names[i]);
  label(x+(71-n*8)/2,y+56,names[i],selected?mint():white(),1,n);
  if(selected)rr(x+30,y+69,11,2,1,mint());
}
static void footer(void){
  rect(0,219,320,21,C(12,24,40));
  rect(0,219,320,1,C(33,63,78));
  label(8,227,"ARROWS MOVE",muted(),1,11);
  label(127,227,"ENTER OPEN",mint(),1,10);
  label(243,227,"MENU HELP",muted(),1,9);
}
static void home(const EhUI *s){
  rr(12,46,296,76,12,C(19,43,61));
  rr(14,48,292,72,11,C(20,41,59));
  label(23,59,"One home.",white(),2,9);
  label(24,89,"Every tool within reach.",muted(),1,25);
  label(24,106,"TI-NSPIRE  /  NDLESS",mint(),1,22);
  /* Quiet launch-stack motif in the right-hand side. */
  rr(267,64,22,27,5,C(55,106,128));
  rr(255,58,22,27,5,C(72,151,169));
  rr(259,62,14,19,3,C(17,56,69));
  label(15,124,"QUICK ACCESS",mint(),1,12);
  label(258,124,"4 APPS",muted(),1,6);
  for(int i=0;i<4;i++)dock_tile(i,i==s->selected);
  footer();
}
static void detail(const EhUI *s,int about){
  rr(12,47,296,163,12,C(22,44,63));
  label(25,61,about?"APP INFORMATION":"QUICK ACCESS",mint(),1,23);
  rr(23,81,72,68,12,C(32,66,82));
  icon(s->selected,34,88);
  label(109,92,long_names[s->selected],white(),1,18);
  label(109,112,about?"SYSTEM MODULE":"APP SHORTCUT",muted(),1,18);
  rr(109,130,122,18,6,C(40,78,85));
  label(115,135,"PREVIEW ONLY",mint(),1,12);
  label(25,161,about?"Navigation engine: GBA":"NO EXTERNAL LAUNCH YET",white(),1,31);
  label(25,179,about?"Same working Run003 core":descs[s->selected],muted(),1,33);
  label(24,196,about?"ENTER/ESC BACK  MENU HELP":"ENTER INFO  ESC BACK",mint(),1,33);
  footer();
}
static void help(void){
  rr(12,47,296,163,12,C(22,44,63));
  label(24,60,"CONTROLS",mint(),1,20);
  const char *labels[]={"ARROWS","ENTER","ESC","MENU"};
  const char *actions[]={"SELECT A TILE","OPEN / NEXT PAGE","BACK / EXIT","TOGGLE HELP"};
  for(int i=0;i<4;i++){
    int y=82+i*28;
    rr(22,y-6,76,24,6,C(34,65,84));
    label(28,y+1,labels[i],white(),1,9);
    label(111,y+1,actions[i],muted(),1,20);
  }
  label(25,197,"APPS DISABLED IN RUN004",mint(),1,29);
  footer();
}
void eh_render004(uint16_t *fb,const EhUI *state){
  if(!fb||!state)return;
  screen=fb;
  if(state->page==EH_PAGE_HOME){
    home(state);
  }else if(state->page==EH_PAGE_HELP){
    background();brand();help();
  }else{
    background();brand();detail(state,state->page==EH_PAGE_ABOUT);
  }
}
