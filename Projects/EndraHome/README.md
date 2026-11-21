> **Run042, 2026-11-21 ET.** EndraHome adds a Files editor for existing plain-text TXT, Markdown and CSV files up to 4 KiB, including eligible .tns-wrapped names. Source `2527598a96b71d0e5e2945491282cdb40ac288a9` on `feature/endrahome-run001`. [Actions 44881971818](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44881971818) passed host and native tests/build. Native artifact **14698546393**, `EndraHome_Run042_Desktop.tns`, **445,170 bytes**, Zehn, SHA-256 `22a4adbbd70f71bd9cefb378ff95ed3130b5fa45dc736c9293699457baeb6746`. Preview artifact **14698122485**, 60 verified frames. Run042 calculator testing pending. Run041 remains the reported-working rollback, including JPEG viewing, zoom and trackpad pan. Transfer: `clear` then `push /storage/emulated/0/Download/EndraHome_Run042_Desktop.tns /upload`.

> **EndraHome Run041 preview build — 2026-11-21 ET.** Source `bc366cab90e0cb0cd7af3525be3df87917bb4c74` on `feature/endrahome-run001`. [Actions 44878619190](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44878619190) passed host and native builds. Artifact **14697087980**, `EndraHome_Run041_Desktop.tns`, **442,502 bytes**, Zehn, SHA-256 `cb352abafc67cf58e2551b5683fc9ff5c4dace1c0e9e5e580eec8ad05ef3ff50`. Preview artifact **14696134016** has 58 verified frames. Desktop names use their configured color on the wallpaper and only the selected name gets a ring. Run041 stores shortcuts and appearance in the existing `EndraHome_Apps.cfg.tns`, imports legacy appearance on first save, and preserves old files for rollback. Calculator testing pending. Run040 remains the rollback. Native Home > 5 > EndraHome registration is required for the completed shortcut release and remains blocked on exact-OS validation. Transfer preview only with `clear` then `push /storage/emulated/0/Download/EndraHome_Run041_Desktop.tns /upload`.

> **EndraHome Run040 — 2026-11-21 ET.** Source `5dbd09f8fac0c0080872160c191cfb449b223035` on `feature/endrahome-run001`. [Actions run 44874525701](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44874525701) passed host/native/report. Native artifact **14694778207**, `EndraHome_Run040_Desktop.tns`, **442,283 bytes**, verified Zehn and SHA-256 `0bef0ebf5c2eac88eb86b2ab4c9ab827bb1132c621d83219718f7d4413203043`. Preview artifact **14694954222** has 54 verified frames. Run040 adds separate Add App and Reassign Selected, validates targets before launch, and makes Files open /documents directly. Run040 hardware testing is pending. Run039 device photos show the desktop and picker and the owner reports failed assignment, flashed launches and incorrect Files navigation. The exact on-device failing stage was not captured. The TI Settings target is 9 if free, else the next supported free number such as 10. Registration is pending verified native routing and safe coexistence. Preserve Run039 and Run038 for rollback. Transfer with `clear` then `push /storage/emulated/0/Download/EndraHome_Run040_Desktop.tns /upload`.

