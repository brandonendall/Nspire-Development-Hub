# Run015 App Manager — hardware test pending

User physically confirmed Run014 auto-assignment works. Run015 retains the tested quick launch, return-home, fast no-refresh shutdown and safe config backup/recovery, but adds these **on-device** controls:

HOME: Arrows choose shortcut, Enter launches a configured app immediately with no confirmations, Menu opens manager, Esc exits.
MANAGER: Up/Down selects Assign/Change App, Rename Shortcut, Remove Shortcut, App Information, Settings. Enter selects, Esc/Menu returns Home.
ASSIGN/CHANGE: Existing tested Run014 picker navigates /documents and saves to the permanent unnumbered EndraHome_Apps.cfg.tns. Backup the file before testing.
RENAME: Native dark 7-column character palette A-Z / 0-9 / punctuation, up to 15 characters. Enter adds a glyph, grid DEL deletes previous, Menu or grid SAVE saves the new name, Esc cancels. This is an on-screen palette (not physical keyboard typing), so no OS white dialog.
REMOVE: Enter on Remove shows ONE confirmation (only for the config edit, not for launching). Enter confirms unassignment, Esc cancels. NEVER deletes the actual .tns app. Rename and Remove use Run014 transactional swap/recovery; other slots are preserved.
INFO: displays label and linked file. SETTINGS: reports launcher features and **future** clock HUD design.

nClock idea (future): user wants real time in EndraHome's dark header, **not** the white popup. No nClock hooks or RTC reads were included in Run015 until a trustworthy supported time source is verified on device. Never claim it is active yet.

First safe test: back up EndraHome_Apps.cfg.tns; keep working Run014. Upload Run015 .tns ONLY. Open and press Esc, then open and test Tools one-press launch/return. Test Info and Settings (non-writing). Rename a nonessential slot and check it persists after reopening. Try Remove only on a spare shortcut after external config backup; verify assigned app file still exists, and reassign via picker. Stop on any freeze, corrupted display or config save error. Do not modify Ndless OS/hook files.
