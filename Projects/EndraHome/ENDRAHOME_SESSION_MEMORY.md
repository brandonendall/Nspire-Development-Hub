> **LATEST DEVICE / BUILD CORRECTION — 2026-11-20:** Run032 was transferred with verified `push <local .tns> /upload` and physically displayed on TI-Nspire CX II CAS; Files and File Actions popup render, configured shortcuts (GBA, ZIP, CXplorer, Files, Text) appear. User says card highlights/background and overall UI are inadequate. Run033 graphics source now committed (latest code includes the real Run031 RLE art, stable dark cards, cyan border-only highlight, dark Files/menu, extra host previews); Actions run **44831808215** host tests PASSED; native build pending until confirmed. **Unfixed and required:** physical Ctrl+Home must trigger **normal calculator power-off/standby** (existing handler only exits), and **native TI Home > 5 > 9** should open EndraHome like theme editor (not installed). These require OS 6.4.0.74-specific safe integration research; NO OS hooks/power-register experiments without explicit approval. Older notes saying `push` without `/upload` or treating Run029 as current are superseded. Preserve Run028/Run032 rollback. See `Projects/EndraHome/RUN033_NOTES.md`.

# EndraHome verified session memory — 2026-11-20

## Mandatory pre-build procedure
1. Read this file and applicable repository project memory/roadmap, existing source, and last successful build logs before making changes.
2. Preserve verified user instructions over older ChatGPT summaries. If commands conflict, inspect the actual script and ask only if necessary.
3. Build with GitHub Actions; proactively inspect run results, fetch the artifact, verify .tns Zehn header/checksum, deliver direct .tns. Do not make user find run IDs unless tools genuinely cannot retrieve them.
4. Never claim calculator runtime behavior tested until user confirms it. Preserve hardware: no unnecessary resets, destructive file operations, or repeated hardware experiments.

## Verified transfer command (CURRENT; user confirmed 2026-11-20)
Termux: `clear` then `push /storage/emulated/0/Download/<file>.tns /upload`.
The trailing `/upload` argument is REQUIRED for this user's current workflow and has worked for days. Do NOT omit it on the basis of an older memory summary. Do not confuse with nlink's own upload subcommand or historical scripts that targeted calculator root (/). Confirm the local source path for each build.

## Graphics milestone
EndraHome Run031 Graphics Demo, GitHub Actions run #3, run ID 44804296431, artifact EndraHome_Run031_GraphicsDemo (artifact ID 14662028363). Native .tns compiled and verified by workflow. User's calculator photo confirms the selected electric-blue EndraHome artwork displays successfully on TI-Nspire CX II CAS. Standalone read-only demo; do not mistake it for completed interactive launcher.

## UI direction — binding
- Use EndraZIP file browser and pop-up menu as the immediate visual/interaction reference. User loves its styling, menu, spacing and file browsing; EndraHome Files should resemble it closely, with EndraHome-specific icon/image tweaks and a much more extensive contextual menu.
- EndraZIP uses native Ndless C RGB565 framebuffer rendering, lcd_blit and custom bitmap fonts (not SDL), per previous project discussion. Verify actual code before porting.
- Prior approved visual direction: dark charcoal background, cyan highlight/selection, gold folder icons, green ZIP icons, white file icons, right-aligned file sizes, polished pop-up menus. Keep performance lightweight.
- EndraHome main launcher should show only user-selected apps, adaptively arranged 1–7 uniform icons; don't force fixed GBA/ZIP/terminal/files dock icons. Enter launches directly; Menu offers details/config.
- File browser is default picker and full file manager; roadmap includes copy/paste, move, rename, delete, text/image/PDF viewers, and configurable alternate apps. Avoid duplicative pages or unnecessary status counters.
- Prioritize polishing EndraHome before Endra Packages; future app bundles with icons remain roadmap, not a reason to delay core UI.

