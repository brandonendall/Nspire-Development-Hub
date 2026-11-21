# EndraHome Run038

## Build evidence

- Repository `brandonendall/Nspire-Development-Hub`, branch `feature/endrahome-run001`, source `Projects/EndraHome/run029/`.
- Source commit `8bc8d111a577e0783d3290d16e014362d0c455c5`. [Desktop workflow #48](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44867780859), run ID `44867780859`. Host tests, native Ndless build, and status report passed.
- Native artifact **14691888846**. Extracted `EndraHome_Run038_Desktop.tns` has a Zehn header and is **440,638 bytes**. Its SHA-256 `dba8c3cfd218b204d20b8788e0eb637e2e6a2afc5b57f19bec5e82b7ba16e9fd` matches the bundled manifest. Both artifact archives matched the GitHub SHA-256 digests.
- Preview artifact **14692278912**. All **33** 320×240 frames matched the local render files byte for byte. The five compositions were inspected in Ocean and Green, all ten 48×48 icon styles were inspected, and the Files popup and manager were checked in contrasting themes.
- Host tests cover 128 shortcut layout combinations and built-in Files, launcher return, assign and rename transactions, browser navigation, image decode and file operations. Run038 tests cover selector wraparound, independent choices, preview/cancel/save/defaults, v1/v2 fallback, invalid fields, interrupted appearance saves, transparent sprites, and framebuffer bounds.

## Appearance and rollback

Cyber Wave, Neon City, Cyber Lab, Reactor and Tech Corridor use the five approved 320×240 package images. The ten icon styles use masked emblems from the approved concept sheet. The desktop brand header is composed separately. The Interface theme tints wallpaper, icons and UI chrome. Folder and text colors remain independent. The Files heading now uses the same scale as BROWSER. Its pop-up uses a more opaque background to hide underlying file names.

The asset bundle is **447,320 bytes** before Zehn compression: 386,560 wallpaper, 51,200 icon, and 9,560 illustrated header bytes. The three RGB565 palette caches total **1,536 bytes**. Art renders directly into the existing framebuffer on redraw. The build adds no full-frame buffer or idle rendering. Device timing and peak memory are unmeasured.

Appearance changes use `EndraHome_Appearance3.cfg.tns`. Run038 reads the existing v1/v2 appearance when the new file is absent. The prior appearance file remains available to Run037, and the shortcut config still uses `EndraHome_Apps.cfg.tns`. Enter on Restore defaults previews the reset across all five settings. Enter again saves.

Preserve Run037 `EndraHome_Run037_Desktop.tns`, 208,327 bytes, SHA-256 `a0a8878e0fd551c36516f22440045f5071a2d566c550ab540882dae814c9faa6`. The work package copy matched [the Run037 Actions artifact](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44863512170), ID **14690592484**, byte for byte. Retain Run036 as an older fallback.

## Calculator status and OS research

**Run038 has not been tested on a physical calculator.** The owner's Run037 photos show green desktop and metallic E icons, browser and compatibility UI, a stock Settings menu pointer, and the native menu displaying items 1–8. This establishes the observed menu state and UI rendering. File mutations, OS standby and hook coexistence are still unverified.

The [TI handheld eGuide](https://education.ti.com/html/webhelp/EG_TINspire/EN/content/m_hh_gs/gsc_turning_on_hh_first_time.HTML) documents Ctrl+On as the OS power-off chord. The [Ndless public header](https://github.com/ndless-nspire/Ndless/blob/master/ndless-sdk/include/libndls.h) exposes key polling, idle and timed sleep services. No documented native standby entry point was identified there. Run038 preserves the existing clean launcher exit path, including the current Ctrl+Home exit behavior. The [native Settings research](NATIVE_SETTINGS_SHORTCUT_RESEARCH.md) records the remaining coexistence, callback and standby constraints.

```sh
clear
push /storage/emulated/0/Download/EndraHome_Run038_Desktop.tns /upload
```
