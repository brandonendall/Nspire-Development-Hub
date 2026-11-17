# Endra engineering contract (user preferences + repo safety)

1. **Read the memory hub** first when told "Load Endra Memory"; fetch current code, README, PR, Actions and release metadata. These memory notes are a dated snapshot.
2. **Work in owner's repos** (`brandonendall`). Other forks may inform recovery but are not the automatic release destination.
3. **Never infer latest source from `main`**: important Nspire apps are developed in feature branches and **draft PR chains**. Inspect base/head SHAs and upstream feature branches before altering files.
4. **No automatic merges to main**; create focused branches/draft PRs and report commit and workflow run IDs, as requested by the user. Avoid force-push.
5. **Preserve working versions** in filenames and Git history. Before modifying a feature, explicitly compare with the last device-confirmed behavior. No reimplementing old bugs or inadvertently reverting newer features.
6. **Hardware first**: preserve calculator resources; avoid risky firmware/hooks/NAND/OS operations, avoid long hidden waits or background loops, use bounded reads and avoid hard resets.
7. **Report test honesty** in three separate columns: host/CI result, artifact output, actual device result. Build success **does not** mean tested on user's calculator or Android device.
8. **Quality/security**: guard path traversal, overwrite, recursive depth, error cleanup, memory use, ZIP CRC, file handles, symlink behavior and directory size; reserve destructive operations for explicit confirmation.
9. **Controls/UX**: Nspire Enter opens; Menu opens actions; Left parent where applicable. Keep compact colorful icons, gray/cyan colorway for file tools, readable text, no duplicate toolbar, no clipped final row, human-readable KB/MB. Don't make a new visual mockup when user says to build.
10. **EndraHome**: seven customizable launcher slots, safe display/keyboard lifecycle, permanent `EndraHome_Apps.cfg.tns`, maintain app data and selection. Do not write launcher settings automatically, nor fake battery/CPU/RAM measurements.
11. **Android**: device discovery with user USB permission; safe app/learner data during upgrade; maintain package identity/signature and accepted visual design; no permission or security bypasses.
12. **Deliverables**: numbered TNS/APK filenames and clean direct links; verify exact paths/files before linking. The owner typically operates from an Android phone, not a PC.
13. **Termux**: begin code blocks with `clear`; use `push "/absolute/path/to/file.tns"` as the preferred Nspire upload. Don't substitute direct `nlink upload` unless explicitly debugging.
14. **Release to community** only after ownership/licensing, working source builds, artifact checksums and safe hardware tests are complete; preserve third-party notices/attribution (including GPL obligations where applicable).
15. **Memory security**: don't store passwords, tokens, private signing keys, account data, private logs, ROMs, personal medical/legal details or other sensitive information in public memory. Keep docs facts limited to development workflow/device capabilities and non-sensitive preferences.
16. **After milestones**: update project-local README/STATUS, `Endra-Memory/BUILD_HISTORY.md`, `KNOWN_BUGS.md`, and `MASTER_MEMORY.md` as applicable; commit changes with source. The system does not autonomously monitor repos or ingest every chat.
