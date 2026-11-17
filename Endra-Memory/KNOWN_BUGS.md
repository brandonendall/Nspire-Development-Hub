# Known issues, regressions and device acceptance gates

## Nspire
- **EndraZIP:** Run005 reported working. Run007 aimed to remove a duplicated lower toolbar and make the last file-row fully scroll-visible. CI success recorded; check on device. Regression test folder ZIP including empty/subfolders and re-extraction, original non-overwrite behavior, ESC/Menu/Enter interactions, archive name `*.zip.tns`.
- **EndraHome:** Runs001 and 002 completely froze TI-Nspire; no reuse. Early long UI captions collided. About 10-second launch/close latency reported on Runs008–009; Run010 quick-launch/exit confirmed by user. Child-app return, image viewer and full generality still require version-specific tests. Do not modify `EndraHome_Apps.cfg.tns` without authorization.
- **EndraCommander:** prior root and text/CFG preview actions fully froze calculator, sometimes hard reset required; prefer isolated SDL restart, bounded file reads, careful Esc/OS restoration. Don't assume a green host build proves safe.
- **EndraGBA:** formerly GBZ compressed 32MB ROM could hit memory/cache limits while smaller ROMs worked; earlier wild-battle crash and direct-ROM-click association regression. Preserve ROM-local saves, RTC and working FastExit; Run010 reworks ugly directory pane layout (folders-left/games-right).
- **EndraDiagnostics:** CPU/RAM gauges had unreadable/dark needles and overlaid 100% labels. Run024 visual improvement pending on-device. Don't label guessed utilization as live telemetry.

## Android/Termux
- **EndraLink:** Android USB permission/detection, 10–11s initial connection, busy directory reads, device selection; do not tie USB connection to a single physical bus path. Validate device enumeration and full disconnect first.
- **EndraTermux:** `push` should auto-detect connected TI-Nspire using Termux:API; repo README is stale on whether NLink source/binary has been imported. Verify actual git tree, installer and checksum before community setup docs.
- **EndraClean:** overlay behavior may be restricted over Settings on some Android/OEM versions; user permission required, system-app cleaning disallowed, Clear cache only. Run039 source-vs-binary differences and install/signing need verified.
- **EndraSpell:** signed builds may not share source/signature with debug APK. Preserve profiles, attempts, recordings and worksheet state, never suggest uninstall just to try a build. Build041 has pending tablet visual checks.
- **PythonUltra:** ZIP truncation/compression hang, editor keyboard/clipboard/long text, timestamps, dark mode, runtime help; roadmap specifies which feature/CG50 run was physically verified.

## Bug triage requirements
For every report log: app, exact Run/APK, calculator OS or Android version, reproduction keys, expected vs actual, whether freeze required reset, code branch/commit, screenshots or reports, CI verification, device retest, rollback.
Never store private device logs or contents in public source control.
