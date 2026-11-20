# EndraHome Run034 — visual coherence and hardware-safe UI cleanup
Updated: 2026-11-20

## Source of authority and user feedback
User's TI-Nspire CX II CAS photographs physically demonstrate Run033 working enough to display five configured desktop apps, a native Endra Files pop-up menu, and Appearance settings preview. Run033 added the Run031 image, but user still reports unfinished visual design: giant E cube and awkward old two-line header conflict with the artwork; a small gold folder icon stuck on the Files E badge must be removed; oversized high-contrast bars and uneven UI need improvement. These are current verified appearance observations, not proof all file actions were tested. No user request to discard valid configured apps; Files is still the only *permanent default* and empty Add App tiles are excluded.

## Run034 cohesive engineering changes
- Replace the giant top-left 40x40 E cube + generic header with one compact, font-matched electric-blue wordmark (no logo badge) and tagline on a slim dark glass strip; keep the original physically approved Run031 320x240 RGB565 background.
- Do not render the separate little yellow folder over Files. Keep all configured slots visually consistent and preserve assignment/persistence; a single Files tile when no assignments exist.
- Give every tile a uniform dark face and thin cyan border on selection; no large white selection backgrounds. Increase inter-row spacing and preserve readable app names.
- Port the same 5x7 bitmap typography style used in `Projects/UltraZip/src/ultrazip.c` for compact headers, tile names, floating actions and footer. Do NOT port UltraZip's LCD init/blit/input to EndraHome: its working native gpSP-derived platform owns safe screen restoration.
- File browser gets a consistent 320x240 title, path band, compact name/size list, dark selection line, cyan rail, readable file-type icons, matching footer. Popup is compact and preserves all eight pre-existing options with proper affordance; no command is claimed functional unless real implementation exists.
- Manager, Appearance, rename dialogs and file details use consistent dark panels and crisp headings. Retain existing adjustable theme/folder/text colors and explicit saves. Add visible end caret to file rename input; this is NOT full arbitrary-position editing.
- Implement only UI changes in `Projects/EndraHome/run029/` and build as **EndraHome_Run034_Desktop.tns**. Core launch, session, storage, platform, file ops and config files are unchanged.
- CI must pass host tests and native compiler; artifact must be retrieved and checksummed, host preview inspected before physical test. Preserve Run028 working rollback, Run031 RGB565 graphics, Run032 and Run033 binaries.

## Outstanding native integration issues — NOT FIXED
The owner's intended **Ctrl+Home / Ctrl+On** behavior is calculator normal standby/power off, NOT exit EndraHome. Main.c currently calls `ui.page.should_exit`; Esc is safe application exit. **TI Home > 5 > 9** should show EndraHome alongside nKits/theme-editor native Settings, not merely an in-app settings panel. Both require OS 6.4.0.74-specific investigation before safe opt-in implementation. Never install startup hooks, alter Ndless settings, power registers, or firmware, or claim either behavior fixed just because the visual build passes.

## Status
Planned source change; CI and physical test pending until separately recorded. Use `clear` and `push /storage/emulated/0/Download/<actual Run034 filename>.tns /upload` with required trailing `/upload`.
