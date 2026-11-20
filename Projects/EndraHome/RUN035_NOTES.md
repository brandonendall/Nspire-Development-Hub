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
Run035 source edits planned, not yet CI passed, artifact verified or hardware tested. Update after CI.
