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
