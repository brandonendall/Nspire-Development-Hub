# EndraHome device test log — November 14, 2026

Hardware: TI-Nspire CX II CAS, user-reported OS 6.4.0.74 / Ndless environment.

## Run001 — unsafe on hardware

- EndraHome Run001 **compiled successfully** in GitHub Actions.
- The user provided a photograph of the native EndraHome **SHORTCUT SETTINGS** screen, demonstrating that the program started and drew UI on the calculator.
- **Observed failure 1:** after reaching the shortcut settings/help screen, the whole device froze, requiring a hardware reset.
- **Observed failure 2:** user tried Run001 again and selected a launcher app (GBA). The whole device froze immediately again, also requiring a hardware reset.
- No verified exception log or stack trace accompanied these two reports.

**Classification: physical device test FAILED; do not relaunch Run001.**

### Unknowns / competing hypotheses

The screenshot alone does not establish whether the source is SDL/nSDL input handling, SDL shutdown/initialization, nested Ndless `nl_exec` execution, memory ownership or compatibility, or another cause. Do not label any one as a proven root cause.

### Diagnostic plan

Run002 Safe strips out SDL and child-app execution, plus all configuration and filesystem I/O. It uses only the existing tested Ndless lcd_init/lcd_blit approach plus discrete key scans. ESC exits; any navigation/Enter/Menu tests must be performed separately. The 30-second software auto-exit **cannot** recover from a hardware-level hang.

Only reintroduce SDL and then launcher execution in **separate subsequent builds**, after physical confirmation.

## Run002 — physical test FAILED (2026-11-14)

- The user photographed the native 'SAFE 002' home screen, showing keyboard/screen diagnostics and a decreasing automatic-exit countdown.
- A second photo shows the 'SAFE INPUT DIAGNOSTIC' Menu/Help screen with a visible 05s countdown.
- The user reported the calculator completely froze after entering Menu. The exact instruction is unknown: the hang may have happened during menu handling or during the subsequent auto-exit/cleanup path. No exception trace was supplied.
- **Classification: failed device test. Do not relaunch Run002.** No further tests should claim that its native lcd_init/lcd_blit pathway is a verified fix.

## Run003 — encouraging physical success (2026-11-14)

- Ported the working gpSP-derived keypad, display mode/portrait triple buffering and state-driven menu architecture to a standalone app.
- User supplied a photo showing EndraHome Run003's home screen, with ZIP selected, and reported: **"Works like a champ but boy is it ugly"**.
- This supports successful display rendering and at least tile selection/navigation on the actual device. It does not prove every help/detail/exit interaction was independently checked.
- External Ndless app execution was intentionally **not implemented**.

## Run004 — user-confirmed working visual redesign

- Same Run003 platform/input/logic source compiled unchanged, with a new `run004/render.c`, `run004/main.c` and four improved screens.
- Host navigation tests, native compiler job, PPM screenshot artifact are tracked by `.github/workflows/endrahome-visual004.yml`.
- **User provided a photo of Run004 on actual CX II CAS and said “It works,” while comparing it to a more sophisticated original seven-icon visual concept.** This confirms a successful home-screen rendering and the user's overall working report; it does not independently verify every detail/exit path. No app launching, ZIP, hooks, config modification or filesystem access.
- Maintain Run003 as recovery baseline; do not silently replace or rename it.

## Run005 — concept-inspired seven-icon UI (2026-11-14)

- User supplied a side-by-side photo showing Run004 physically working and a more polished visual reference with a big EndraHome cube, seven glossy icons, and a refined footer.
- Added `run005/` retaining unmodified Run003 native video/input module, with new seven-card state logic and its own RGB565 concept-inspired renderer.
- GitHub Actions run [43599465872](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43599465872) **passed** host 7-item navigation tests, four preview images and Ndless native .tns compilation.
- **Physical calculator test: NOT YET REPORTED.** Seven cards are preview-only; do not claim they launch apps. The status pictogram is decorative.
- Keep working Run003/Run004. First test Run005's immediate ESC return, then other navigation controls; stop on any freeze.

## Run005 — actual calculator report (2026-11-14)

User supplied a new photo of EndraHome Run005 displayed on the TI-Nspire CX II CAS with the cube logo and all seven icons; user explicitly said **“It works build the next one.”** This validates physical home-screen rendering at minimum, and user-reported operation. It is not direct evidence of every menu/exit edge case. The photo shows that long icon captions run into neighboring captions, which Run006 attempts to correct. No actual app launch was possible in Run005.

## Run006 — initial controlled launch probe (2026-11-14)

