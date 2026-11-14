# EndraHome Run007 — universal configurable Ndless app launcher (REDO)

**Supersedes the former ZIP-only Run007 experiment** (preserved in Git history, never approved as user-tested). EndraHome is a *home screen and generic launcher*, NOT a ZIP utility. EndraZIP is only one possible, user-configured application shortcut, exactly like EndraGBA or any other Ndless app.

**Status:** Host and native build verification in GitHub Actions; physical TI-Nspire CX II CAS test **pending**. Previous Run006's tiny test program was reported working on hardware. Universal launching different apps, especially EndraGBA, is not yet verified.

## What changed (and what stayed unchanged)

- **New:** Seven configurable app shortcuts, each with a user-editable display label and absolute `/documents/... .tns` path. All seven default to DISABLED. The launcher does not scan root or automatically discover programs.
- **New:** Read-only `EndraHome_Apps007.cfg.tns` file placed beside `EndraHome_Run007_Universal.tns` in Documents. Simple text, easily edited on Android/Termux. No changes to Ndless configuration files or hooks.
- **New:** Custom dynamic labels, configured/disabled status indicator, per-app detail view and confirmation before any launch. The same seven colorful icon designs are preserved.
- **Unchanged:** Real-device working `../gba-ui/platform.c` native video/input/timer code from Run003 and `../run005/logic7.c` navigation, and Run006's proven sequence: **restore TI LCD/clock + OS screen before calling `nl_exec(path,0,NULL)`**.
- **Unchanged:** External apps return to normal TI Documents when they exit; EndraHome is not resident in memory and does not automatically reappear. A future version can study return-to-home behavior separately.
- **No built-in ZIP/archive/file management.** Any configured executable is a separate program, and the launcher doesn't perform its operations.

## Files to install (both in the same folder)

1. `EndraHome_Run007_Universal.tns`
2. `EndraHome_Apps007.cfg.tns` (plain text, user edited)

The launcher finds the config next to its own executable. **If the config is missing or all slots have empty paths, nothing can launch.** You can still navigate safely.

### Config format

Each line: `slot=VisibleLabel|/documents/Folder/Program.tns`, with slot 0 to 6. For example:

```ini
# This is just an example: use a path confirmed to exist on your own calculator.
0=EndraGBA|
1=EndraZIP|
2=EndraCmd|
3=Files|
4=Images|
5=Tools|/documents/EndraHome/EndraHome_Test006.tns
6=Favorites|
```

Paths shown in examples are NOT assertions about your device. The user must copy the true absolute path of a working program; the launcher refuses empty/untrusted paths and only reads files under `/documents/`.

The first seven icons are **shortcuts, not installed apps**. Assign each any compatible Ndless `.tns` you choose; labels are cosmetic and do not restrict executable types. No application scanning, installation or automatic label discovery is implemented in Run007.

## Device test sequence

1. **Keep Run006 installed and unchanged as the proven rollback.**
2. Make a copy of the already-working `EndraHome_Test006.tns` companion app into a Documents folder, if needed.
3. Put the new launcher and config together in that folder. Edit the **Tools slot (5)** to point to the actual companion app path.
4. Open Run007 and immediately press Esc: verify correct exit to TI Documents.
5. Launch again, navigate, Menu/help, Enter/details; verify unconfigured slots show a setup notice.
6. Select **Tools → Enter (details) → Enter (confirm) → Enter (launch)**. Verify the tiny companion success popup appears, then exit it.
7. **Only once that works,** configure another, independently working, low-risk Ndless program. Test it first from Documents, then from EndraHome with confirmation.
8. Avoid testing your GBA emulator until simple, smaller programs have passed. A working test-app launch does not guarantee emulator memory/video compatibility. If any freeze recurs, stop and report the last action.

Note: The seven icons are not a filesystem browser. Future releases may add an in-app shortcut editor and app discovery, but those capabilities are not claimed here.

## Build/CI

- `make -C Projects/EndraHome/run007 test`: host parser and UI state unit tests.
- `make -C Projects/EndraHome/run007 preview`: four RGB565 screenshot previews.
- `bash toolchain/build.sh Projects/EndraHome/run007`: native compiler in pinned Ndless SDK.
- CI: `.github/workflows/endrahome-universal007.yml`; packages the single `.tns`, editable config and README.

This code uses the gpSP-derived platform under the existing GPL-2.0-or-later project licensing; the bitmap font is public domain.
