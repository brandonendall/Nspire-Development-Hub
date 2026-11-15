# EndraHome Run016 — Physical QWERTY Rename

User physically tested Run015 and reported "Everything worked great" but requested removing the awkward 42-cell alphabet palette: TI-Nspire CX II CAS already has a full **QWERTY keyboard**.

## New rename controls

- **Home → select shortcut → Menu → Rename Shortcut → Enter.**
- Dark text-entry field replaces the on-screen alphabet grid entirely.
- **Type the new name directly using physical A–Z, 0–9, Space and common punctuation keys.** Shift modifies A–Z to uppercase; unshifted letters are lowercase, like the TI-Nspire's physical keyboard.
- **Del** removes the last character; **Enter saves**; **Esc cancels**. Max 15 ASCII printable characters; the '|' delimiter is forbidden because the config uses it between label and .tns path. No auto-repeat for letter presses to avoid accidental duplication; release key between repeating the same letter.
- On save, use the hardware-tested Run014 backup, staged write, verification and recovery mechanism to preserve every other shortcut in the permanent `EndraHome_Apps.cfg.tns`. No new config filename.
- All other Run015 features preserved: **single-press Home launch**, auto-return after child, fast no-`refresh_osscr` shutdown, Assign/Change file picker, remove shortcut without deleting the actual file, app info, settings.
- Keyboard implementation reuses mappings proven in `Projects/UltimateCommander/src/nx_platform.c` and uses Ndless `isKeyPressed(KEY_NSPIRE_*)`; kept isolated in `text_input.c` so proven display hardware code is unchanged. Keys are edge-detected each frame, with no OS text dialog, resident hook, SDL, nClock popup, or firmware changes.

## First physical test

Back up your configured `EndraHome_Apps.cfg.tns`; keep working Run015. Upload ONLY `EndraHome_Run016_QwertyRename.tns`. Open Home and press Esc (fast exit). Reopen and test Home Tools immediate launch and child automatic return. Highlight a spare shortcut; Menu → Rename Shortcut → Enter. **Type the intended name on the QWERTY keypad**; test Del, type again, press Enter to save. Return Home, confirm name survives restart and its app path still launches. Try Shift + letter for capitalization; test digits and spaces. Stop if no letters are registered, if keys double type, if UI freezes, or if config fails to save. Run015 remains known-working rollback.

Clock design request for a future run: integrate time directly in EndraHome's dark header (not nClock's white popup). NOT implemented in Run016.