- New `run006/` source builds a small independent `EndraHome_Test006.tns` and gated `EndraHome_Run006_LaunchProbe.tns`.
- GBA-style native platform from Run003 and Run005 navigation core remain unchanged.
- Tools > Enter (details) > Enter (confirm) > Enter (test) is the only path that calls `nl_exec`. Before launching, the launcher restores OS video/timer; it passes zero additional arguments. No GBA or other real apps are launched.
- Tiny caption readability fix: split Endra/GBA, Endra/ZIP and Endra/Cmd labels into two lines.
- Host state tests and preview screenshots are tracked in workflow `endrahome-launch006.yml`. **Calculator test not yet reported.** First test companion directly, then launcher ESC, then navigation, then confirm launch. Stop at any freeze.

## Run006 — user-reported device PASS (2026-11-14)

After the Run006 companion-app, launcher navigation/ESC, and guarded Tools launch instructions were provided, the user reported **“Everything works.”** Record this as a **user-reported pass of the full Run006 test sequence**, including the small `EndraHome_Test006.tns` handoff. No detailed step-by-step logs or crash traces were provided, so this does not prove stability across different Ndless applications or GBA execution. Preserve Run006 as a known user-reported working baseline.

## Run007 — real ZIP app controlled test (new, hardware untested)

Run007 adds ZIP-only real app execution after a separate confirmation, preserving unchanged Run003 display+keyboard platform, unchanged Run005 navigation and Run006 corrected icon captions. The ZIP app is **not bundled**: user explicitly copies a working ZIP executable alongside the launcher under `EndraHome_ZIP007.tns`. Tools repeats the small companion test; EndraGBA and every other icon remain preview-only. Test direct ZIP first, launcher ESC second, Tools test third, ZIP nested launch last. This is *not* evidence that other apps can launch safely.

## Run007 — ZIP-only plan superseded (2026-11-14)

A compiled ZIP-target-only Run007 was proposed but the user explicitly rejected ZIP-specific behavior: EndraHome is meant to be a **general purpose home screen and configurable app launcher**, not a ZIP utility. This ZIP-only build was **not physically tested** and is now superseded; its source remains only in Git commit history for audit.

## Run007 — UNIVERSAL REDO (2026-11-14, physical test pending)

- New Run007 source in `run007/` accepts seven independent app shortcuts from a read-only `EndraHome_Apps007.cfg.tns` file beside `EndraHome_Run007_Universal.tns`.
- All paths start empty. The user must assign `/documents/.../*.tns` targets and optional labels; each selection requires a separate confirmation. There is **no ZIP-only logic**, no built-in utilities, file scanning, hooks, or config writes.
- Preserves unchanged GBA-derived Run003 platform, Run005 navigation and Run006 display-restore-before-`nl_exec` sequence.
- GitHub Actions [run 43604524140](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43604524140) **passed** host logic tests, render previews and native Ndless build. Physical calculator test **not reported**. First recommended target is the previously working small Run006 companion; arbitrary Ndless/GBA launches remain unverified.

## Run008 — readability + setup diagnostics (2026-11-14)

The user showed a Run007 Universal home photo: rendering confirmed, no explicit device test of app launching yet. Long dock captions were still crowded. Run008 retains the Run003 GBA-native input+LCD layer, Run005 navigation and Run007 universal config/launch code unchanged, but abbreviates default captions and adds home/menu configuration status. It uses the SAME `EndraHome_Apps007.cfg.tns` file as Run007. [GitHub Actions run 43606891909](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43606891909) passed host and native verification. **Run008 physical device test still pending.** Initial test should be immediate ESC + labels/status, then only the known-working Run006 tiny program as configured shortcut. Do not assert real GBA/other app launching works.

## Run008 — real device successful test + timing (2026-11-14)

The user supplied photographs showing (1) EndraHome Run008's Tools detail panel reporting “Shortcut configured” and “READY – ENTER TO LAUNCH,” (2) its “LAUNCH APPLICATION?” confirmation screen, and (3) the **EndraHome Test 006 success message** displayed over the regular TI Documents browser. **This confirms the universal config-enabled launch path for the tiny Run006 companion program on physical hardware.** User then measured **exactly 10.04 seconds between the final confirmation Enter and the test application's appearance.** This is a significant usability performance issue, not a failure to launch. Do not claim any other app or GBA is safe without its own test.

## Run009 — new experimental timing comparison (2026-11-14)

- Source: `run009/`. A version-matched filename is now mandatory: `EndraHome_Apps009.cfg.tns` next to `EndraHome_Run009_FastHandoff.tns`. No fallback to 007/008. New builds should always increment the three-digit suffix in the configuration filename.
- Removes redundant preflight target open/close and the explicit 180-ms pause before `nl_exec`, **without altering** the proven native LCD/timer/OS restore-before-launch sequence or keypad/platform module.
- Expected performance savings are uncertain and **cannot be assumed to explain 10.04 seconds**. Compare Run009 final Enter-to-popup time against the Run008 10.04-second baseline and, ideally, direct TI Documents launch time.
- Physical test pending. Preserve working old builds and record exact result.

