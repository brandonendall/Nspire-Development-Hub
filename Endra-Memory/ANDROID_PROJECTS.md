# Endra Android and cross-platform applications
_Source/repo inventory checked 2026-11-17. This covers the actual Android lineup, distinct from the TI-Nspire `EndraZIP` app._

## EndraLink — Android calculator interface
- Repository: https://github.com/brandonendall/EndraLink ; package `com.endralink.app`, Kotlin/Java, Android USB host support.
- Project vision: one hydra-branded mobile application supporting fx-CG50, TI-Nspire (and future devices) with USB connection, file list, transfer, folder actions and diagnostics.
- Repo `main` README documents 0.1.8 with fx-CG50 USB permission, read-only FAT16 browser, diagnostic export and published debug APK. Some newer versions 0.1.9–0.1.25 were discussed/constructed through collaboration/feature builds and are **not independently validated simply by main README**. Check releases/commits/current Actions and real APK before claiming precise latest.
- Owner reported earlier transfers and multi-file handling working in later prototypes, but later TI-Nspire connection experiences included delayed initialization, busy directory errors and missing USB detection. Proposed safer MVP: USB device enumeration -> permission -> identify -> root listing -> disconnect, then expand cautiously.
- USB device discovery **must not hardcode** `/dev/bus/usb/001/002`. Android hardware should use permissioned UsbManager and appropriate transports. Historical TI-Nspire USB VID:PID `0451:E022` is a diagnostic reference, not proof every variant uses it.
- User cares about truthful status, fast start/exit, working file operations and same polished Endra color/branding. The current APK must be verified independently of static source docs.
- Core source `app/src/main/java/com/endralink/app/`; Android Actions `.github/workflows/android.yml`. Some historical work took place in collaborator fork `OffCamera-Civilman/EndraLink`; use owner repo as destination.

## EndraClean — Android user-app cache cleaner
- Repository: https://github.com/brandonendall/EndraClean ; package `com.endra.clean`.
- Main README advertises Run039 consolidated `0.39.3-gold` APK in `downloads/EndraClean.apk` and SHA256SUMS; latest APK can outpace historical checked-in source. **Physical-device signoff pending**.
- Hydra art; navy panels, gold/yellow accent; save user-app scan/profile; Manage Apps; zero-cache filtering, progress overlay, Stop.
- For ordinary user apps only; explicit user selection plus permissions. **Clear cache ONLY**, not Clear data/Storage, never `pm clear`. System apps informational/non-cleanable. Android security/Settings restrictions must be respected; no bypasses.
- Signing/certificate mismatch and package upgrades may risk data. Avoid casual uninstall advice.

## EndraSpell — learning/word practice
- Repository: https://github.com/brandonendall/EndraSpell ; package `com.endra.spell.test` for signed test updates.
- Main README: Build029 older approved public download; Build034 Checkpoint A/B source baseline with 48 tests and approved worksheet scan/crop/read-review-save-practice flow; **signed Build041** linked as visual correction test, hardware acceptance pending. Public main source may not include the final Build041 source. Do not claim that signed Build041 key is stored in repo.
- Must protect learner data: stable learner IDs, archived weeks, Heart Words, recordings, backup/recovery; never migrate by uninstalling or overwriting with a differently signed debug APK.
- Target phones/tablets, visual accuracy, child-friendly learning and accessible interactions.

## EndraCanasta — Flutter social card game
- Repository: https://github.com/brandonendall/EndraCanasta ; Flutter/Dart, cross-platform Android/iOS target.
- Founder credit in README: **Created by Kat.** Keep project authorship.
- Prototype 0.1: learn/practice, solo, casual/competitive, friends/private tables, UI customization, four-player interactive table. Production multiplayer needs server-authoritative hidden-information/fair-play model; simulated network players acceptable for initial prototype. Not a production multiplayer release.
- No open-source license has been granted; preserve rights/notices.

## EndraMath — educational math app
- Repository: https://github.com/brandonendall/EndraMath ; currently project vision/planning, **no tested APK**.
- Curriculum from early number to college math; learn -> guided practice -> independent practice -> quiz -> review; placement supportive and skill-based, not grade labels; local progress, independent learner profiles, optional dragons Solaris/Aurora.
- Approved visual theme: warm orange/amber/gold magical academy and dragon. Do not replace with another theme without approval.
- Preserve all learning content outside paid cosmetics and avoid timed/shaming mechanics.

## EndraTermux — Android utility environment
- Repository: https://github.com/brandonendall/EndraTermux ; portable `push` shell utility plus ARM64 Rust `nlink` and source.
- **Run004** archive and source tracked. The `README.md` is stale relative to repository tree about binary/source presence; reconcile before publicizing a fresh-install guarantee.
- The user wants eventually to share builds with the calculator community. Before public release verify binary provenance, source completeness, licenses, release downloads, signing/checksums, installation instructions, and hardware compatibility.

## Cross-platform / brand rule
`EndraZIP` is currently the **Ndless TI-Nspire** utility in Nspire-Development-Hub; it is not presently evidenced as a separate Android ZIP APK. If the user speaks the name as "Android zip" or "Andrew Zip", inspect conversation context before concluding Android package exists. Check the actual repos for any future separately named Android ZIP application.
