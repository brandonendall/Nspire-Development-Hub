"""Apply the current file-only cheat loader and mark the Run014 executable."""
import runpy
from pathlib import Path

runpy.run_path(str(Path(__file__).with_name("run013_cheats.py").resolve()))
root = Path("upstream-gpsp")
for filename, old, new, expected in [
    ("gui.c", '"EndraGBA Run013"', '"EndraGBA Run014"', 2),
    ("Makefile", 'Run013 Cheats" --version 13', 'Run014 File Cheats" --version 14', 1),
]:
    path = root / filename
    text = path.read_text()
    assert text.count(old) == expected
    path.write_text(text.replace(old, new))
