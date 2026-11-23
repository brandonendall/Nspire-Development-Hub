# EndraGBA project memory — Run012

## Run012 verified delivery — 2026-11-21 ET

- Source commit: `551687aa3cdb062365299582250f1163ee620981`.
- Draft PR: [#46](https://github.com/brandonendall/Nspire-Development-Hub/pull/46),
  based on `feature/endragba-run011-nightmode`. No source merge to main.
- [GitHub Actions 44874060224](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44874060224): **SUCCESS**.
  Baseline preservation assertions, host ASan/UBSan checks, native pinned-SDK
  compile and packaging all passed.
- Artifact **14694973380**, `EndraGBA_Run012_LoadingNightMode`.
- Executable: **EndraGBA_Run012_LoadingNightMode.tns**, **480198 bytes**, Zehn v1;
  header size matches file size. SOURCE_COMMIT.txt matches the built commit.
- SHA-256: `ec855a1cd498da024cf07a1705901f2492669d72e7e741cf306a72baf58bf9db`.
  Independently computed from extracted TNS and matched to bundled SHA256SUMS.txt.
- Downloaded ZIP SHA-256:
  `f8276f3fb498d68fc83dd06b80d841b7a6d0f5f6f996a2316c85e8078f4a724c`,
  matched to GitHub's artifact digest.
- All nine CI PNGs exactly match checked local renders. 0/25/100%, state status,
  failure and stopped screens visually inspected. They are code-derived host
  previews, not hardware screenshots. Long-name/zero-total bounds also pass.
- Native build retains warnings in unchanged legacy emulator sources; no errors.
  CI completed with normal sanitizer settings (local ptrace-only LeakSanitizer
  limitation did not apply to CI).
- No known new regression from source/host checks. Actual GBZ loading, game/save/RTC
  behavior and fast exit on **Run012 hardware remain PENDING** owner verification.
- Next: owner transfers the exact TNS using `/upload`, tests the normal working
  GBZ and checks black/gray loading, menus, fast exit, existing save/RTC behavior.
  Record findings before declaring calculator success.
- Primary rollback **Run011** remains unchanged: source
  `f899c4ff9bddb6d5eac70be49570d18b45f89da2`, run **44862054240**,
  artifact **14690386825**, 480480 bytes,
  SHA-256 `33ddfd84b70b917540df199a390bf3a68d28b4c6e9d484d004f78bb9cf7883bd`.
  It was retrieved and checksum verified. Retain Run010 and Run009 too.

## Implementation record — 2026-11-21 ET

Owner: Brandon / Endall. Active branch:
`feature/endragba-run012-loading-nightmode`, based on Run011 branch head
`67721cb257e5ea201dfa99a7eeca29c1b9c7c885`.

Latest owner handoff confirms Run011 looks good on the calculator except its
blue loading screen. Run012 corrects the separate loading/status/failure palette
to black/gray; the full Run011 patch chain and all non-UI behavior are retained.
Patch asserts only six loading colors and GUI/build version metadata change.
Actual C host rendering and sanitizer checks pass locally for nine frames,
including 0/25/100% progress, status, failure, stopped and long-name cases.
Native CI and artifact verification have passed as recorded above.
Run012 physical testing is pending; next is the owner check of GBZ, UI, fast
exit, saves and RTC.

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

## Run013 — cheat files
Branch feature/endragba-run013-cheats builds from Run012 (141c929f). Added per-ROM .cht.tns import/reload and ten menu toggles, including a US-only one-shot Chaos Ring. See RUN013.md and test_run013.c. Host RAM-write fixture passed. Rollback Run012; test on CX II CAS with an empty first inventory slot. No hardware test yet.