## Run009 — stopwatch photograph (2026-11-14)

The user submitted a Timex CHRONO photo showing **9.94 seconds** after the Run009 speed-test request. This is consistent with a Run009 final-Enter-to-test-popup interval, but the stopwatch image alone does not prove which build was running or the exact start/stop events. Tentative comparison to Run008's 10.04-second interval: **0.10 seconds (about 1.0%) lower**. App-loader speed remains effectively unresolved; no proof of a substantial optimization. Next: time launching the same tiny test program directly from TI Documents, optionally repeat timing for Run008 and Run009.

## Run009 — mislabeled help screen diagnosed (2026-11-14)

Photo of Run009 Help shows `CONFIG: FILE LOADED`, **`EndraHome_Apps007.cfg.tns`**, and `Beside EndraHome Run009`. The source loader is actually built with the versioned **Apps009** macro; but the renderer inherited Run007's old header before the 009 header could define the correct name, due to a common header guard. This explains the mixed text. The Run009 code was patched to use its own `run009/render.h` with a version-matched header, plus compiled-artifact CI checks for 009 present and 007 absent. Do not tell the user that this photo proves Run009 loaded the 007 config. The new binary requires physical confirmation.

## Run010 — shutdown-path diagnosis (2026-11-14, pending physical test)

User explicitly clarified that EndraHome itself takes approximately **10 seconds to close**. The measured 10.04s (Run008) and 9.94s stopwatch photo (Run009 context) cover end-to-end handoff and don't yet separate time spent closing Home from child app loading. Source inspection found `refresh_osscr()` in GBA-derived `eh_platform_stop()` and a second call after `nl_exec` in the launcher. Ndless docs tie that function to refreshing the Documents browser after file create/delete, and a community developer warned it can be slow with many files. Run010 isolates this by omitting these refresh calls ONLY while preserving the known-working LCD/timer restoration and launcher state machine. It strictly expects matching `EndraHome_Apps010.cfg.tns`. **No physical calculator result yet.** First time Esc-only exit to TI Documents; if that fails, return to Run009. Then compare the tiny Tools test if safe.

## Run010 — permanent config policy correction, device pending (2026-11-14)

User superseded the earlier per-build naming request: **no Run number in the shortcut config**. Run010 and future releases use `EndraHome_Apps.cfg.tns`. Old Run007/008/009 binaries remain unchanged. User can manually copy their configured file to the permanent filename once. An imported Run007 renderer header could make Run009's Help page show Apps007 despite the loader using 009; Run010 adds a local renderer header, aligning Help with the correct permanent name. The no-`refresh_osscr()` shutdown optimization is still **not yet physically tested**. Start with Esc-only exit; stop on any blank or frozen screen.

## Run010 — confirmed fast exit and working test launch (2026-11-14)

User report after installing Run010: **"Android Home works great. It exits out real fast. It's perfect, really. It launches the file immediately, and then it goes back out."** In context the user means EndraHome (not an Android phone home app). This is a physical-device PASS of fast Esc/exit and the simple Tools handoff, relative to roughly 10-second Run008/009 delays. No exact new speed measurement was supplied, and the no-`refresh_osscr` change is a plausible cause but not rigorously isolated by a direct timing phase trace. User's next requirement: child should return to EndraHome, not stay in TI Documents.

## Run011 — Return Home attempt (2026-11-14, physical test pending)

New `run011` session loop preserves Run010's no-browser-refresh LCD/timer restoration and calls the Ndless API `nl_exec(path,0,NULL)` with the native graphics platform stopped. **If** `nl_exec` returns after the child exits, Run011 clears modal/launch state and restarts the known-good LCD/UI, retaining the selected tile. If a child resets/crashes/terminates its caller, automatic return may not happen. Host tests check that the return state does not relaunch automatically and that Esc still exits. Native CI and physical test tracked separately. Test only the tiny Run006 companion first; preserve Run010 baseline and stop if any freeze.

## Run011 — hardware test PASS (2026-11-14)

User reports successful Run011 return-home test: **"It worked perfectly."** Test followed previous request to launch small EndraHome_Test006.tns through Tools and return to EndraHome, with the transfer verified as 28,306 bytes by Endra Push v2. This is a USER-REPORTED PHYSICAL PASS for the tiny test child, not yet for GBA or third-party apps.

## Run012 — one-press launch (hardware test pending)

New user-requested controls: Enter on Home launches configured app immediately, Menu opens selected app info, Menu from info shows config help; no confirmation prompt. Run011's child return and Run010's fast exit preserved. Config stays EndraHome_Apps.cfg.tns, with Run011 preserved as rollback. See run012/README.md.

