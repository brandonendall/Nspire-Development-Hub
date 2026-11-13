# Handoff import verification — November 13, 2026

## Archive inspected
The uploaded `UltraGBA_COMPLETE_AI_REBUILD_HANDOFF (1).zip` contains **397 archived files**, approximately 6.6 MB uncompressed, including multiple source snapshots, Git history, six historical .tns binaries, pinned gpSP source, patches, workflows, docs, and checksum manifests.

## Uploaded to the private hub
- Run007 branch snapshot: **25 source/doc/workflow files** copied from `OffCamera-Civilman/Nspire-Development-Hub`.
- Pinned upstream gpSP reference: **185 text files** from commit `2ddeb23b565cab84e20278bb73ed76f3d91b25f5`, with one open replacement BIOS binary not copied by this text API transfer.
- Shared Ndless Docker build wrapper and dedicated smoke workflow.
- Project notes and isolated placeholders for EndraGBA, UltraZip and Ultimate Commander.
- **Original complete nine-branch Git commit history** preserved in `handoff/history/UltraGBA-all-repo-branches.bundle`, verified by successful GitHub Action 43347375606. This is a portable bundle, not nine active branches and does not replace the private hub's `main`.

## Verified CI
- Shared SDK smoke .tns build succeeded: run `43346931188`, artifact `NspireSmoke_Run001`.
- Run007 emulator .tns build succeeded: run `43346931207`, artifact `UltraGBA_Run007_BrowserClean`.
- Both used Ndless Docker image `bensuperpc/ndless:latest` as resolved at build time; actual image digest recorded in build logs.
- Actual calculator hardware testing remains **pending**.

## Still not transferred to the new repository
- Six historical built .tns files from the uploaded ZIP; they remain in the original handoff, though a new Run007 build artifact is available.
- All 397 ZIP members as literal GitHub files; repeated historical snapshots were not duplicated on main. Original commit ancestry is preserved in the Git bundle.
- Actual source for UltraZip and Ultimate Commander; not supplied in this archive.
- A standalone binary Ndless SDK; the external Docker compiler is the shared build environment.

No copyrighted ROMs, official BIOS, personal save files or secrets were uploaded.
