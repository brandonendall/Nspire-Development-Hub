# EndraHome Run014 — Assignment SaveFix (hardware test required)

Run013 successfully displayed the file picker, but the user submitted a photo of the **EndraZIP** Info page showing **SAVE FAILED — CONFIG UNCHANGED** after selecting a program. The cause is NOT yet proven. A strong candidate in Run013 is its direct `rename(temp,existing_cfg)` operation, which may fail on this Ndless filesystem when the destination exists. The original config remained unchanged (as the UI reported).

## Fix and diagnostics

- Preserves the physically verified Run012 one-press launch + Run011 return Home + Run010 fast no-refresh exit.
- The app picker remains exactly the same; select icon → Menu → **ASSIGN APP** → browse Documents → select any safe .tns.
- Uses short staging filename `EHNew.cfg.tns`, shorter than Run013's `EndraHome_Apps.cfg.tns.pending.tns`.
- Writes and reads back a complete staged config with the selected shortcut and verifies that ALL other labels/paths match the current registry before committing.
- If a permanent config already exists, safely **renames** the old config to `EHOld.cfg.tns` first, then moves the validated staged config to the permanent `EndraHome_Apps.cfg.tns`. The original remains intact under its backup filename until the new copy has been read and verified. Successful saves retire the temporary backup. If replacing fails, the program attempts to restore the original.
- Startup recovery restores `EHOld.cfg.tns` to the permanent name **only if** the permanent file is missing. A hard failure preserves files for manual recovery rather than deleting them.
- **Failure now shows actual stage and OS error number** on the Info screen (examples `CREATE TEMP FAILED E...`, `MOVE OLD FAILED E...`, `INSTALL FAILED E...`, `RESTORE OLD FAILED E...`). This lets us distinguish permissions/filename failures from rename behavior.
- The permanent config name remains `EndraHome_Apps.cfg.tns`, unnumbered. All files on the calculator remain independently launchable. Do not replace your configured file with the bundled blank template.

## Safe physical test

1. **Back up your existing `EndraHome_Apps.cfg.tns`** externally before testing any version that writes to it, and keep working Run012 installed. Run013's failure should not have changed your config; check its current Tools shortcut still works.
2. Upload only `EndraHome_Run014_SaveFix.tns` to the TI-Nspire root (same folder as configured file). Use your verified Termux `push` utility (it discovers the current USB path).
3. Open Run014, test Esc-only fast exit, then open again. Select the previously empty EndraZIP tile → Menu → Assign App → select the already-installed `EndraZIP_006.tns`. The app should show **SHORTCUT SAVED** if successful.
4. Esc to Home → Enter on EndraZIP should launch the assigned app immediately, and closing it should return to Home. Then exit and relaunch EndraHome: assignment must persist.
5. If saving still fails, **take a photo showing the new error stage and E-number. Do not delete/rename any file manually until we interpret the result.** If `EHOld.cfg.tns` is present but the permanent file is missing, stop: Run014 will attempt automatic restore on next start, but a manual rescue from backup may be necessary.

Host tests validate saving and reloading, preservation of all other shortcuts/comments, abort on stale backup/staged files, and crash recovery. As with all calculator changes, native CI success does NOT confirm actual device behavior. No OS hooks, SDL, browser file deletion, or arbitrary writes to other files.
