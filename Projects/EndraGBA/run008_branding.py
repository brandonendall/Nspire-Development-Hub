#!/usr/bin/env python3
"""Layer EndraGBA display branding onto the preserved Run007 behavior.

Run from the repository root with the pinned source in upstream-gpsp/.
Save paths and Ndless association behavior remain Run007's.
"""
from pathlib import Path
import runpy

runpy.run_path(str(Path(__file__).resolve().parents[1] / "UltraGBA/run007_patch.py"))
source = Path("upstream-gpsp")
gui = source / "gui.c"
text = gui.read_text()
labels = {
    "gpsp-libretro for nSpire (Alpha v0.4.1)": "EndraGBA Run008",
    "gpsp-libretro for nspire (Alpha v0.4.1)": "EndraGBA Run008",
    "Select to exit gpSP and return to the menu.": "Exit EndraGBA and return to Documents.",
    "Exit gpSP": "Exit EndraGBA",
    "The backlight while gpSP runs (CX II); the": "Backlight while EndraGBA runs (CX II); the",
    "calculator's own comes back when gpSP closes.": "calculator's setting returns on exit.",
    "This option requires gpSP to be restarted before it will take effect.": "Restart EndraGBA for this to take effect.",
    "This option requires gpSP restart to take effect.": "Restart EndraGBA for this to take effect.",
}
for old, new in labels.items():
    assert old in text, f"Expected menu label missing: {old}"
    text = text.replace(old, new)
gui.write_text(text)

makefile = source / "Makefile"
text = makefile.read_text()
assert "ZEHNFLAGS := --version 12" in text
text = text.replace("ZEHNFLAGS := --version 12", 'ZEHNFLAGS := --name "EndraGBA Run008" --version 8')
makefile.write_text(text)

# Preserve compatibility; this does not migrate data or repair associations.
paths = (source / "nspire_paths.c").read_text()
host = (source / "nspire_host_main.c").read_text()
assert "UltraGBA_Data" in paths and "UltraGBA_Tests" in paths
assert "cfg_register_fileext" not in host and "register_rom_types(" not in host
assert "--240x320-support=true" in text
print("EndraGBA Run008: menus and metadata renamed; existing data paths retained")
