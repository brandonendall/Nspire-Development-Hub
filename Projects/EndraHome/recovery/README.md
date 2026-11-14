# EndraHome Run002 Safe Diagnostic

**Why this build exists:** Run001 displayed the Shortcut Settings page on Brandon's TI-Nspire CX II CAS, then the calculator completely locked, requiring a hard reset. That screenshot confirms initial rendering and navigation into help; it does **not** identify the exact cause of the lockup.

Run002 intentionally isolates input, exit and native display rendering:
- **No SDL initialization or shutdown**; uses Ndless lcd_init / lcd_blit instead.
- **No app execution** (Enter merely shows "ENTER REGISTERED - LAUNCH OFF").
- **No file I/O, settings.cfg, shortcuts.cfg, filesystem browsing or root access.**
- **No hooks, clock changes, install/patch, ROM launch or system mutations.**
- ESC closes directly to the TI document browser; MENU toggles Help; arrow keys navigate.
- **Automatic exit after approximately 30 seconds** if the main loop remains responsive. This is **not a hardware watchdog**: it cannot rescue the calculator from a CPU/system-level deadlock.

## First physical test
1. If Run001 is frozen, perform the familiar hardware reset, then **do not relaunch Run001**.
2. Transfer only **EndraHome_Run002_Safe.tns** to a documents folder. Do not overwrite your existing apps.
3. Launch Run002 with Ndless. First immediately press **ESC** and confirm the TI browser returns.
4. If that succeeds, launch again, press **MENU**, **ENTER**, change icons with arrows, then ESC.
5. If that also succeeds, launch again and leave Run002 untouched for 30 seconds to test automatic exit.
6. If **any** freeze repeats, stop testing and capture which exact key/action caused the freeze.

These are diagnostic procedures, NOT claims of a verified fix. If Run002 passes, investigate Run001's SDL lifecycle and event interaction next, only then re-enable shortcuts in a separate build.

The source is at Projects/EndraHome/recovery/. Host state tests: make test.
Cross-build: bash toolchain/build.sh Projects/EndraHome/recovery.
