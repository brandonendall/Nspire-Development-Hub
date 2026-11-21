# EndraHome Run039

## Verified build

- Branch `feature/endrahome-run001`, source `Projects/EndraHome/run029/`, commit `e3020175a75b87276a553e1985ad3963986a2987`.
- [Actions #49](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44872738465) (run `44872738465`) passed host, native Ndless and status report.
- Native artifact **14695005870** has an independently verified archive digest. `EndraHome_Run039_Desktop.tns` is **441,062 bytes**, starts with Zehn, and SHA-256 `bcf5a50a335b5ef16fd76189293e2c848f6c6285bb81ac48be612552f6d2f3c9` matches the packaged manifest.
- Preview artifact **14694935436** passed archive digest verification. Its 53 real 320×240 host renders match the local renders byte for byte. Inspected the eight interface colors, appearance and contrasting Files views with glyphs, menu and launcher examples.
- Host tests cover 128 app layouts, Files-only startup, assignment and rename transactions, launcher return, browser navigation, image decoding, and temporary-fixture file operations including 33 MiB streaming. Run039 exercises every legacy interface ID, selector wraparound, independent colors, save/reload/cancel/defaults, interrupted saves, all five wallpapers and ten launcher icons, case-insensitive compound suffixes, and framebuffer bounds. AddressSanitizer and UndefinedBehaviorSanitizer passed the display tests with leak detection disabled because the host runner cannot run LeakSanitizer under tracing.

## Changes and migration

- Interface choices use stable v4 IDs in this order: Blue, Red, Green, Yellow, Orange, Purple, Black and White. Black has dark artwork/chrome. White has a separate light ramp and light chrome. Text and folder colors keep their original 20 options and values. Local contrast backing preserves the chosen text and folder colors.
- Legacy Ocean/Blue/Cyan/Teal map to Blue. Charcoal/Slate/Black map to Black. Gray/Silver/White map to White. Red/Dark Red, Green/Lime, Yellow/Gold, Purple/Pink and Orange/Brown map to their retained base colors.
- Save writes `EndraHome_Appearance4.cfg.tns` after confirmation. Reads fall back through Run038 v3, legacy v1/v2 and the original app-config theme when needed. Run038 appearance remains available to the rollback. Wallpaper, icon, folder, text and app assignments retain their prior interpretation. The original packed artwork bytes remain unchanged.
- Files preserves the folder design and small header. Ten file kinds have separate 14×13 glyphs with compound-suffix classification. Plain `.tns` uses a generic TI document glyph. The E program glyph is reserved for explicit ELF/Zehn extensions as filename hints. Listing performs no content probes and icon kind does not change open, launch or format support.

## Hardware and rollback

Run038 owner photographs confirm the UI rendered on TI-Nspire CX II CAS 6.4.0.74/Ndless. They show Appearance with Reactor and Dragon Core, a desktop with four configured shortcuts, the compact Files header/list/footer, and the native read-only probe reporting a stock pointer and an uninstalled shortcut. That evidence supersedes the older blanket pending status for those observed screens. It does not verify all app launches, persistence, file mutations, or standby. Run039 physical testing is pending.

Keep `EndraHome_Run038_Desktop.tns` (440,638 bytes, SHA-256 `dba8c3cfd218b204d20b8788e0eb637e2e6a2afc5b57f19bec5e82b7ba16e9fd`, Actions artifact **14691888846**) as the primary rollback. Keep Run037 as the older backup. Run039 preserves the existing safe exit and read-only TI Settings probe. Genuine standby and TI Settings registration require separate evidence and review.

```sh
clear
push /storage/emulated/0/Download/EndraHome_Run039_Desktop.tns /upload
```