> **Current EndraHome build — Run039, 2026-11-21 ET.** `feature/endrahome-run001`, source `Projects/EndraHome/run029/`, commit `e3020175a75b87276a553e1985ad3963986a2987`. [Actions #49](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44872738465) passed host, native Ndless and report. Artifact **14695005870**, `EndraHome_Run039_Desktop.tns`, **441,062 bytes**, Zehn, SHA-256 `bcf5a50a335b5ef16fd76189293e2c848f6c6285bb81ac48be612552f6d2f3c9`. Preview artifact **14694935436**, 53 verified 320×240 renders. Run039 calculator testing is pending. Owner's four Run038 device photos confirm Appearance, the Reactor/Dragon Core desktop with four configured shortcuts, compact Files, and a read-only stock TI Settings pointer result. Run038 is the primary rollback. Native registration, real Ctrl+Home standby, and physical file mutations remain unverified. Transfer with `clear` then `push /storage/emulated/0/Download/EndraHome_Run039_Desktop.tns /upload`.

> **Current EndraHome build — Run038, 2026-11-20 ET.** Active branch `feature/endrahome-run001`, source `Projects/EndraHome/run029/`, commit `8bc8d111a577e0783d3290d16e014362d0c455c5`. [Actions #48](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44867780859) (ID 44867780859) passed host, native Ndless, and report jobs. Native artifact **14691888846**, `EndraHome_Run038_Desktop.tns`, **440,638 bytes**, Zehn, SHA-256 `dba8c3cfd218b204d20b8788e0eb637e2e6a2afc5b57f19bec5e82b7ba16e9fd`. Preview artifact **14692278912** contains 33 verified 320×240 renders. **Run038 physical calculator testing is pending.** Run037 is the rollback. Its physical photos confirm green theme rendering and a stock Settings menu pointer with only items 1–8 visible. File mutations and OS standby are unverified. Native TI Settings registration and true Ctrl+Home standby remain open. Transfer with `push /storage/emulated/0/Download/EndraHome_Run038_Desktop.tns /upload` after `clear`.

# EndraHome — current project status (2026-11-14)

Repo: `brandonendall/Nspire-Development-Hub`. **Active code branch:** `feature/endrahome-run001`, [draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10). The root `main` branch has the [Hydra handoff](../../handoff/HYDRA-START-HERE.md) and [AGENTS.md](../../AGENTS.md). Inspect current branch sources before making claims.

## Current build

Run029 Desktop builds on Run028 Dynamic Launcher, the owner's installed baseline. [Run029 source and controls](run029/) · [Current build and hardware status](PROJECT_MEMORY.md) · [Run028 source](run028/).

## Hardware test history

| Build | Result | Meaning |
|---|---|---|
| Run001 | **FAILED** (full device freeze) | Original SDL/nSDL menu and GBA launcher froze. Do not use. |
| Run002 | **FAILED** (full device freeze) | Native `lcd_init` Menu/Help diagnostic froze. Do not use. |
| Run003 | **User-reported working** | UltraGBA/gpSP-derived native display/input rendered UI; ZIP tile selected in photo. |
| Run004 | **User-reported working** | Four-icon visual UI displayed on device, but didn't match the target appearance. |
| Run005 | **User-reported working** | Seven-icon cube-logo UI displayed on device; some captions overlapped. |
| Run006 | **User-reported working** | Corrected captions; Tools icon launched the standalone tiny companion test app after confirmation. |
| Run007 ZIP-only idea | **CANCELLED** | Was compiled but **not user-tested**. Superseded by the user-requested universal design. |
| **Run007 UNIVERSAL REDO** | **CI verified; device test pending** | Seven configurable app targets; **none** preassigned. No special ZIP behavior. |
| **Run008 UI polish** | **DEVICE PASS for tiny Tools app:** photos show configured Tools, confirmation screen, and success popup. Measured **10.04 s** from final Enter to target opening. | General GBA/large app compatibility not yet tested. |
| **Run009 Fast Handoff** | **CI verification and device test pending** | Expects exactly `EndraHome_Apps009.cfg.tns`, skips redundant target pre-open and 180-ms sleep; compare timed launch to 10.04 s baseline. |

## The actual Run007: configurable launcher

[Run007 source and setup](run007/README.md). [GitHub Actions build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43604524140): host tests, renderer previews, native Ndless executable and SHA256 checksum **PASSED**. This is **not physical compatibility confirmation**.

- Launcher executable: `EndraHome_Run007_Universal.tns`.
- Companion plain-text file: `EndraHome_Apps007.cfg.tns`, placed beside the launcher.
- Seven colorful dock icons, 0–6. Each can have **any** explicit compatible Ndless `.tns` executable target under `/documents/`; icon names can be edited independently of the target paths.
- **All shortcuts ship disabled** to prevent accidental launch. A read-only parser validates complete paths and rejects traversal, root, system and non-TNS targets. Missing/unconfigured entries show a setup notice.
- User selects icon → Enter (details) → Enter (confirmation) → Enter (launch). Esc cancels. No filesystem scanning, built-in archiving, file management, app installation, `ndless.cfg.tns` changes or resident process.
- On launch, **restore LCD/timer/OS screen first**, then `nl_exec(path,0,NULL)`; this is the **user-reported working Run006 handoff pattern**. Launched apps close to TI Documents, **not** back into the EndraHome UI.
- Display/touchpad hardware handling is compiled **unchanged from `gba-ui/platform.c`** (Run003); seven-way navigation is compiled **unchanged from `run005/logic7.c`**. The Run006 caption fix is retained.

### First Run007 test

1. Keep working Run006 unchanged as rollback; do not overwrite UltraGBA or any other apps.
2. Put the Run007 binary and `EndraHome_Apps007.cfg.tns` in the same Documents folder. Set **only one initial app** to the known-working `EndraHome_Test006.tns`. Example (only if this path matches the actual calculator): `5=Tools|/documents/EndraHome/EndraHome_Test006.tns`.
3. Verify direct launch of the tiny app from TI Documents, and Run007 immediate Esc exit.
4. Navigate dock icons, Menu, unconfigured paths, detail view, then confirm one test launch. Confirm return to TI Documents.
5. Only then try a different independent **lightweight** Ndless app. Save EndraGBA for a separate compatibility trial; it may have higher memory/video demands. Stop testing after any freeze.

See [device test log](DEVICE-TEST-RESULTS.md) for all observed user reports, including failures.

## Source directory map

- `src/` — Run001 SDL (failed, retained historically).
- `recovery/` — Run002 diagnostic (failed).
- `gba-ui/` — Run003 gpSP-derived display/keyboard baseline (preserve).
- `run004/` — visually improved four-icon baseline.
- `run005/` — seven-icon cube-logo UI with pure state navigation.
- `run006/` — proven small companion-app launch pattern and caption fix.
- `run007/` — **current** universal configurable launcher; prior ZIP-only experiment exists only in git history.
- `GBA-ARCHITECTURE-ANALYSIS.md` — explanation of reused native engine.

## Hydra / AI work policy

Record source version, CI outcome and separate **physical** device result for every build. Preserve Run003–Run006 rollback artifacts, current UltraGBA Run007/RTC/GBZ/save behavior, and all unrelated apps. Keep EndraHome on the development branch and draft PR until ready. Do not report the universal launcher as fully tested merely because the tiny Run006 test handoff worked.

## Run008 — readable labels and visible config status (2026-11-14)

**New source:** [run008/](run008/). [Passing CI run 43606891909](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43606891909) includes host tests for the caption layout and original seven-slot configuration/state tests, 4 full-screen previews, and a native Ndless `.tns` build with SHA256 verification. **User-provided photographs now confirm that Run008 correctly displays Tools as configured, shows the launch confirmation, and successfully starts the Run006 companion app. Measured final-confirmation-to-app interval: 10.04 seconds.** This does not establish that arbitrary Ndless applications (particularly the emulator) will work.

The user's Run007 calculator photo showed successful UI rendering, but labels still clipped/overlapped and all seven icons showed unconfigured gray dots; the user has not yet explicitly confirmed that a config file loaded or any Run007 app handoff worked. Run008 is a conservative improvement that shortens default labels to GBA, ZIP, Cmd, Files, Images, Tools, Favs, while preserving full names on detail pages. It also shows `CONFIG FILE MISSING`, `CHECK CONFIG FILE`, or `N / 7 READY` directly on Home; Menu explains configuration status.

**Important:** Run008 deliberately reads `EndraHome_Apps007.cfg.tns`, the same config filename and path semantics as Run007. The user should put that file alongside `EndraHome_Run008_UIPolish.tns` and edit a single slot (preferably the already-working Run006 tiny companion program) to test the universal launcher. All seven slots default disabled. This is NOT app autodiscovery or built-in ZIP functionality. The Run003 native input/video, Run005 seven-way logic, Run007 registry/parser, confirmation overlay and `nl_exec` sequence are compiled unchanged; only Run008 renderer/captions and executable title are new. **Test app launch separately; no GBA/universal-launch success claim until user verifies.**

See [run008/README.md](run008/README.md) for installation and safe test order. Preserve Run003–Run007 binaries and any user save files.

## Run009 — current speed experiment & required release numbering

[Run009 setup](run009/README.md) and [CI workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-run009.yml). The user timed **10.04 seconds** from final Enter confirmation in Run008 until the tiny test program actually appeared. Run008 therefore has a **successful real device test for this one target, with unacceptable launch latency**. The cause of the bulk of that delay remains unknown.

**Strict naming policy, starting Run009:** EndraHome Run`NNN` must load only `EndraHome_AppsNNN.cfg.tns`. Thus Run009 requires `EndraHome_Apps009.cfg.tns`, not 007 or 008. On the calculator, the user will copy/rename their existing edited 007 config to 009. Paths and labels inside remain unchanged. The program and its Menu help derive the same suffix from `EH7_CONFIG_FILE`, backed by a test assertion. Keep earlier versioned files installed so older builds still work independently.

**Run009 launch change:** after the tested LCD/timer/OS restore, no more redundant `fopen/fclose` of the target and no extra 180-ms pause before `nl_exec(path,0,NULL)`. Loader handles missing targets and errors. Keep this as a **measured experiment only**; removing 180 ms cannot by itself account for the observed 10.04 seconds. Native/device compatibility must still be tested.

**Next test:** run same tiny program from Run009 and time *final Enter → success popup*, then compare to the 10.04-second Run008 measurement. Also time direct launch of the program in TI Documents: this helps estimate how much delay comes from the app loader rather than EndraHome itself. Stop immediately after a freeze, preserving working Run006/Run008.

## Run009 stopwatch evidence

A new stopwatch photograph shows **9.94 seconds** in the context of the Run009 launch-speed test. Assuming it measured the same final confirmation Enter → test app popup interval as Run008, the result improves on **10.04 seconds by 0.10 seconds (about 1%)**. The photo only shows the watch and does not independently identify the build or endpoints; record as **candidate Run009 measurement** rather than definitive proof of improvement. See [launch performance log](LAUNCH-PERFORMANCE.md). Next diagnostic: direct launch of the test executable from TI Documents, with the same timing conventions, to estimate platform loader time.

## Run010 — isolate EndraHome's slow shutdown (device test pending)

User clarified on November 14, 2026: **"It takes our Endra home about 10 seconds to close out."** This changes the working diagnostic hypothesis from generic child loading to the launcher shutdown/OS handoff. The prior end-to-end measurements were 10.04 seconds (Run008) and a stopwatch photo showing 9.94 seconds (following Run009); neither separately measured the exit phase.

**Code review of the actual Run009 source** showed `eh_platform_stop()` calls `refresh_osscr()` immediately after restoring the original LCD registers and timer; Run009 `main()` calls `refresh_osscr()` AGAIN after child execution. Ndless documentation describes `refresh_osscr` as a document-browser refresh when files are created/deleted, while EndraHome only reads its configuration. Nspire community advice warns it is slow on calculators with many files. That is a **specific plausible cause**, not proven.

[Run010 source](run010/) copies the working Run009 display/input platform locally and **removes only the `refresh_osscr()` calls** in the local platform stop and main exit code. All hardware LCD/timer/OS base restoration, existing UI/key logic, config rules, and `nl_exec(path,0,NULL)` are preserved. Run010 requires **`EndraHome_Apps010.cfg.tns`** beside `EndraHome_Run010_NoRefresh.tns`; do not modify the working Run009 config.

**Primary device test:** launch Run010 with no app selected and press **Esc**, timing return to usable TI Documents. Stop if the screen is blank, garbled or frozen; use Run009 as rollback. Only if Esc works, time Tools' tiny `EndraHome_Test006.tns` launch. This isolates whether the OS browser refresh caused ~10s. **No Run010 physical result yet.**

## Stable app shortcuts config — user-requested (2026-11-14)

**Supersedes the previous “match the RunNNN number in the config” rule.** From Run010 onward EndraHome reads **`EndraHome_Apps.cfg.tns`**, without a number. Only the launcher executable uses a Run number. The config remains in the same TI Documents folder as the launcher and is read-only from EndraHome. Users can copy their configured 007/009 file once to the stable name; older builds retain their older expectations. Do not auto-modify or delete their files.

Run010 includes a local renderer header to fix an old-help-filename issue seen in Run009: its Help screen showed Apps007 even though the actual Run009 loader expected Apps009. Run010's Help and loader both use the stable filename. The separate Run010 **no-browser-refresh** exit-speed experiment remains untested on physical hardware. [Test instructions](run010/README.md).

## Run011 — experimental Return Home session

**Run010 hardware PASS (user report):** user explicitly confirmed Run010 now exits very quickly, launches the tiny Tools target immediately and no longer suffers from the approximately 10-second shutdown delay. Its removal of unnecessary OS document-browser refresh is the first tested workaround. This is a user report for the known test app, not verified compatibility for arbitrary programs.

**Run011 source:** [run011](run011/) — implements the requested behavior after a launched child exits: EndraHome releases the LCD/timer, calls Ndless's `nl_exec(path,0,NULL)`, and, **when the child call returns**, safely reinitializes EndraHome graphics/input and shows the seven-icon Home again with the same selected dock icon. It clears request and confirmation state so the same app never relaunches automatically. Escape from Home still exits to TI Documents as Run010 did. All config access still uses the permanent `EndraHome_Apps.cfg.tns` (do not overwrite edited config with blank template). Run010 remains unchanged as a tested rollback.

**Device test pending:** follow [run011/README.md](run011/README.md). Test Esc-only exit first, then only the known tiny `EndraHome_Test006.tns`; dismiss its success message and see whether EndraHome reappears. Ndless documentation describes `nl_exec` but does not guarantee every third-party app returns to this callsite; resets/crashes/emulators might still exit to Documents. No resident hooks or OS modifications.

## Run013 — in-app Assign shortcut

User confirmed Run012 one-press launch is fast on hardware. Run013 implements the requested Menu → App Info → **ASSIGN APP** button → browse `/documents` → select a `.tns` file; the shortcut is saved into the same permanent `EndraHome_Apps.cfg.tns` file. Preserves Run012 quick launch and Run011 return-home. **Back up configured file before the first hardware test; physical write persistence not yet confirmed.** Instructions and source: [run013/README.md](run013/README.md). Keep Run012 as fallback.

## Run029 — requested integrated desktop, typography, power, OS launcher (2026-11-18)

**Original Run029 requirements.** The current implementation and verification are in [the Run029 notes](run029/) and [project memory](PROJECT_MEMORY.md). Base implementation on Run028 DynamicLauncher, preserving the working Endra Files browser, read-only preview, file operations, fast exit, return-home, dynamic shortcut registry, and the permanent `EndraHome_Apps.cfg.tns`. Preserve Run028 as rollback; do not require calculator connection during development.

1. **Always-visible Files tile:** Home must always display slot 3 as Files. When its assigned external .tns path is blank, Enter opens the built-in Endra Files browser directly; when assigned, Enter launches that file manager. If the external executable is missing or fails to launch, fall back to built-in Files safely. Keep assignment/change capability and never silently erase a configured path.
2. **Legible icon captions:** Replace Run028 `render.c`'s cropped five-pixel glyph rendering (which truncates columns from the 8x8 bitmap) with full-width glyphs or a properly designed small bitmap font, appropriate spacing, center alignment, and truncation/wrapping bounded to each tile. Apply the same improvement to Files folder names and avoid overlapping UI.
3. **Ctrl+ON:** Add shortcut detection only after verifying TI-Nspire CX II CAS OS 6.4 / Ndless key mapping and a safe documented standby/power API. Do not directly modify power registers or hook OS power handling without validation. A graceful exit to OS is an acceptable fallback, but must not be misrepresented as powering off.
4. **TI native Settings menu option 9:** Research whether OS Home -> 5 Settings can safely register an Ndless app shortcut. Do NOT modify firmware, OS menus, `ndless.cfg.tns`, or persistent hooks merely to imitate native integration. If no supported extension point exists, document a reversible quick-launch alternative as a separate opt-in feature.
5. **Hardware preservation:** compile and host-test all changes in CI first; test a single release on device, with safe Esc-only exit first and no repeated resets. Never overwrite user configuration or previously working TNS files.

**Engineering distinction:** The OS Settings entry is a feasibility investigation, not a guaranteed feature of Run029. No native OS integration is implemented merely by adding a menu item to EndraHome itself.
