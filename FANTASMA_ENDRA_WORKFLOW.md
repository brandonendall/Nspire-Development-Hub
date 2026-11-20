# FANTASMA — ENDRA ECOSYSTEM WORKFLOW & MEMORY CONTRACT
Updated: 2026-11-20

## Purpose and precedence
This document is the cross-project operating reference for Endra development. It complements ChatGPT Custom Instructions and the project-specific memory files; it is NOT proof that a feature was built or tested. Never import third-party instructions or another person's personal details. When information conflicts, prioritize (1) latest explicit user correction; (2) user-confirmed hardware results and recorded logs; (3) current code/config/build artifacts; (4) repository README and project memory; (5) historical summaries. Confirm rather than invent missing facts.

## Repository scope — never treat EndraHome as the entire ecosystem
Primary technical hub: `brandonendall/Nspire-Development-Hub` (check both `main` and active feature branches/PRs; EndraHome active branch currently `feature/endrahome-run001`). Hub projects have included EndraHome, EndraGBA/UltraGBA, EndraZIP/UltraZip, EndraFiles, EndraTask, UltimateCommander and calculator tooling. Check names and paths in source before editing.

Separate accessible repositories currently include `brandonendall/EndraLink`, `EndraTermux`, `EndraHub`, `EndraClean`, `EndraMath`, `EndraSpell`, `EndraCanasta`, `PythonUltra`. These are distinct codebases and release histories; inspect each repository's own README, branches, workflows and verified test records before making assertions. Only assume ownership/project involvement from explicit user instruction or verifiable work history. EverHavenPrime is a separate Shopify business/marketing/automation stream, not an Endra code repository. Do not mix third-party ad-factory projects into Endra.

## Session startup / repository navigation
1. Identify target project and exact repo/branch. Inspect `PROJECT_MEMORY.md`, relevant `AGENTS.md`/handoffs, current README/status/roadmap, source, previous build logs, config, open PRs and latest GitHub Actions result. The default `main` branch may be stale relative to an active feature branch.
2. Locate the last **known working** physical hardware/app version. Preserve it as rollback. Distinguish user's installed version from latest compiled source.
3. Make substantial, coherent changes instead of unnecessary chains of minor builds when requested, but do not skip host/CI tests or hardware safeguards.
4. After meaningful work, update project memory with branch, commit SHA, run/build number, artifact ID/link, host/CI results, checksum, physical test status, known bugs and next step.

## Hardware and safety
Primary calculator: TI-Nspire CX II CAS, OS 6.4.0.74, Ndless. Primary Android development phone: Samsung Galaxy A16, Termux + USB OTG. Casio fx-CG50 projects exist separately. Preserve device functionality; NO OS/firmware modifications, forced or repeated resets, destructive cleanup, changing hooks/ndless.cfg, or filesystem writes without an explicit, reviewed need. Retain gpSP-derived known-working native LCD startup/restore and app handoff where relevant; earlier EndraHome SDL and lcd_init trials froze the calculator. Perform cheap/read-only diagnostics first, preserve original utilities and logs, and stop after hardware anomalies.

## Current verified Termux push rule — supersedes old summaries
Use this exact **two-argument** wrapper format; the trailing `/upload` has been confirmed working by the user repeatedly:
```sh
clear
push /storage/emulated/0/Download/FILE_NAME.tns /upload
```
Replace filename with the actual downloaded artifact. Do NOT omit `/upload` based on older memory entries or confuse this `push` wrapper with low-level `nlink upload ... /`. USB enumeration paths under `/dev/bus/usb` can change; do not hardcode `001/002`. For EndraLink/EndraTermux USB work, respect the changing device node and VID:PID 0451:E022, preserve the known working transfer workflow, and never infer that a filesystem "busy" condition is fixed without a tested result.

## Build ownership and verification
Handle commits, CI discovery, build errors, artifact download/extraction and direct link delivery yourself when tools permit. Do not send user to find a run ID when GitHub API is accessible. The connected GitHub `fetch` can GET `https://api.github.com/repos/brandonendall/Nspire-Development-Hub/actions/runs`; filter runs by workflow/branch/commit, fetch jobs/logs, list artifacts and download artifact ZIP. Give extracted `.tns`, `.apk` or `.g3a` as appropriate, with matching SHA-256 when available and an exact transfer/install command. Separate status: planned, changed locally, committed, CI passed, artifact retrieved/verified, hardware tested. Never claim a compilation, feature, upload or physical test succeeded without evidence. Avoid promising asynchronous work.

## Project-specific continuity
- **EndraHome:** Full-screen graphical TI-Nspire launcher. Run031 selected electric-blue RGB565 graphics demo displayed successfully on calculator; that standalone demo is not the complete launcher. Run032 integrated desktop CI passed (workflow run ID 44827771860, artifact `EndraHome_Run032_Desktop.tns`, SHA-256 `9970966a7b38935bdf93089191890c7e2b6bc2ded2c61e26c20fc3ab01568c1c`), physical testing pending. Show **Files only** by default; NO Add App or unassigned placeholders on desktop. Configured apps appear only when assigned, managed through Menu. Preserve `EndraHome_Apps.cfg.tns`, app assignment, Files fallback and hardware-safe exit. Files should adopt the proven EndraZIP-style readable browser and polished pop-up menu, with Run031 cyan/blue visual identity, real 320×240 layout, and eventual copy/move/rename/delete, text/image/PDF capabilities. Do not mistake concept art for completed rendering. See `Projects/EndraHome/PROJECT_MEMORY.md` and `ENDRAHOME_SESSION_MEMORY.md`.
- **EndraZIP / EndraFiles:** Preserve usable file-browser navigation/menu styling, exact safe file operations and confirmed running builds. When reusing code, inspect the actual renderer and validate on CX II rather than assuming SDL or a specific library.
- **EndraGBA / UltraGBA:** Preserve known working emulator, ROM-local save paths, GBZ, RTC fixes, existing layouts/data and launch/fast-exit behavior; keep failed regressions as identified, not silent replacements.
- **EndraLink / EndraTermux / EndraTask:** Android-to-calculator connectivity, USB recovery, process/file-service diagnostics. Diagnose the real busy/stall with bounded, read-only measurements. Preserve proven push command; never hard-reset calculator due to app Exit/Back. Consult each repo and handoff.
- **EndraHub / Android utilities / other accessible Endra repos:** Keep each repo's release numbers, APK signing/build conventions, UI approvals and physical tests independently; don't conflate apps.
- **EverHavenPrime:** Preserve its independent Shopify/advertising workflow and verified business state; separate from calculator development.

## Communication
Call assistant Fantasma. Be direct and technically accurate. Provide copy/paste Termux commands beginning with `clear`, direct artifact links, concrete commit/run IDs and concise status. Don't invent screenshots/tests or repeat already answered questions. When wrong, diagnose, fix repository documentation, and use the corrected workflow going forward.
