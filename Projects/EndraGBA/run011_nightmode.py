#!/usr/bin/env python3
"""Apply the approved monochrome night palette to Run010.

Only native GUI drawing colors and version metadata change. Do not alter
game rendering, brightness controls, ROM paths, save data or hardware code.
"""
from pathlib import Path
import re
import runpy
import hashlib

runpy.run_path(str(Path(__file__).with_name("run010_browser_ui.py").resolve()))
root = Path("upstream-gpsp")
gui = root / "gui.c"
s = gui.read_text()
before = s
# In the existing native UI color encoding, red/blue are five-bit and
# green six-bit. Even green values avoid introducing the packed low bit.
# Preserve the existing conversion/rotation implementation.
palette = {
    "COLOR_BG": (0, 0, 0),
    "COLOR_ROM_INFO": (23, 46, 23),
    "COLOR_ACTIVE_ITEM": (27, 54, 27),
    "COLOR_INACTIVE_ITEM": (23, 46, 23),
    "COLOR_FRAMESKIP_BAR": (15, 30, 15),
    "COLOR_HELP_TEXT": (20, 40, 20),
    "COLOR_MOVING": (28, 56, 28),
    "ENDRA_NAVY": (2, 4, 2),
    "ENDRA_FOLDER": (0, 0, 0),
    "ENDRA_GAME": (0, 0, 0),
    "ENDRA_DIVIDER": (7, 14, 7),
    "ENDRA_ACCENT": (10, 20, 10),
    "ENDRA_WHITE": (28, 56, 28),
    "ENDRA_MUTED": (23, 46, 23),
}
# Override shared color macros only for NSPIRE_LIBRETRO; other frontends
# still compile with their original colors.
anchor = "#define COLOR_MOVING        color16(31, 50, 4)\n"
assert s.count(anchor) == 1
overrides = "#if defined(NSPIRE_LIBRETRO)\n"
for name, rgb in palette.items():
    if name.startswith("COLOR_"):
        overrides += f"#undef {name}\n#define {name} color16{rgb}\n"
overrides += "#endif\n"
s = s.replace(anchor, anchor + overrides, 1)
for name, rgb in palette.items():
    if name.startswith("ENDRA_"):
        s, count = re.subn(r"(?m)^(#define " + name + r"\s+)color16\([^\n]+\)$",
                           lambda m: m.group(1) + f"color16{rgb}", s)
        assert count == 1, name

# A restrained mid-gray selected row replaces the old glaring inverse-video
# highlight. Both library and emulator menus share the same selected-row style.
old = """        print_string_pad(line_buffer, COLOR_BG, COLOR_ACTIVE_ITEM, 10,
         (line * 10) + 40, 41);"""
new = """#if defined(NSPIRE_LIBRETRO)
        print_string_pad(line_buffer, ENDRA_WHITE, ENDRA_ACCENT, 10,
         (line * 10) + 40, 41);
#else
""" + old + """
#endif"""
assert s.count(old) == 1
s = s.replace(old, new, 1)

# Dark charcoal column headers and a thin footer separator, matching the mockup.
old = """  fill_rect(120, 24, 2, 192, ENDRA_DIVIDER);
  fill_rect(0, 40, 320, 2, ENDRA_DIVIDER);"""
new = """  fill_rect(0, 24, 120, 16, ENDRA_NAVY);
  fill_rect(122, 24, 198, 16, ENDRA_NAVY);
  fill_rect(120, 24, 2, 192, ENDRA_DIVIDER);
  fill_rect(0, 40, 320, 2, ENDRA_DIVIDER);"""
assert s.count(old) == 1
s = s.replace(old, new, 1)
for old in ("ENDRA_FOLDER, 7, 28);", "ENDRA_GAME, 130, 28);"):
    assert s.count(old) == 1
    s = s.replace(old, old.replace(old.split(",")[0], "ENDRA_NAVY"), 1)
old = "  fill_rect(0, 216, 320, 24, ENDRA_NAVY);"
assert s.count(old) == 1
s = s.replace(old, old + "\n  fill_rect(0, 216, 320, 1, ENDRA_DIVIDER);", 1)
assert s.count('"EndraGBA Run010"') >= 1
s = s.replace('"EndraGBA Run010"', '"EndraGBA Run011"')
gui.write_text(s)

mk = root / "Makefile"
m = mk.read_text()
old = '--name "EndraGBA Run010 Browser" --version 10'
assert m.count(old) == 1
mk.write_text(m.replace(old, '--name "EndraGBA Run011 NightMode" --version 11', 1))

# Verify retained browser and exit contracts, plus near-neutral palette.
for name, (r,g,b) in palette.items():
    assert r == b and g == 2*r and 0 <= r <= 31
    assert f"color16({r}, {g}, {b})" in s
for token in ("endra_browser_row(", "Folders are on the left; games are on the right.",
              "game_list_save_order", "nspire_skip_state_load = key_down(KEY_SHIFT);"):
    assert token in s
p = (root / "nspire/nspire.c").read_text()
assert "refresh_osscr();" not in p and "timer_restore();" in p
assert "enableAlignmentExceptions();" in p
assert "UltraGBA_Data" in (root / "nspire_paths.c").read_text()
print("Run011 NightMode: monochrome UI palette and browser chrome; Run010 navigation and Run009 FastExit retained")
