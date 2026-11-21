#!/usr/bin/env python3
"""Run012: match the shared loading/status/error renderer to Run011 NightMode.

Apply the verified cumulative patch chain first. Only six loading palette
constants and the two version labels may differ from its output.
"""
from pathlib import Path
import hashlib
import re
import runpy

runpy.run_path(str(Path(__file__).with_name("run011_nightmode.py").resolve()))
root = Path("upstream-gpsp")
before = {p.relative_to(root).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
          for p in root.rglob("*") if p.is_file()}
loader = root / "nspire_loading.c"
original = loader.read_text()
source = original
# The existing MENU_COLOR packs BGR555 plus the green low bit. Match Run011's
# even-green neutral grays; keep its native conversion and framebuffer path.
palette = {
    "COLOR_BACK":  ((2, 8, 10), (0, 0, 0)),
    "COLOR_TITLE": ((31, 63, 31), (28, 56, 28)),
    "COLOR_TEXT":  ((16, 40, 24), (23, 46, 23)),
    "COLOR_BAR":   ((13, 40, 18), (20, 40, 20)),
    "COLOR_EMPTY": ((4, 14, 16), (7, 14, 7)),
    "COLOR_ERROR": ((31, 44, 0), (28, 56, 28)),
}
for name, (old, new) in palette.items():
    pattern = r"(?m)^(#define " + name + r"\s+)MENU_COLOR" + re.escape(str(old)) + r"$"
    source, count = re.subn(pattern, lambda m: m.group(1) + "MENU_COLOR" + str(new), source)
    assert count == 1, f"Unexpected loading palette: {name}"
    r, g, b = new
    assert r == b and g == 2*r and 0 <= r <= 31
# Prove every non-palette byte is preserved, including progress, waits and errors.
def without_palette(text):
    return re.sub(r"(?m)^#define COLOR_\w+\s+MENU_COLOR\([^\n]+\)$", "", text)
assert without_palette(source) == without_palette(original)
loader.write_text(source)

gui = root / "gui.c"
g = gui.read_text()
assert g.count('"EndraGBA Run011"') == 2
gui.write_text(g.replace('"EndraGBA Run011"', '"EndraGBA Run012"'))
makefile = root / "Makefile"
m = makefile.read_text()
old = '--name "EndraGBA Run011 NightMode" --version 11'
assert m.count(old) == 1
makefile.write_text(m.replace(old, '--name "EndraGBA Run012 LoadingNightMode" --version 12', 1))

after = {p.relative_to(root).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
         for p in root.rglob("*") if p.is_file()}
assert before.keys() == after.keys()
changed = {name for name in before if before[name] != after[name]}
assert changed == {"nspire_loading.c", "gui.c", "Makefile"}, changed
assert gui.read_text().replace('"EndraGBA Run012"', '"EndraGBA Run011"') == g
assert "refresh_osscr();" not in (root / "nspire/nspire.c").read_text()
print("Run012: six loading colors and version labels only; all other Run011 bytes retained")
