#!/usr/bin/env python3
"""Run010: give EndraGBA's 320x240 ROM picker a usable left-hand folder pane.

Layers directly on confirmed-fast Run009: no file, save, RTC, GBZ, timer, LCD,
hotkey or Ndless association code is changed. Native browser only; preserve
unrelated backends' original screen rendering and key behavior.
"""
from pathlib import Path
import runpy

runpy.run_path(str(Path(__file__).with_name("run009_fast_exit.py").resolve()))
root = Path("upstream-gpsp")
gui = root / "gui.c"
source = gui.read_text()

def one(old: str, new: str, what: str) -> None:
    global source
    assert source.count(old) == 1, f"{what}: expected exactly once, got {source.count(old)}"
    source = source.replace(old, new, 1)

one(
    "#define COLOR_MOVING        color16(31, 50, 4)\n",
    """#define COLOR_MOVING        color16(31, 50, 4)

/* EndraGBA ROM browser layout: render into the native 320x240 UI surface.
 * All pixels and labels fit within the existing orientation/rotation path.
 * Other gpSP frontends keep their original list geometry. */
#if defined(NSPIRE_LIBRETRO)
#undef FILE_LIST_ROWS
#define FILE_LIST_ROWS 17
#define ENDRA_DIR_X      4
#define ENDRA_DIR_W    116
#define ENDRA_FILE_X   125
#define ENDRA_FILE_W   190
#define ENDRA_ROW_Y(n) (44 + (n) * FONT_HEIGHT)
#define ENDRA_NAVY     color16(1, 6, 12)
#define ENDRA_FOLDER   color16(3, 11, 16)
#define ENDRA_GAME     color16(2, 9, 14)
#define ENDRA_DIVIDER  color16(8, 26, 27)
#define ENDRA_ACCENT   color16(5, 31, 28)
#define ENDRA_WHITE    color16(31, 63, 31)
#define ENDRA_MUTED    color16(21, 48, 27)
#endif
""",
    "UI layout constants",
)