## Repository / workflow
Repository brandonendall/Nspire-Development-Hub; current feature branch feature/endrahome-run001. Graphics demo source Projects/EndraHome/run029; workflow .github/workflows/endrahome-graphics-demo.yml.
Keep a dated, explicit distinction between planned, source committed, CI passed, downloaded/verified, and hardware tested. Never claim a background build is underway without triggering/checking it.

## Run032 big-jump requirements — confirmed 2026-11-20
- User explicitly rejects visible **Add App** tile/button on desktop. Remove it entirely. **Files is the only permanently visible default tile** when no user applications have been configured; no other placeholders, empty slots, or setup tiles.
- When the user assigns applications through the Menu/Settings flow, show only the actual assigned app icons plus Files. Preserve ability to add/change/remove apps in a menu, not on the home screen. Do not conflate hiding Add App with deleting app assignment functionality.
- Consolidate this desktop change with EndraZIP-inspired polished Files browser and extended pop-up menu into **one substantial Run032 build**, rather than multiple minor hardware-test builds.
- Preserve existing config compatibility and fallback to built-in Files; keep tested launch/exit/OS display restoration and hardware safety. No automatic resets, filesystem writes, or destructive operations; writes only after explicit user action and confirmation where needed.
- Check actual EndraZIP renderer code before claiming parity; maintain true 320x240 layout, responsive controls, and memory-aware assets. Host tests and CI first, one deliberate calculator test after verified artifact.
- The previous note about a default 1–7 tile arrangement is superseded for **unassigned slots**: never show an Add App or blank tile. Files-only when none assigned.

## Run032 implementation checkpoint (2026-11-20)
- Source updated in existing run029 code directory (not a separate new run032 directory): desktop.c already filtered unassigned slots; test.c corrected to require Files-only empty desktop; render.c removes fallback Add App label, labels R032; picker_render.c adds cyan frame/selection and removes redundant count strip; files_render.c refines cyan pop-up border.
- Makefile now targets EndraHome_Run032_Desktop.tns; workflow .github/workflows/endrahome-run029.yml renamed Run032, configured branch push trigger on source changes, runs host suites and native Ndless compilation and uploads Run032 artifact.
- This is a first integrated source pass, **not yet confirmed CI pass or hardware tested**. Check CI run and tests; fix any failures before distribution. Do not claim the concept-art background is fully integrated: Run031 GraphicsDemo proved artwork but current launcher uses native dynamic gradient. More visual polish is required to match concept.
- GitHub Actions latest-build issue #44 may lag; inspect it for new run status before using it as evidence.

## Run032 CI correction & verified build — 2026-11-20
- Initial GitHub Actions Run032 workflow run 44815791615 FAILED at host `test_return.c` due obsolete assertion expecting unconfigured Add App slot 0 to be selected. Native stage correctly skipped. Updated test to expect Files as first tile; fix commit `6abfabf19158b8efbf13083c56d5328ba48e2239`.
- GitHub Actions Run032 run #36, ID **44827771860**: host SUCCESS, native SUCCESS, report SUCCESS. Artifact **EndraHome_Run032_Desktop** ID **14675430613** downloaded and independently inspected. Executable **EndraHome_Run032_Desktop.tns**, **140531 bytes**, Zehn header, SHA-256 `9970966a7b38935bdf93089191890c7e2b6bc2ded2c61e26c20fc3ab01568c1c`, matches workflow SHA256SUMS.txt. **Physical Run032 test PENDING**. Keep Run028 and working Run031 graphics demo as rollback/milestones. Run032 styling is source improvements, NOT full Run031 artwork integration yet.
- For future runs, connected GitHub `fetch` action CAN read `https://api.github.com/repos/brandonendall/Nspire-Development-Hub/actions/runs`; parse workflow_runs, select matching workflow name and branch, then use `fetch_workflow_run_jobs`, `fetch_workflow_run_artifacts`, `download_workflow_artifact`. Do not rely on commit workflow runs function (PR-only) and do not ask user to find run ID unless truly unavoidable.
- Current transfer: `clear` followed by `push /storage/emulated/0/Download/EndraHome_Run032_Desktop.tns /upload` (required trailing destination).
