> **CURRENT ENDRAHOME BUILD — Run035 VERIFIED (2026-11-20):** Active branch `feature/endrahome-run001`, actual source `Projects/EndraHome/run029/`; source/test correction commit `caa6230055693829061aa3488974403c95deecf2`. **GitHub Actions Run035 workflow #43 ID 44835908930:** host **PASS**, native Ndless **PASS**, report **PASS**. Downloaded artifact `EndraHome_Run035_Desktop` ID **14676869580**, verified actual `EndraHome_Run035_Desktop.tns`: **205,862 bytes**, initial **Zehn**, SHA-256 **049de8fa1ecfcbe1b5495a15d3ab5feb3471f6860d79291ea44961649f1808e9**, matching packaged SHA256SUMS.txt. Preview artifact **14678820938**, nine real 320×240 host PPM screenshots inspected: original Run031 branded electric-blue **E cube and EndraHome/TI-Nspire header restored without a new top bar**; no bottom instructions bar; old fake `Run031 96%` text scrubbed; baked bottom controls replaced by decorative darkened reflection within the existing framebuffer. Desktop card arrangement unchanged, Files-only if unassigned, E icon consistent. **Run035 physical device test PENDING**. **Run034 physical UI display CONFIRMED** from user's four photos (desktop grid, manager, Appearance, Files menu) but file mutations, native OS standby and Home > 5 > 9 not verified. No LCD init, input handling, config, filesystem, firmware, power register or Ndless hook changes. Preserve Run034 as rollback. Working transfer: `clear` then `push /storage/emulated/0/Download/EndraHome_Run035_Desktop.tns /upload`; `/upload` required. Details in `Projects/EndraHome/RUN035_NOTES.md`. This is the most recent state; earlier "latest Run034" and historical one-argument push notes are superseded.

# EndraHome Run035 — native wallpaper desktop, no drawn header or footer (2026-11-20)

## Hardware feedback and baseline
Owner's four Run034 physical TI-Nspire CX II CAS photos confirm that the Run034 desktop with configured GBA / ZIP / CXplorer / Files / Text, app manager, Appearance settings and eight-option Endra Files popup are displaying on hardware. Owner says overall improved, but the **drawn desktop title/header still covers the beautiful original Run031 artwork logo/header**, and the persistent bottom shortcut legend wastes screen space. These are UI display successes only, not proof every file operation or hardware power-off was tested. Preserve Run034 and earlier working builds as rollback.

## Run035 approved single coherent graphics patch
- Use the entire original, hardware-proven 320×240 Run031 RGB565 artwork as native full-screen desktop background, showing its original shaded **E cube + EndraHome / TI-Nspire Edition** logo area. Do not draw any additional top bar, duplicate EndraHome lettering, fake top-right title, or version label.
- Original artwork embeds an outdated `Run031 | 96%` battery readout. Without lying about battery status, scrub ONLY that top-right caption using nearby neutral pixels, blending on the left boundary. Preserve rest of actual original header.
- Original artwork embeds a key legend along bottom y~210-239. To honor no persistent footer, replace baked-in control strip *in existing framebuffer* using a darkening reflection of artwork rows 183..203. No extra large image buffer, filesystem changes or firmware writes; keys continue to function as before.
- Preserve uniform dark E app cards, cyan selection borders, Files as only default unassigned slot, user-configured app shortcuts, and all EndraFiles/manager/theme screens.
- Native rendering and ESC/Enter/Menu input handlers unchanged. Do NOT change `platform.c`, `main.c`, `files.c`, `config_save.c`, `registry.c`, LCD/timer cleanup or Ndless hooks. Compute at most one native framebuffer redraw per normal UI event.
- CI should run host tests, all nine real 320×240 screenshot previews, native Ndless compile, executable Zehn check and SHA256. Inspect preview visuals before user transfer; preserve old binaries.

## Outstanding features — NOT implemented
- `Ctrl+Home/On` must invoke genuine normal TI OS standby rather than only exiting EndraHome. Hardware safety forbids guessed power-register hacks or fake blank LCD; research approved OS-specific handoff separately.
- Native TI Home > 5 (Settings) > 9 (EndraHome) like nKits theme editor. It requires OS-specific integration. Never install a resident hook, modify Ndless config, or patch firmware without independent safety validation and explicit opt-in.

## Transfer
After verifying artifact, provide direct `EndraHome_Run035_Desktop.tns` and exact Termux command:
```sh
clear
push /storage/emulated/0/Download/EndraHome_Run035_Desktop.tns /upload
```
Trailing `/upload` REQUIRED; USB bus node not hardcoded.

## Status at code commit
Run035 source, CI, artifact and preview verification completed as described at the top of this file; physical calculator test is pending.