helpers = """#if defined(NSPIRE_LIBRETRO)
/* Keep all printing inside its pane: gpSP's default print_string clips to
 * the full LCD width, not to a 2-column file-browser cell. */
static void endra_browser_abbrev(const char *src, char *out,
                                 size_t cap, size_t max_chars)
{
  size_t n;
  if (!cap) return;
  if (!src) src = "";
  if (max_chars >= cap) max_chars = cap - 1;
  n = strlen(src);
  if (n <= max_chars)
  {
    memcpy(out, src, n + 1);
    return;
  }
  if (max_chars <= 3)
  {
    memcpy(out, src, max_chars);
    out[max_chars] = 0;
    return;
  }
  memcpy(out, src, max_chars - 3);
  memcpy(out + max_chars - 3, "...", 3);
  out[max_chars] = 0;
}

/* For a path, keep the relevant trailing directories rather than its root. */
static void endra_browser_path(const char *src, char *out,
                               size_t cap, size_t max_chars)
{
  size_t n;
  if (!cap) return;
  if (!src) src = "";
  if (max_chars >= cap) max_chars = cap - 1;
  n = strlen(src);
  if (n <= max_chars)
  {
    memcpy(out, src, n + 1);
    return;
  }
  if (max_chars < 4)
  {
    endra_browser_abbrev(src, out, cap, max_chars);
    return;
  }
  memcpy(out, "...", 3);
  memcpy(out + 3, src + n - (max_chars - 3), max_chars - 3);
  out[max_chars] = 0;
}

static void endra_browser_chrome(const char *path, u32 dirs, u32 files, u32 col)
{
  char line[72];
  fill_rect(0, 0, 320, 24, ENDRA_NAVY);
  print_string("ENDRAGBA  |  GAME LIBRARY", ENDRA_WHITE, ENDRA_NAVY, 7, 2);
  endra_browser_path(path, line, sizeof(line), 51);
  print_string(line, ENDRA_MUTED, ENDRA_NAVY, 7, 13);

  fill_rect(0, 24, 121, 192, ENDRA_FOLDER);
  fill_rect(122, 24, 198, 192, ENDRA_GAME);
  fill_rect(120, 24, 2, 192, ENDRA_DIVIDER);
  fill_rect(0, 40, 320, 2, ENDRA_DIVIDER);

  snprintf(line, sizeof(line), "FOLDERS (%u)", (unsigned)dirs);
  print_string(line, col == 1 ? ENDRA_WHITE : ENDRA_MUTED,
               ENDRA_FOLDER, 7, 28);
  snprintf(line, sizeof(line), "GAMES (%u)", (unsigned)files);
  print_string(line, col == 0 ? ENDRA_WHITE : ENDRA_MUTED,
               ENDRA_GAME, 130, 28);

  fill_rect(0, 216, 320, 24, ENDRA_NAVY);
}

/* status: 0 normal, 1 selected, 2 being moved */
static void endra_browser_row(const char *text, u32 x, u32 y, u32 w,
                              size_t max_chars, int status)
{
  char clipped[64];
  u16 bg = status == 1 ? ENDRA_ACCENT :
           (x == ENDRA_DIR_X ? ENDRA_FOLDER : ENDRA_GAME);
  u16 fg = status == 1 ? ENDRA_WHITE :
           (status == 2 ? COLOR_MOVING : ENDRA_MUTED);
  endra_browser_abbrev(text, clipped, sizeof(clipped), max_chars);
  fill_rect(x, y, w, FONT_HEIGHT, bg);
  print_string(clipped, fg, bg, x + 3, y);
}

static void endra_browser_footer(int moving, const char *status)
{
  char line[64];
  if (moving >= 0)
  {
    print_string("MOVE: Up/Down to reorder  Enter/Tab: confirm",
                 ENDRA_WHITE, ENDRA_NAVY, 5, 217);
    print_string("Esc/Menu: undo move", ENDRA_MUTED, ENDRA_NAVY, 5, 228);
  }
  else if (status && status[0])
  {
    endra_browser_abbrev(status, line, sizeof(line), 51);
    print_string(line, ENDRA_WHITE, ENDRA_NAVY, 5, 217);
    print_string("Enter: open/play   Left/Right: switch panes",
                 ENDRA_MUTED, ENDRA_NAVY, 5, 228);
  }
  else
  {
    print_string("Enter: open/play   Left/Right: switch panes",
                 ENDRA_WHITE, ENDRA_NAVY, 5, 217);
    print_string("1-0: play  Shift: fresh  Tab: move  Menu: back",
                 ENDRA_MUTED, ENDRA_NAVY, 5, 228);
  }
}
#endif

"""
one(
    "s32 load_file(u8 **wildcards, u8 *result)\n{",
    helpers + "s32 load_file(u8 **wildcards, u8 *result)\n{",
    "browser helper insertion",
)
one(
    "      print_string(current_dir_short, COLOR_ACTIVE_ITEM, COLOR_BG, 0, 0);",
    """#if defined(NSPIRE_LIBRETRO)
      endra_browser_chrome((const char *)current_dir_name, num_dirs, num_files,
                           current_column);
#else
      print_string(current_dir_short, COLOR_ACTIVE_ITEM, COLOR_BG, 0, 0);
#endif""",
    "header",
)
one(
    """#elif defined(NSPIRE_LIBRETRO)
       if(moving >= 0)
       {
         print_string("Moving it: up/down, then enter to put it there,",
          COLOR_HELP_TEXT, COLOR_BG, 20, 220);
         print_string("or esc to put it back.", COLOR_HELP_TEXT, COLOR_BG, 20, 230);
       }
       else
       {
         print_string(list_status[0] ? list_status :
          "enter or 1-9, 0: start  shift: without its state",
          COLOR_HELP_TEXT, COLOR_BG, 20, 220);
         print_string("tab: move a game  shift+tab: A to Z  menu: back",
          COLOR_HELP_TEXT, COLOR_BG, 20, 230);
       }
""",
    """#elif defined(NSPIRE_LIBRETRO)
       endra_browser_footer(moving, list_status);
""",
    "footer",
)
one(
    """#if defined(NSPIRE_LIBRETRO)
           /* The first ten start with their number key, shown before them. */
           char row[100];
           u16 color = COLOR_INACTIVE_ITEM;

           if((s32)current_file_number == moving)
             color = COLOR_MOVING;
           else if((current_file_number == current_file_selection) &&
            (current_column == 0))
             color = COLOR_ACTIVE_ITEM;
           if(current_file_number < 10)
             snprintf(row, sizeof(row), "%u %s", (current_file_number + 1) % 10,
              (char *)file_list[current_file_number]);
           else
             snprintf(row, sizeof(row), "  %s", (char *)file_list[current_file_number]);
           print_string(row, color, COLOR_BG, FILE_LIST_POSITION, ((i + 1) * 10));
#else""",
    """#if defined(NSPIRE_LIBRETRO)
           char row[100];
           int selected = current_file_number == current_file_selection &&
                          current_column == 0;
           int moving_row = (s32)current_file_number == moving;
           if(current_file_number < 10)
             snprintf(row, sizeof(row), "%u %s", (current_file_number + 1) % 10,
                      (char *)file_list[current_file_number]);
           else
             snprintf(row, sizeof(row), "  %s",
                      (char *)file_list[current_file_number]);
           endra_browser_row(row, ENDRA_FILE_X, ENDRA_ROW_Y(i),
                             ENDRA_FILE_W, 30, moving_row ? 2 : selected);
#else""",
    "file entries",
)
one(
    """         if(current_dir_number < num_dirs)
         {
           if((current_dir_number == current_dir_selection) &&
            (current_column == 1))
           {
             print_string(dir_list[current_dir_number], COLOR_ACTIVE_ITEM,
              COLOR_BG, DIR_LIST_POSITION, ((i + 1) * 10));
           }
           else
           {
             print_string(dir_list[current_dir_number], COLOR_INACTIVE_ITEM,
              COLOR_BG, DIR_LIST_POSITION, ((i + 1) * 10));
           }
         }""",
    """         if(current_dir_number < num_dirs)
         {
#if defined(NSPIRE_LIBRETRO)
           char row[100];
           snprintf(row, sizeof(row), "+ %s",
                    (char *)dir_list[current_dir_number]);
           endra_browser_row(row, ENDRA_DIR_X, ENDRA_ROW_Y(i),
                             ENDRA_DIR_W, 18,
                             current_dir_number == current_dir_selection &&
                             current_column == 1);
#else
           if((current_dir_number == current_dir_selection) &&
            (current_column == 1))
           {
             print_string(dir_list[current_dir_number], COLOR_ACTIVE_ITEM,
              COLOR_BG, DIR_LIST_POSITION, ((i + 1) * 10));
           }
           else
           {
             print_string(dir_list[current_dir_number], COLOR_INACTIVE_ITEM,
              COLOR_BG, DIR_LIST_POSITION, ((i + 1) * 10));
           }
#endif
         }""",
    "folder entries",
)
one(
    """         case CURSOR_RIGHT:
           if(current_column == 0)
           {
             if(num_dirs != 0)
               current_column = 1;
           }
           break;

         case CURSOR_LEFT:
           if(current_column == 1)
           {
             if(num_files != 0)
               current_column = 0;
           }
           break;""",
    """         case CURSOR_RIGHT:
#if defined(NSPIRE_LIBRETRO)
           /* Folders are on the left; games are on the right. */
           if(current_column == 1 && num_files != 0)
             current_column = 0;
#else
           if(current_column == 0)
           {
             if(num_dirs != 0)
               current_column = 1;
           }
#endif
           break;

         case CURSOR_LEFT:
#if defined(NSPIRE_LIBRETRO)
           if(current_column == 0 && num_dirs != 0)
             current_column = 1;
#else
           if(current_column == 1)
           {
             if(num_files != 0)
               current_column = 0;
           }
#endif
           break;""",
    "correct physical pane navigation",
)
# Avoid a tiny, unpainted strip below the last row. Leave true game controls alone.
assert "nspire_gui_digits_move = 0;" in source
assert "game_list_save_order" in source
assert "nspire_skip_state_load = key_down(KEY_SHIFT);" in source
assert "if(current_column == 1)" in source
assert source.count("endra_browser_chrome(") == 2
assert source.count("endra_browser_row(") == 3
# Update only user-facing version line (not the external attribution).
assert source.count('"EndraGBA Run008"') == 1, "Expected branding from Run008"
source = source.replace('"EndraGBA Run008"', '"EndraGBA Run010"', 1)
gui.write_text(source)

mk = root / "Makefile"
text = mk.read_text()
previous = '--name "EndraGBA Run009 FastExit" --version 9'
assert text.count(previous) == 1
mk.write_text(text.replace(previous,
            '--name "EndraGBA Run010 Browser" --version 10', 1))
# FastExit still untouched in platform. Save/RTC files must stay compatible.
platform = (root / "nspire" / "nspire.c").read_text()
assert "refresh_osscr();" not in platform
assert "timer_restore();" in platform
assert "enableAlignmentExceptions();" in platform
paths = (root / "nspire_paths.c").read_text()
assert "UltraGBA_Data" in paths
print("EndraGBA Run010: left-hand directories, right-hand games, safe truncation, readable UI")
