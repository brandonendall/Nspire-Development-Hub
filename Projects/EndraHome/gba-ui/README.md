# EndraHome Run003 — gpSP-style video/input

This is an **experimental diagnostic**, not a confirmed stable release. The real TI-Nspire CX II CAS froze on BOTH previous builds. Use a test device/backup and stop if this one locks.

## Architecture drawn from working UltraGBA source

Reference: `vendor/upstream-gpsp`, the checked-in snapshot of `andymcca/lr-gpsp-nspire` commit `2ddeb23b565cab84e20278bb73ed76f3d91b25f5`. Compare:
- `nspire/nspire.c` — saves LCD state, initializes three aligned display buffers, flips between buffers, and restores the OS LCD/timer state on exit.
- `nspire_input.c` and `nspire_keys.h` — keypad register reading, touchpad arrow mapping, rising-edge keys, direction hold repeat.
- `nspire_video_ui.c` and `gui.c` — menu graphics and screen transitions kept separate from core emulation.

Run003 reproduces the relevant concepts in its own standalone platform and navigation code, and adds **a completely different EndraHome visual presentation**. It is an adaptation, **not** a claim that source bytes or hardware behavior are identical to UltraGBA.

### What is different from Run001 and Run002?

- No SDL or direct Ndless `lcd_init` / `lcd_blit`. Avoids the original SDL lifecycle and the Run002 `lcd_init(SCR_TYPE_INVALID)` exit path.
- Native 240x320 **portrait** LCD handling and 320x240 logical UI (the gpSP scheme), plus triple buffering and saved hardware state.
- Direct keypad + touchpad edge detection based on gpSP, arrows repeat after 400ms at approximately 15Hz.
- Home screen, app-details page, information page and help page in a state machine independent of graphics.
- GBA-compatible Zehn metadata includes `--240x320-support=true`, absent from earlier EndraHome builds.
- No file reads/writes, no hooks, no ZIP, **no app launches** (`nl_exec` is intentionally absent), no auto-exit timer. Only ESC from Home triggers cleanup to TI browser.

### First test — only if you decide to try another diagnostic

1. Reset the device if required. Do not relaunch Run001 or Run002.
2. Put `EndraHome_Run003_GBAUI.tns` in an ordinary Documents folder, separate from EndraGBA.
3. Enter and immediately press ESC to test the OS display restore. If that fails, stop.
4. If first test passes, launch again. Use arrows to choose a dock icon, Enter to open details, Enter again for app information, ESC to return, Menu for help, then ESC from Home to quit.
5. If any action freezes, stop and report the **last action and last screen**.

**Compilation and host tests do NOT constitute a verified hardware fix.**

### Licensing

This project adapts the original GPL-2.0-or-later gpSP Nspire platform design. The original code and its license are retained under `vendor/upstream-gpsp/` (notably `vendor/upstream-gpsp/libretro-gpsp/COPYING`). The adapted Run003 sources are distributed under GPL-2.0-or-later. Public-domain 8x8 font comes from `Projects/UltimateCommander/vendor/font8x8_basic.h`. No ROMs or game saves are embedded.

Build with `bash toolchain/build.sh Projects/EndraHome/gba-ui`.
