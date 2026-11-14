# EndraHome Run005: concept-matching redesign (experimental)

User-supplied photos on 2026-11-14 compared a **working Run004 physical calculator screen** with the original visual concept: centered cube brand logo, dark charcoal palette, seven colorful apps, neon selection and simplified bottom control bar.

This version recreates that composition at the TI-Nspire's **actual 320x240** resolution. The concept image has more fine typography/pixel detail than can be shown at native size; gradients, shape detail and letters are approximations, not the concept screenshot copied or embedded.

## Run005 change scope
- **Unmodified and physically proven** Run003 `../gba-ui/platform.c`: native portrait LCD, three screen buffers, key/touchpad polling.
- New purely software `render.c`: 3D cube icon, center title, subtitle, seven gradient app icons, compact dock captions, highlight and matching footer.
- New tiny `logic7.c`: 7 selectable dock entries rather than 4, preserving all other previous page semantics.
- No SDL, no new hardware calls, no file operations, no startup hooks, no CPU clock modifications, **no external Ndless app launch**. Enter opens information only. Battery is a *decorative unmeasured icon*.

## Build and test
- Host logic: `make -C Projects/EndraHome/run005 test`
- Host previews: `make -C Projects/EndraHome/run005 preview` — emits four PPM screenshots in build/.
- Ndless: `bash toolchain/build.sh Projects/EndraHome/run005`
- CI: `.github/workflows/endrahome-preview005.yml` — expect `EndraHome_Run005_PreviewMatch.tns` plus host preview artifact.

## Physical test
Keep Run003 and Run004. Install this separate experimental .tns to Documents. First try launch + immediate Esc exit. If that succeeds, launch again, move selection through all seven apps, Menu/Help, Enter/Detail, Esc/Back, and exit. If a freeze occurs, stop and report the last key/screen. Do not test 'launching GBA' from here: that capability is deliberately absent.

App names in Run005 are placeholders until independently implemented. Do not imply EndraCmd, Images, Favorites or other shortcut modules are already built.

Source subject to applicable GPL-2.0-or-later terms because the native platform is adapted from gpSP; the 8x8 font is public domain. No user photographs or proprietary logos are embedded.
