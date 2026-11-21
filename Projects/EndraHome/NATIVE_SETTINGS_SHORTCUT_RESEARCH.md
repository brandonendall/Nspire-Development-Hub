# EndraHome — native TI Home > 5 Settings shortcut research (2026-11-20)

## User requirement
From the native TI-Nspire CX II CAS home screen (OS 6.4.0.74 / Ndless), press **Home > 5 (Settings)** and select the next available item **EndraHome** (9 if unoccupied, otherwise the next available supported selector). Open existing EndraHome launcher directly, preserving the official TI OS menus. User demonstrates that the existing **nKits ThemeEditor** already inserts a "Theme Editor" entry into the same native Settings menu. This is NOT EndraHome's in-app "Menu > Appearance".

## Source inspected, not an installation
1. `https://github.com/Ziyang-Bai/nKits`, `ThemeEditor/src/platform.c`: `theme_platform_menu()` and `menu_find()` find a verified OS 5-word Settings record table and register a **resident callback**. It requires a specific pristine Settings menu signature (commands {1,2,4,5,6,7,8,9}), validates OS fingerprints, then swaps a live table pointer while using `resident_lock()`; it can undo its own change only when the expected table pointer is still present. On CX II, uses the reusable factory "Build TP Menus" UTF-16 string slot for caption. It is NOT a general plugin registry. Inserting a second record without coordinating may break removal or overwrite the first program's resident table.
2. `https://github.com/tornadus/nTheme`, `src/menu.c` and `src/hooks.c`: `menu_install()` also copies a 5-word menu and replaces a literal pointer. In `CX II CAS, OS 6.4.0.74` lookup the OS ID is 49, fingerprint address 0x10000020 value 0x1042aee0, menu_literal 0x10a41a0c and stock menu_base 0x10c44fc4. Its main.c rejects unexpected existing resident OS patch; this is evidence that two independent installers may conflict.
3. Ndless SDK supports `nl_exec()` to launch a native executable from its file path, but an OS-menu callback must be resident and coordinate display/handoff. Native menu cannot be registered simply by uploading an extra `.tns`.

## Safety decision / next verification
**NOT IMPLEMENTED, NOT USER-TESTED.** There is no verified supported coexistence method for EndraHome plus the owner's already-installed nKits ThemeEditor (and possible nTheme). Avoid writing OS function pointers, menu literal, hooks, ndless/startup or firmware in a main EndraHome launcher build.

A future **separate opt-in** module must:
- Identify OS ID, hardware subtype and fingerprint read-only; return with no writes on mismatch.
- Discover actual current menu table pointer and all existing menu handlers on device in a bounded, read-only scan; establish if ThemeEditor/nTheme is already resident.
- Validate the existing table shape, copied-table lifetime, callback ABI and number-to-key mapping, especially if entry 9 is already occupied.
- Demonstrate a cooperative chain or single-owner menu extension where both theme editor and EndraHome remain removable and callback memory stays resident; no blindly replacing one literal pointer.
- Host/emulator tests first for cold boot, native menu display, pressing selected shortcut, launching, exiting, OS sleep, conflict, rollback; reversible operator-controlled installation only with explicit consent and hardware-safety review.
- If a safely chained, reversible route cannot be proven, do not install it. Continue using normal OS Browse rather than risking device boot/functionality.

Power chord Ctrl+Home genuine OS standby is a separate unresolved requirement. Do not replace it with mere app exit, a blank screen or direct power-register writes. Preserve Run034/Run035 known-good builds.
