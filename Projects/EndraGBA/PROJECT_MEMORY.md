# EndraGBA active development pointer (2026-11-20)

Current source and detailed project memory: [feature/endragba-run011-nightmode](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endragba-run011-nightmode/Projects/EndraGBA/PROJECT_MEMORY.md). Draft PR [#45](https://github.com/brandonendall/Nspire-Development-Hub/pull/45). Main's Run008 README is historical, not latest.

Run009 FastExit was user-confirmed on hardware; Run010 left-folders/right-games UI and loaded Shining Soul II GBZ are shown in user device photos. Preserve both rollback builds. Run011 is the approved unified black/gray display theme, pending device validation. See active project memory for CI and artifact results.

Android build folder: `/storage/emulated/0/lab/Endra/eGBA/`. Always `clear` then `push /storage/emulated/0/lab/Endra/eGBA/ACTUAL_FILENAME.tns /upload`. Preserve GBZ, RTC, ROM-local saves, existing data directories and native LCD restoration. Do not modify OS or Ndless associations.

## Run011 verified delivery — 2026-11-21
CI run 44862054240 completed SUCCESS at source f899c4ff9bddb6d5eac70be49570d18b45f89da2. Artifact 14690386825 retrieved and extracted. EndraGBA_Run011_NightMode.tns: 480480 bytes, SHA-256 33ddfd84b70b917540df199a390bf3a68d28b4c6e9d484d004f78bb9cf7883bd, matches bundled SHA256SUMS.txt. Physical test pending. Run010 rollback. No claim of on-device verification.
