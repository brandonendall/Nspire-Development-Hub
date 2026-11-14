#!/usr/bin/env python3
"""EndraGBA Run009: isolated experiment for native Documents refresh at exit.

Builds the proven Run007 feature set through Run008's branding-only patch,
then skips exactly one expensive-looking OS browser refresh on the quit path.

The normal LCD, timer, alignment, brightness, save and RTC code is untouched.
Never auto-edits Ndless config, stored games or device document associations.
Actual speed and UI restoration require CX II CAS hardware verification.
"""
from pathlib import Path
import runpy

root = Path("upstream-gpsp")
assert root.is_dir(), "Run from repository root with source in upstream-gpsp/"
runpy.run_path(str(Path(__file__).with_name("run008_branding.py").resolve()))

target = root / "nspire" / "nspire.c"
before = target.read_text()
old = """\ttimer_restore();
\tenableAlignmentExceptions();
\t/* The OS's file list doesn't see files gpSP moves or writes (zgpSP_saves,
\t * states): without this it kept showing files and folders that were gone. */
\trefresh_osscr();
"""
new = """\ttimer_restore();
\tenableAlignmentExceptions();
\t/* Run009 FastExit experiment: do not force a full native Documents
\t * browser refresh while exiting. All LCD/timer restoration stays intact.
\t * A new ROM-local save/state may not appear in the OS file list until
\t * Documents is reopened/refreshed. Revert if UI does not recover. */
"""
assert before.count(old) == 1, "Unexpected gpSP native restore layout"
assert before.count("refresh_osscr();") == 1, "Extra OS refresh calls need review"
after = before.replace(old, new, 1)
assert after.count("refresh_osscr();") == 0
for required in (
    "set_display_buffer(os_screen);",
    "HW_WRITE(HW_LCD_TIMING0, LCDTiming0);",
    "HW_WRITE(HW_LCD_TIMING1, LCDTiming1);",
    "HW_WRITE(HW_LCD_CONTROL, LCDControl);",
    "timer_restore();",
    "enableAlignmentExceptions();",
):
    assert required in after, f"Required OS restore disappeared: {required}"
target.write_text(after)

# Preserve unchanged save/exit call order and the legacy compatibility paths.
host = (root / "nspire_host_main.c").read_text()
for required in (
    "void nspire_leave_game(void)", "update_backup_force();",
    "nspire_config_close_game();", "void nspire_quit_game(void)",
    "nspire_leave_game();", "save_config_file();",
    "nspire_brightness_restore();", "nspire_restore();",
):
    assert required in host, f"Expected exit path missing: {required}"
paths = (root / "nspire_paths.c").read_text()
assert "UltraGBA_Data" in paths and "UltraGBA_Tests" in paths

# Deliberately preserve the Run008 build identity except for this test's label.
makefile = root / "Makefile"
mk = makefile.read_text()
assert mk.count('--name "EndraGBA Run008" --version 8') == 1
makefile.write_text(mk.replace(
    '--name "EndraGBA Run008" --version 8',
    '--name "EndraGBA Run009 FastExit" --version 9', 1
))
print("EndraGBA Run009: one OS refresh skipped; native LCD/timer restore and saves preserved")
