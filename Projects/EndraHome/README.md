# EndraHome — current project status (2026-11-14)

Repo: `brandonendall/Nspire-Development-Hub`. **Active code branch:** `feature/endrahome-run001`, [draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10). The root `main` branch has the [Hydra handoff](../../handoff/HYDRA-START-HERE.md) and [AGENTS.md](../../AGENTS.md). Inspect current branch sources before making claims.

## Hardware test history

| Build | Result | Meaning |
|---|---|---|
| Run001 | **FAILED** (full device freeze) | Original SDL/nSDL menu and GBA launcher froze. Do not use. |
| Run002 | **FAILED** (full device freeze) | Native `lcd_init` Menu/Help diagnostic froze. Do not use. |
| Run003 | **User-reported working** | UltraGBA/gpSP-derived native display/input rendered UI; ZIP tile selected in photo. |
| Run004 | **User-reported working** | Four-icon visual UI displayed on device, but didn't match the target appearance. |
| Run005 | **User-reported working** | Seven-icon cube-logo UI displayed on device; some captions overlapped. |
| Run006 | **User-reported working** | Corrected captions; Tools icon launched the standalone tiny companion test app after confirmation. |
| Run007 ZIP-only idea | **CANCELLED** | Was compiled but **not user-tested**. Superseded by the user-requested universal design. |
| **Run007 UNIVERSAL REDO** | **CI verified; device test pending** | Seven configurable app targets; **none** preassigned. No special ZIP behavior. |
| **Run008 UI polish** | **CI verified; device test pending** | Cleaner seven-icon labels and visible config status; same Run007 config/launch architecture. |

## The actual Run007: configurable launcher

[Run007 source and setup](run007/README.md). [GitHub Actions build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43604524140): host tests, renderer previews, native Ndless executable and SHA256 checksum **PASSED**. This is **not physical compatibility confirmation**.

- Launcher executable: `EndraHome_Run007_Universal.tns`.
- Companion plain-text file: `EndraHome_Apps007.cfg.tns`, placed beside the launcher.
- Seven colorful dock icons, 0–6. Each can have **any** explicit compatible Ndless `.tns` executable target under `/documents/`; icon names can be edited independently of the target paths.
- **All shortcuts ship disabled** to prevent accidental launch. A read-only parser validates complete paths and rejects traversal, root, system and non-TNS targets. Missing/unconfigured entries show a setup notice.
- User selects icon → Enter (details) → Enter (confirmation) → Enter (launch). Esc cancels. No filesystem scanning, built-in archiving, file management, app installation, `ndless.cfg.tns` changes or resident process.
- On launch, **restore LCD/timer/OS screen first**, then `nl_exec(path,0,NULL)`; this is the **user-reported working Run006 handoff pattern**. Launched apps close to TI Documents, **not** back into the EndraHome UI.
- Display/touchpad hardware handling is compiled **unchanged from `gba-ui/platform.c`** (Run003); seven-way navigation is compiled **unchanged from `run005/logic7.c`**. The Run006 caption fix is retained.

### First Run007 test

1. Keep working Run006 unchanged as rollback; do not overwrite UltraGBA or any other apps.
2. Put the Run007 binary and `EndraHome_Apps007.cfg.tns` in the same Documents folder. Set **only one initial app** to the known-working `EndraHome_Test006.tns`. Example (only if this path matches the actual calculator): `5=Tools|/documents/EndraHome/EndraHome_Test006.tns`.
3. Verify direct launch of the tiny app from TI Documents, and Run007 immediate Esc exit.
4. Navigate dock icons, Menu, unconfigured paths, detail view, then confirm one test launch. Confirm return to TI Documents.
5. Only then try a different independent **lightweight** Ndless app. Save EndraGBA for a separate compatibility trial; it may have higher memory/video demands. Stop testing after any freeze.

See [device test log](DEVICE-TEST-RESULTS.md) for all observed user reports, including failures.

## Source directory map

- `src/` — Run001 SDL (failed, retained historically).
- `recovery/` — Run002 diagnostic (failed).
- `gba-ui/` — Run003 gpSP-derived display/keyboard baseline (preserve).
- `run004/` — visually improved four-icon baseline.
- `run005/` — seven-icon cube-logo UI with pure state navigation.
- `run006/` — proven small companion-app launch pattern and caption fix.
- `run007/` — **current** universal configurable launcher; prior ZIP-only experiment exists only in git history.
- `GBA-ARCHITECTURE-ANALYSIS.md` — explanation of reused native engine.

## Hydra / AI work policy

Record source version, CI outcome and separate **physical** device result for every build. Preserve Run003–Run006 rollback artifacts, current UltraGBA Run007/RTC/GBZ/save behavior, and all unrelated apps. Keep EndraHome on the development branch and draft PR until ready. Do not report the universal launcher as fully tested merely because the tiny Run006 test handoff worked.

## Run008 — readable labels and visible config status (2026-11-14)

**New source:** [run008/](run008/). [Passing CI run 43606891909](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43606891909) includes host tests for the caption layout and original seven-slot configuration/state tests, 4 full-screen previews, and a native Ndless `.tns` build with SHA256 verification. **Physical device testing is pending.**

The user's Run007 calculator photo showed successful UI rendering, but labels still clipped/overlapped and all seven icons showed unconfigured gray dots; the user has not yet explicitly confirmed that a config file loaded or any Run007 app handoff worked. Run008 is a conservative improvement that shortens default labels to GBA, ZIP, Cmd, Files, Images, Tools, Favs, while preserving full names on detail pages. It also shows `CONFIG FILE MISSING`, `CHECK CONFIG FILE`, or `N / 7 READY` directly on Home; Menu explains configuration status.

**Important:** Run008 deliberately reads `EndraHome_Apps007.cfg.tns`, the same config filename and path semantics as Run007. The user should put that file alongside `EndraHome_Run008_UIPolish.tns` and edit a single slot (preferably the already-working Run006 tiny companion program) to test the universal launcher. All seven slots default disabled. This is NOT app autodiscovery or built-in ZIP functionality. The Run003 native input/video, Run005 seven-way logic, Run007 registry/parser, confirmation overlay and `nl_exec` sequence are compiled unchanged; only Run008 renderer/captions and executable title are new. **Test app launch separately; no GBA/universal-launch success claim until user verifies.**

See [run008/README.md](run008/README.md) for installation and safe test order. Preserve Run003–Run007 binaries and any user save files.
