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
