"""Apply Run012, then wire one-shot ARv3 codes into its existing menu."""
import runpy
from pathlib import Path
here = Path(__file__).resolve().parent
runpy.run_path(str(here / "run012_loading_nightmode.py"))
root = Path("upstream-gpsp")
def edit(file, old, new, count=1):
    path = root / file
    text = path.read_text()
    assert text.count(old) == count, old
    path.write_text(text.replace(old, new))
(root / "endra_ar3.inc").write_bytes((here / "run013_cheats.c").read_bytes())
edit("nspire_host_main.c", '#include <unistd.h>', '#include <unistd.h>\n#include "endra_ar3.inc"')
edit("nspire_host_main.c", '  states_open_game(gamepak_filename);', '  load_ar3(rom_path, gamepak_code);\n  states_open_game(gamepak_filename);')
edit("nspire_host_main.c", '  process_cheats();', '  process_cheats();\n  apply_ar3();')
edit("gui.c", '"EndraGBA Run012"', '"EndraGBA Run013"', 2)
edit("gui.c", '"Activate/deactivate this cheat code."', '"Left/right toggles. Resume to apply."')
edit("gui.c", '"Cheats and Miscellaneous options"', '"Cheats and save options"')
# Refresh labels after an explicit reload without restarting the game.
gui = root / "gui.c"
s = gui.read_text()
pos = s.index("  for(i = 0; i < 10; i++)", s.index("  choose_menu(&main_menu);", s.index('print_string_ext("No game loaded yet."')))
end = s.index("  //current_menu->init_function();",pos)
labels = s[pos:end]
s = s[:pos] + "  refresh_ar3_labels();\n" + s[end:]
needle = "  void submenu_cheats_misc()"
s = s.replace(needle, "  void refresh_ar3_labels() {\n" + labels.replace("for(i =", "for(int i =") + "  }\n  void reload_ar3_menu() { extern void endra_reload_ar3(void); endra_reload_ar3(); refresh_ar3_labels(); }\n" + needle, 1)
anchor = '    submenu_option(NULL, "Back", "Return to the main menu.", 15)\n  };\n#else\n  menu_option_type cheats_misc_options'
assert anchor in s
s = s.replace(anchor, '    action_option(reload_ar3_menu, NULL, "Reload cheat file", "Reload this game.s .cht.tns; resets toggles.", 15),\n' + anchor.replace('", 15)', '", 16)'), 1)
gui.write_text(s)
edit("nspire_host_main.c", '#include "endra_ar3.inc"', '#include "endra_ar3.inc"\nvoid endra_reload_ar3(void) { if (*current_rom) load_ar3(current_rom, gamepak_code); }')
edit("Makefile", '--name "EndraGBA Run012 LoadingNightMode" --version 12', '--name "EndraGBA Run013 Cheats" --version 13')
