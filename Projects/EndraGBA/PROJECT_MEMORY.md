# EndraGBA project memory — Run012

## Current work — 2026-11-21 ET

Owner: Brandon / Endall. Active branch:
`feature/endragba-run012-loading-nightmode`, based on Run011 branch head
`67721cb257e5ea201dfa99a7eeca29c1b9c7c885`.

Latest owner handoff confirms Run011 looks good on the calculator except its
blue loading screen. Run012 corrects the separate loading/status/failure palette
to black/gray; the full Run011 patch chain and all non-UI behavior are retained.
Patch asserts only six loading colors and GUI/build version metadata change.
Actual C host rendering and sanitizer checks pass locally for nine frames,
including 0/25/100% progress, status, failure, stopped and long-name cases.
Native CI/artifact identity will be recorded after the build completes.
Run012 physical testing is pending. Next: compile, retrieve and verify TNS,
inspect CI previews, then owner checks GBZ, UI, fast exit, saves and RTC.

Primary rollback: Run011 (480480 bytes, checksum verified from artifact
14690386825). Keep Run010/Run009 too. See [Run012 notes](RUN012_NOTES.md)
for exact rollback SHA-256, source, build and mandatory `/upload` commands.
No firmware/hooks/resets/config migration. Download filename:
`EndraGBA_Run012_LoadingNightMode.tns`.

## Historical record

Previous active branch: `feature/endragba-run011-nightmode`. Do not use stale main's Run008 as latest.
- Run009 FastExit: owner reported immediate launch and exit on physical CX II CAS. Removed only exit-time refresh_osscr(); preserve it and LCD/timer restoration. Actions 43642224771; PR #17.
- Run010 BrowserLeft: source bc4b3d9686cf65a3dccf96287293f585c0847e58; CI 43809944137 passed; PR #36. Owner's photos show folders on left, GBZ game selected, Shining Soul II loaded and emulator menu displayed. Separate save/RTC and timed exit tests not yet recorded. Keep Run010 as UI rollback, Run009 as fast-exit rollback.
- Run011 NightMode: owner approved concept preview, then requested build. Native GUI palette only: black backgrounds, charcoal header areas, gray lettering, mid-gray selected rows with light lettering. No gameplay filter or brightness change. Source f899c4ff9bddb6d5eac70be49570d18b45f89da2; PR #45; CI run 44862054240, patch assertions passed; native build status to be recorded after completion. Physical test pending.
- Preserve ROM-local saves/states, GBZ paging, RTC, UltraGBA_Data/UltraGBA_Tests, native display/timer restore, keypad behavior, and no automatic Ndless association changes.
- Android eGBA directory explicitly confirmed by owner: /storage/emulated/0/lab/Endra/eGBA/
- Required Termux command:
```sh
clear
push /storage/emulated/0/lab/Endra/eGBA/EndraGBA_Run011_NightMode.tns /upload
```
Next: verify CI and checksum, provide direct TNS, test both menus on hardware, launch GBZ and verify exit/save/RTC without disturbing working data. Follow main FANTASMA_ENDRA_WORKFLOW.md.

## Run011 verified delivery — 2026-11-21
CI run 44862054240 completed SUCCESS at source f899c4ff9bddb6d5eac70be49570d18b45f89da2. Artifact 14690386825 retrieved and extracted. EndraGBA_Run011_NightMode.tns: 480480 bytes, SHA-256 33ddfd84b70b917540df199a390bf3a68d28b4c6e9d484d004f78bb9cf7883bd, matches bundled SHA256SUMS.txt. Physical test pending. Run010 rollback. No claim of on-device verification.