## Run012 — physical pass, Run013 Assign Picker pending (2026-11-14)

After Run012 quick-launch was tested with the small Tools companion, user confirmed: **"Yes, it was super fast."** This is user-reported physical pass of one-press launch speed, not a precise timed benchmark and not an arbitrary third-party app compatibility guarantee. New requirement: replace manual config path typing with EndraHome Menu → Info → Assign App → browse calculator Documents → select .tns → automatically persist slot.

Run013 is implemented on feature/endrahome-run001 at Projects/EndraHome/run013/; adds a bounded native Documents file picker using opendir/readdir/stat and a one-slot configuration update with a staged temporary file and rename. The existing permanent EndraHome_Apps.cfg.tns is modified **only on user-initiated assignment**. Existing labels, paths in other slots, and other lines/comments are preserved; if persistence fails, original config is left unchanged, and the UI displays SAVE FAILED. Run012/011 stay untouched as known-working fallbacks. **Run013 requires physical testing** even when host tests and native compilation pass, especially configuration writes on actual Ndless storage. Before testing, back up the user's configured EndraHome_Apps.cfg.tns; do not upload blank template over it. Start by assigning the tiny EndraHome_Test006.tns to Tools and verify persistence across restart; no GBA test yet.


## Run013 — In-app assignment save FAIL on hardware (2026-11-14)

**User-supplied physical TI-Nspire CX II CAS photo:** EndraHome **EndraZIP** info panel displays `SHORTCUT NOT CONFIGURED`, the visible `ASSIGN APP` button and the green error toast **`SAVE FAILED - CONFIG UNCHANGED`**. The picker apparently returned to the info page following file selection; no file path or errno is shown, so the failing operation cannot be definitively identified from the screenshot. The quick-launch and UI features otherwise remain unchallenged by this picture.

**Suspected cause:** Run013 `config_save.c` created a lengthy filename `EndraHome_Apps.cfg.tns.pending.tns` and attempted `rename(temp, configured_existing_filename)` directly. On Ndless/TI Documents, that may fail when destination exists or when the temporary filename is rejected. Treat this as a hypothesis only, not a confirmed diagnosis.

**Run014 fix + instrumentation:** `run014` adopts short temporary and backup sibling file names, verifies fully written temp config, moves existing permanent config into backup before renaming in validated new config, attempts rollback if install fails, and attempts startup recovery if an interrupted commit left only backup. A failed save shows the exact stage (e.g. CREATE TEMP FAILED, MOVE OLD FAILED, INSTALL FAILED) and errno to distinguish actual cause. Host tests and native CI passed in Actions run `43639502717`, but **device validation is still pending**. Back up the configured `EndraHome_Apps.cfg.tns` externally before the first Run014 write test. Preserve Run012 for rollback. Never overwrite an edited config with the blank template.

## Run014 and Run015 — shortcut save and manager physically confirmed (2026-11-15)

User tested Run014 SaveFix and replied **"It works great!!!"**, confirming the previously failing in-app shortcut assignment now saves properly on physical CX II CAS. This does not prove the exact cause of Run013's failed rename, but it confirms the new Run014 path works.

User then tested Run015 App Manager and reported **"Everything worked great"**, followed by a specific ergonomic request: replace Run015's on-screen alphabetical rename palette with direct hardware QWERTY input. Run016 QWERTY rename compiled and passed host tests, but no separate explicit physical keyboard typing report is recorded yet; don't infer physical success for that particular feature.

## Run017 — native Files and text preview physical PASS (2026-11-15)

User provided a **physical device photograph** displaying the dark **TEXT PREVIEW** interface with actual lines of a Termux/nlink text file opened through EndraHome's built-in native Files. The screenshot establishes read-only text preview drawing correctly and reading a real text file on the calculator. On follow-up asking whether **Esc returns from text viewer and EndraHome exits quickly without freezing**, user explicitly replied **"Yes it's perfect"**. Thus Files text preview, back navigation and quick exit are **user-reported hardware PASS**. This confirms no reported stall for that test but does not establish memory/performance behavior for all file sizes or formats.

Next: Run018 image viewer development using the supplied `mviewer.zip` (mViewer CX Edition 4.1, 2013) as reference for JPEG viewing, zoom/pan behavior, and careful review of source/license; retain EndraHome's own tested Files browser, native framebuffer and fast no-extra-refresh exit. Compatibility warnings should be avoided through real supported binary metadata and CX II-safe implementation, not by silencing the warning. mViewer source includes GPLv2 code and separately licensed libjpeg components; comply with corresponding licenses if copied. Do not transplant mViewer's own browser, legacy compatibility mode, or hooks. Preserve Run017 as the known-good fallback. JPEG viewer has **not yet been physically validated**.
