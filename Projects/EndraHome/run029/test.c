#include "registry.h"
#include "ui.h"
#include "desktop.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 EhRegistry r;eh7_registry_init(&r);
 assert(eh7_registry_parse(&r,"0=GBA|/documents/GBA/App.tns")==1);
 EhUniversalUI s;eh7_ui_init(&s,&r);
 eh7_ui_handle(&s,EH_KEY_ENTER);
 assert(s.request==1); /* Home Enter still instant launch */
 EhUniversalUI m;eh7_ui_init(&m,&r);
 eh7_ui_handle(&m,EH_KEY_MENU);
 assert(m.page.page==EH_PAGE_DETAIL&&m.manager_item==EH_MANAGER_ADD);
 eh7_ui_handle(&m,EH_KEY_ENTER);assert(m.assign_slot==2&&m.adding_app);
 m.assign_slot=0;
 eh7_ui_handle(&m,EH_KEY_DOWN);
 eh7_ui_handle(&m,EH_KEY_ENTER);assert(m.assign_slot==1&&!m.adding_app);
 EhUniversalUI n;eh7_ui_init(&n,&r);
 eh7_ui_handle(&n,EH_KEY_MENU);
 n.manager_item=EH_MANAGER_RENAME;
 eh7_ui_handle(&n,EH_KEY_ENTER);
 assert(n.renaming);
 assert(!eh16_ui_type(NULL,'A'));
 assert(eh16_ui_type(&n,'E'));
 assert(eh16_ui_type(&n,'n'));
 assert(eh16_ui_type(&n,'d'));
 assert(eh16_ui_type(&n,'r'));
 assert(eh16_ui_type(&n,'a'));
 assert(!strcmp(n.rename_input,"Endra"));
 assert(eh16_ui_type(&n,8)); /* DEL/backspace */
 assert(!strcmp(n.rename_input,"Endr"));
 assert(eh16_ui_type(&n,'a'));
 assert(!eh16_ui_type(&n,'|')); /* Config delimiter prohibited */
 assert(!strcmp(n.rename_input,"Endra"));
 assert(eh7_ui_handle(&n,EH_KEY_ENTER)); /* Enter saves */
 assert(n.edit_action==EH15_RENAME&&!n.renaming);
 assert(!eh16_ui_type(&n,'Z'));
 EhUniversalUI empty;eh7_ui_init(&empty,&r);
 eh7_ui_handle(&empty,EH_KEY_MENU);
 empty.manager_item=EH_MANAGER_RENAME;
 eh7_ui_handle(&empty,EH_KEY_ENTER);
 assert(empty.renaming);
 eh7_ui_handle(&empty,EH_KEY_ENTER);
 assert(empty.renaming&&!empty.edit_action);
 assert(!strcmp(empty.notice.text,"NAME REQUIRED - TYPE QWERTY"));
 eh7_ui_handle(&empty,EH_KEY_ESC);
 assert(!empty.renaming);
 EhUniversalUI max;eh7_ui_init(&max,&r);
 eh7_ui_handle(&max,EH_KEY_MENU);
 max.manager_item=EH_MANAGER_RENAME;
 eh7_ui_handle(&max,EH_KEY_ENTER);
 for(int i=0;i<EH7_LABEL_CAP-1;i++)assert(eh16_ui_type(&max,'a'));
 assert(!strcmp(max.rename_input,"aaaaaaaaaaaaaaa"));
 assert(eh16_ui_type(&max,'z'));
 assert(strlen(max.rename_input)==EH7_LABEL_CAP-1);
 assert(eh16_ui_type(&max,127));
 assert(strlen(max.rename_input)==EH7_LABEL_CAP-2);
 EhUniversalUI rm;eh7_ui_init(&rm,&r);
 eh7_ui_handle(&rm,EH_KEY_MENU);
 rm.manager_item=EH_MANAGER_REMOVE;
 eh7_ui_handle(&rm,EH_KEY_ENTER);
 assert(rm.confirm_remove);
 eh7_ui_handle(&rm,EH_KEY_ESC);
 assert(!rm.confirm_remove&&!rm.edit_action);
 eh7_ui_handle(&rm,EH_KEY_ENTER);
 eh7_ui_handle(&rm,EH_KEY_ENTER);
 assert(rm.edit_action==EH15_REMOVE);
 EhUniversalUI info;eh7_ui_init(&info,&r);
 eh7_ui_handle(&info,EH_KEY_MENU);
 info.manager_item=EH_MANAGER_INFO;
 eh7_ui_handle(&info,EH_KEY_ENTER);
 assert(info.page.page==EH_PAGE_HELP&&!info.subview);
 eh7_ui_handle(&info,EH_KEY_ESC);
 eh7_ui_handle(&info,EH_KEY_DOWN);
 eh7_ui_handle(&info,EH_KEY_ENTER);
 assert(info.page.page==EH_PAGE_HELP&&info.subview);
 /* Files never launches its obsolete external override. */
 EhUniversalUI fm;eh7_ui_init(&fm,&r);
 eh7_ui_handle(&fm,EH_KEY_RIGHT);eh7_ui_handle(&fm,EH_KEY_RIGHT);assert(fm.page.selected==EH_FILES_SLOT);
 eh7_ui_handle(&fm,EH_KEY_ENTER);assert(fm.open_files && !fm.request);
 fm.open_files=0;eh7_ui_handle(&fm,EH_KEY_MENU);
 fm.manager_item=EH_MANAGER_FILES;
 eh7_ui_handle(&fm,EH_KEY_ENTER);assert(fm.open_files && !fm.request);
 EhRegistry bare;eh7_registry_init(&bare);EhUniversalUI only;eh7_ui_init(&only,&bare);
 assert(only.page.selected==EH_FILES_SLOT);
 eh7_ui_handle(&only,EH_KEY_ENTER);assert(only.open_files && !only.assign_slot && !only.request);
 only.open_files=0;eh7_ui_handle(&only,EH_KEY_MENU);
 assert(only.manager_item==EH_MANAGER_ADD);
 eh7_ui_handle(&only,EH_KEY_ENTER);
 assert(only.assign_slot==1&&only.adding_app&&only.page.selected==EH_FILES_SLOT);
 only.assign_slot=0;
 only.manager_item=EH_MANAGER_REASSIGN;
 eh7_ui_handle(&only,EH_KEY_ENTER);
 assert(!only.assign_slot&&only.notice.text[0]);
 for(int i=0;i<EH7_SLOT_COUNT;i++)strcpy(bare.slots[i].path,"/documents/A.tns");
 only.manager_item=EH_MANAGER_ADD;eh7_ui_handle(&only,EH_KEY_ENTER);
 assert(!only.assign_slot&&strstr(only.notice.text,"FULL"));
 /* Every possible subset of shortcuts. Render and input use identical geometry. */
 for(unsigned mask=0;mask<128;mask++){
  EhRegistry grid;eh7_registry_init(&grid);
  for(int i=0;i<7;i++)if(mask&(1u<<i))strcpy(grid.slots[i].path,"/documents/A.tns");
  EhTile tiles[7];int count=eh_desktop_layout(&grid,tiles),files=0;
  for(int i=0;i<count;i++){
   files+=tiles[i].slot==EH_FILES_SLOT;
   int right=eh_desktop_select(&grid,tiles[i].slot,EH_KEY_RIGHT);
   int left=eh_desktop_select(&grid,tiles[i].slot,EH_KEY_LEFT);
   assert(right==(i+1<count && tiles[i+1].row==tiles[i].row?tiles[i+1].slot:tiles[i].slot));
   assert(left==(i>0 && tiles[i-1].row==tiles[i].row?tiles[i-1].slot:tiles[i].slot));
   for(int down=0;down<2;down++){
    int target=eh_desktop_select(&grid,tiles[i].slot,down?EH_KEY_DOWN:EH_KEY_UP);
    int closest=321,expected=tiles[i].slot;
    for(int j=0;j<count;j++)if(tiles[j].row==tiles[i].row+(down?1:-1)){
     int d=tiles[j].center-tiles[i].center;if(d<0)d=-d;
     if(d<closest){expected=tiles[j].slot;closest=d;}
    }
    assert(target==expected);
   }
  }
  assert(files==1);
  assert(count>=1);
 }
 /* Live preview stays separate from saved appearance until a successful save. */
 eh7_ui_handle(&info,EH_KEY_DOWN);eh7_ui_handle(&info,EH_KEY_RIGHT);
 assert(info.preview.folder==1 && info.preview.theme==0 && info.preview.font==0);
 eh7_ui_handle(&info,EH_KEY_DOWN);eh7_ui_handle(&info,EH_KEY_LEFT);
 assert(info.preview.font==EH_COLOR_COUNT-1 && !r.appearance.font);
 eh7_ui_handle(&info,EH_KEY_ESC);assert(!info.preview.font && !info.preview.folder);
 eh7_ui_handle(&info,EH_KEY_ENTER);eh7_ui_handle(&info,EH_KEY_RIGHT);
 eh7_ui_handle(&info,EH_KEY_ENTER);assert(info.edit_action==EH15_THEME && !r.appearance.theme);
 info.edit_action=0;
 eh7_ui_handle(&info,EH_KEY_ESC);
 r.appearance=(EhAppearance){EH_GREEN,3,2,1,4};
 eh7_ui_handle(&info,EH_KEY_ENTER);
 for(unsigned n=1;n<=EH_THEME_COUNT;n++){
  eh7_ui_handle(&info,EH_KEY_LEFT);
  assert(info.preview.theme==(EH_GREEN+EH_THEME_COUNT-n)%EH_THEME_COUNT);
  assert(r.appearance.theme==EH_GREEN && !info.edit_action);
 }
 eh7_ui_handle(&info,EH_KEY_RIGHT);assert(info.preview.theme==EH_YELLOW);
 eh7_ui_handle(&info,EH_KEY_LEFT);assert(info.preview.theme==EH_GREEN);
 for(int i=0;i<EH_LOOK_WALLPAPER;i++)eh7_ui_handle(&info,EH_KEY_DOWN);
 assert(info.setting_row==EH_LOOK_WALLPAPER);
 for(unsigned n=1;n<=EH_WALLPAPER_COUNT;n++){
  eh7_ui_handle(&info,EH_KEY_RIGHT);
  assert(info.preview.wallpaper==(n+1)%EH_WALLPAPER_COUNT);
  assert(!info.edit_action && r.appearance.wallpaper==1 && info.preview.icon==4);
 }
 eh7_ui_handle(&info,EH_KEY_DOWN);
 assert(info.setting_row==EH_LOOK_ICON);
 for(unsigned n=1;n<=EH_ICON_COUNT;n++){
  eh7_ui_handle(&info,EH_KEY_LEFT);
  assert(info.preview.icon==(4+EH_ICON_COUNT-n)%EH_ICON_COUNT);
  assert(!info.edit_action && r.appearance.icon==4 && info.preview.wallpaper==1);
 }
 eh7_ui_handle(&info,EH_KEY_RIGHT);
 eh7_ui_handle(&info,EH_KEY_ESC);
 assert(info.preview.icon==4 && info.preview.wallpaper==1);
 eh7_ui_handle(&info,EH_KEY_ENTER);
 for(int i=0;i<EH_LOOK_DEFAULTS;i++)eh7_ui_handle(&info,EH_KEY_DOWN);
 eh7_ui_handle(&info,EH_KEY_ENTER);
 assert(info.setting_row==EH_LOOK_THEME && !info.edit_action);
 assert(!info.preview.theme && !info.preview.folder && !info.preview.font &&
        !info.preview.wallpaper && !info.preview.icon);
 assert(r.appearance.theme==EH_GREEN && r.appearance.font==2 &&
        r.appearance.wallpaper==1 && r.appearance.icon==4);
 eh7_ui_handle(&info,EH_KEY_ENTER);
 assert(info.edit_action==EH15_THEME);
 EhUniversalUI shortcut;eh7_ui_init(&shortcut,&r);
 eh7_ui_handle(&shortcut,EH_KEY_MENU);
 for(int i=0;i<EH_MANAGER_TI;i++)eh7_ui_handle(&shortcut,EH_KEY_DOWN);
 assert(shortcut.manager_item==EH_MANAGER_TI);
 eh7_ui_handle(&shortcut,EH_KEY_ENTER);assert(shortcut.subview==2);
 eh7_ui_handle(&shortcut,EH_KEY_ENTER);assert(shortcut.edit_action==EH15_NATIVE_CHECK);
 shortcut.edit_action=0;eh7_ui_handle(&shortcut,EH_KEY_ESC);
 assert(shortcut.page.page==EH_PAGE_DETAIL);
 EhToast toast={0};
 eh_toast_set(&toast,"Opened");uint32_t start=UINT32_MAX-100;
 assert(!eh_toast_tick(&toast,start));assert(!eh_toast_tick(&toast,start+3u*EH_TICKS_PER_SECOND-1));
 assert(eh_toast_tick(&toast,start+3u*EH_TICKS_PER_SECOND) && !toast.text[0]);
 eh_toast_set(&toast,"First");eh_toast_tick(&toast,50);
 eh_toast_set(&toast,"First");eh_toast_tick(&toast,60);
 assert(!eh_toast_tick(&toast,59+3u*EH_TICKS_PER_SECOND));
 puts("PASS all 128 desktop layouts, navigation, Files, settings, QWERTY and toast expiry");
 return 0;
}
