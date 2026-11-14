# EndraHome Run009 — release-numbered config and measured launch-speed experiment

**User's confirmed Run008 result (November 14, 2026):** photographs show a configured Tools shortcut on the details page, the confirmation prompt, and the **Run006 tiny companion success popup in TI Documents**. The user timed **10.04 seconds from pressing Enter to confirm launch until the test program appeared**. That measurement is the baseline. It is a real success for *this* small companion app, not proof that all large apps launch safely.

## Requested changes

1. **Version-matched config file:** Run009 searches **only** for `EndraHome_Apps009.cfg.tns`, alongside `EndraHome_Run009_FastHandoff.tns`. No fallback to Apps007/Apps008. Existing 007/008 versions remain unchanged. Future builds should bump the three-digit suffix consistently; use the versioned config macro `EH7_CONFIG_FILE` for both loader and help screen.
2. **Small fast-path experiment:** The Run008 launcher restored the screen, separately opened+closed the program file, slept 180 ms and then called `nl_exec`. Run009 removes the redundant preflight `fopen/fclose` and 180-ms sleep, and calls the loader directly **after** validating the path and performing the exact previously proven display/timer/OS restoration. Ndless remains responsible for returning errors when a file cannot launch. **We do not claim these changes solve the 10.04-second delay.** That much time likely includes loader/OS overhead that these simple changes cannot remove, and it needs measured device comparison.
3. **No extra architecture changes:** Same Run003 platform, Run005 navigation, Run007 registry semantics and confirmation flow, Run008 renderer and captions. No dangerous loading while native video is active, root access, OS hooks, or config writes.

## Prepare the config on the calculator

Make a **copy** of your already-edited `EndraHome_Apps007.cfg.tns`. Rename the COPY exactly:

`EndraHome_Apps009.cfg.tns`

Place that next to the **new** `EndraHome_Run009_FastHandoff.tns` in TI Documents. Keep the old 007 file for Run007/008. The content format stays identical:

```ini
0=EndraGBA|
1=EndraZIP|
2=EndraCmd|
3=Files|
4=Images|
5=Tools|/documents/EndraHome_Test006.tns
6=Favorites|
```

The path here is just an **example**, adjust it to match the actual path of the tiny test app. All default paths in the bundled config are empty until configured.

## Physical test (safe, repeatable)

1. Preserve Run006 and Run008 .tns binaries. Launch Run009 and test immediate ESC return to TI Documents.
2. Verify the Menu help names **EndraHome_Apps009.cfg.tns** and Home reports `1 / 7 READY` when only Tools is assigned.
3. Select **Tools → Enter (details) → Enter (confirm) → Enter (launch)** and **time from that final Enter until the SUCCESS popup appears**, as you did for Run008.
4. Compare **Run008: 10.04 s** with **Run009: actual measurement**. For a useful comparison, also time opening `EndraHome_Test006.tns` *directly from TI Documents*: if direct load is also slow, most of the delay is likely in Ndless/OS startup rather than EndraHome.
5. If anything freezes, stop, report the last key/screen. No GBA test yet.

Avoid interpreting a new fast-feeling confirmation dialog as evidence of faster loader startup. Only the measured **final Enter → success popup** interval counts.

## Build/CI

- `make -C Projects/EndraHome/run009 test`: parser/state and caption tests, including strict filename 009 assertion.
- `make -C Projects/EndraHome/run009 preview`: 320x240 PPM UI snapshots.
- `bash toolchain/build.sh Projects/EndraHome/run009`: native executable.
- [Workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-run009.yml) builds, tests and publishes the `.tns` plus the matching config file.

App-specific GPL-2.0-or-later obligations continue from the adapted gpSP display platform.

## Correction: Run009 Help screen printed 007 (2026-11-14)

The owner photographed Run009's Help screen showing `CONFIG: FILE LOADED`, then the **wrong label** `EndraHome_Apps007.cfg.tns`, and `Beside EndraHome Run009`. Source review identified an include-order bug: `run009/render.c` included the Run007 `render.h` before its own versioned registry header, so the shared header guard caused the **renderer** to inherit the **Apps007** macro even though the **Run009 loader in `main.c` uses Apps009**. The screen label was misleading; it does not establish that Run009 opened an Apps007 file.

**Fix:** add a local `run009/render.h` which includes the local 009 registry first; point `main.c` and `preview.c` to it, and have CI verify the actual compiled preview **and** native ELF contain `EndraHome_Apps009.cfg.tns` but not `EndraHome_Apps007.cfg.tns`. This is a non-invasive display/build fix; the native key/display platform and launch lifecycle are unchanged. This revision still expects `EndraHome_Apps009.cfg.tns` alongside the launcher. The physical calculator screenshot was from the earlier revision; the corrected rebuild still needs user test.
