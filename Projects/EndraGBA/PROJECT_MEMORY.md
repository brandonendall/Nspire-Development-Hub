# EndraGBA project memory — 2026-11-20
Active branch: `feature/endragba-run011-nightmode`. Do not use stale main's Run008 as latest.
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
