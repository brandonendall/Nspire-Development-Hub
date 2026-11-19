# TI-Nspire / Ndless Endra application lineup
_As observed 2026-11-17. Most current source is on unmerged feature branches. Check PRs, workflows and device evidence before new coding._

## EndraZIP (formerly UltraZIP)
- Repository: `brandonendall/Nspire-Development-Hub`; historical source `Projects/UltraZip/` on main, rebranded source `Projects/EndraZIP/` on feature PRs.
- **Run005**: user confirmed browser and normal ZIP functionality working. Controls: **Enter/Right opens selected directory or previews ZIP**, **Menu opens actions**; Left parent; Esc leave; up/down navigate. Do not resurrect old Enter-to-popup behavior. Palette: gray/charcoal, cyan accents, colored folder/archive/file icons.
- **Run006**: PR [#6](https://github.com/brandonendall/Nspire-Development-Hub/pull/6), branch `fantasma/endrazip-006-recursive-folder-zip`; recursive whole-folder ZIPs including nested, hidden, and empty directories; creates `<folder>.zip.tns` next to source and retains originals; strong Deflate compression; no overwrite. CI passed 88 checks/27 UI states on GitHub Actions run `43494449893`; actual calculator results require user verification.
- **Run007**: PR [#37](https://github.com/brandonendall/Nspire-Development-Hub/pull/37), branch `fantasma/endrazip-run007-ui-fix`, commit `080fea4c13d13114e624fb810386aff6bf959394`. Addresses **duplicate lower toolbar** and last bottom file-row being clipped on scroll. Builds `EndraZIP_007.tns` while preserving Run006 folder ZIP. GitHub Actions run `43812008118` succeeded, artifact `EndraZIP_007`, ID `14201036451`; **physical testing pending in available record**.
- EndraZIP Run007 user local path `/storage/emulated/0/lab/Endra/eZip/EndraZIP_007.tns`, upload via `push`, not direct `nlink upload`.
- Future: preserve file and folder zipping/extraction, archive traversal protection, readable sizes (KB/MB), no UI regressions. Work from latest feature branch, not main's UltraZip.

## EndraHome
- Central launcher: branch `feature/endrahome-run001`, draft PR [#10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10); source `Projects/EndraHome/runNNN/` on branch, **not main**.
- User-approved design intent: dark TI-native 320×240 interface; seven visually distinct cyan/blue/yellow/purple/green icons (GBA, ZIP, Cmd, Files, Images, Tools, Favorites), selected highlight, Enter launch, Menu options. Static design mockups are **not** evidence of built UI.
- Runs 001/002 **froze entire calculator**; do not reuse these architectures as safe baselines. Run003 rebuilt video/input using known gpSP platform and user said works; Run004 four-card GUI worked; Run005 seven-icon UI worked but labels collided; Run006 two-stage tiny companion launch test `EndraHome_Test006.tns` reported working.
- Run007 generic **seven user-configurable Ndless shortcuts**, no hardcoded ZIP tool, versioned config. Run008 clean caption diagnostics and verified limited tiny-app launch after confirmation (user measured ~10.04s). Run009 performance experiment remained ~10s. Run010 removed redundant OS screen refresh on exit; user confirmed fast launch/exit as a known-working reference. Run011 attempted return to Home after child. Later builds have explored built-in file browser/text/image viewing.
- Permanent config beginning **Run010**: `EndraHome_Apps.cfg.tns`, located with application; never overwrite existing user configuration or silently regenerate it.
- **Installed baseline on 2026-11-19.** Brandon identified Run028 Dynamic Launcher on his calculator. [Editable Run028 source](https://github.com/brandonendall/Nspire-Development-Hub/tree/780a111755f301a853826816c178a60dad7c33c9/Projects/EndraHome/run028) · [Passing build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44164319709).
- **Latest compiled build on 2026-11-19.** Run029 Desktop has the permanent Files tile, the navy/cyan E app icon and title badge, separate interface, text and folder colors, and four-way navigation. Native artifact `14566606768`. Physical Run029 verification is pending. [Run029 source](https://github.com/brandonendall/Nspire-Development-Hub/tree/feadc9783474de408ebb18c3b3419b1772d2983e/Projects/EndraHome/run029) · [Passing build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44597518309) · [Project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md).
- Owner wants EndraHome eventually to be a one-stop shop with user-selectable defaults and a built-in Files browser, text, JPG/PNG, PDF and basic file operations. Start conservatively with fast read-only and verified test paths before destructive features.

## EndraGBA (formerly UltraGBA)
- Source roots: `Projects/UltraGBA/` historical, `Projects/EndraGBA/` for new brand, upstream gpSP source `vendor/upstream-gpsp/`.
- Known user goals: quick loading and exiting, native ROM picker with readable folders, GBZ compressed ROM support, same-folder save behavior, custom RTC that advances, no unwarranted data directory rewrites.
- Historical working layout: executable adjacent to `UltraGBA_Data/` and test files; preserve actual user data. User reported EndraGBA FastExit launched/exited promptly.
- Run008 brand changes in PR #2; **Run009 FastExit** PR [#17](https://github.com/brandonendall/Nspire-Development-Hub/pull/17); **Run010 BrowserLeft** PR [#36](https://github.com/brandonendall/Nspire-Development-Hub/pull/36), branch `feature/endragba-run010-browser-left`, commit `bc4b3d9686cf65a3dccf96287293f585c0847e58`. Run010 organizes **folders on left, games on right**, preserving navigation, Shift bypass, ROM launch/resume, saves and fast exit. Requires device acceptance; keep Run009.
- The ROM picker previously presented folder entries crammed to the right and truncated; aesthetic fix must not break ROM loading. Don't include copyrighted ROM files in source or memory.

## EndraCommander NX / Endra SDL
- EndraCommander grew from restored `Projects/UltimateCommander/` baseline and earlier nCXplorer-style interest in high-capacity file/folder copy/move/ZIP.
- User wants **whole-folder cut/copy/paste**, large ~33MB files, clean popup/menu, and human-readable KB/MB rather than raw bytes; preserve icon coloring, cyan-gray theme.
- Runs 003–006 had full calculator hard freezes involving root browsing, ESC from safe-mode root warning, and .cfg text preview. PR [#7](https://github.com/brandonendall/Nspire-Development-Hub/pull/7) isolates root-warning ESC lock. These runs are **not** proven safe to recommend.
- Owner requested clean SDL/nSDL restart. PR [#8](https://github.com/brandonendall/Nspire-Development-Hub/pull/8) `fantasma/endra-sdl-restart-001`: read-only browser; user said **everything worked perfectly**. PR [#9](https://github.com/brandonendall/Nspire-Development-Hub/pull/9): cautious text/CFG preview; test on calculator before any broader file operations.
- Keep destructive operations behind explicit confirmation, bounds/path safeguards and real-device checks. Prefer safety over breadth.

## EndraDiagnostics
- Native diagnostics suite on TI-Nspire CX II CAS, versions **Run001 through Run024** tracked in numerous separate, chained PRs.
- Work includes LCD/keypad, battery and charging reporting, storage, read-only system diagnostics, CPU/RAM info, multi-screen inspections and eventually four-gauge dashboard. Preserve quick manual exit, avoid polling loops that hang device, avoid unverified RAM/CPU percentage claims.
- User repeatedly reported **CPU/RAM gauge needles absent or unreadable**. PR [#38](https://github.com/brandonendall/Nspire-Development-Hub/pull/38) Run024 on `feature/endradiagnostics-run024-gauge-clarity`, head `e6436ce11343a09e58a1fe85a3f44844812e2a32`: visual-only high-contrast needles, no 100% text covering gauges. Physical device acceptance **pending**.
- Distinguish actual hardware measurements from illustrative UI and bounded diagnostics; no synthetic percentages presented as live CPU/RAM telemetry.

## Shared build/toolchain
- `toolchain/build.sh`, pinned Ndless SDK images and GitHub Actions workflows; upload TNS and SHA-256 when possible. Use feature-specific workflow.
- Do not modify `ndless.cfg.tns`, system hooks, ROM associations, or OS root automatically. Recover from failures using the last user-confirmed build, not by repeatedly testing known-freezing candidates.
