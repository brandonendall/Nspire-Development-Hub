# PROJECT_MEMORY.md — Nspire Development Hub
Updated 2026-11-19

## READ THIS FIRST IN EVERY NEW CHAT
This file is the durable project index. Read this file, then AGENTS.md, handoff/HYDRA-START-HERE.md, and the active project's README and latest GitHub Actions runs. Check ALL branches and open draft PRs: the default branch may NOT contain current work. Treat the repository and its commits as the source of truth, not chat summaries. Do not claim builds are device-tested until user confirms.

## EndraHome on TI-Nspire CX II CAS

Brandon identified **Run028 Dynamic Launcher** as the version installed and photographed on his calculator. The source is in `Projects/EndraHome/run028/` on `feature/endrahome-run001`. The branch name dates from Run001. [Project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md) · [Run028 source at the passing build](https://github.com/brandonendall/Nspire-Development-Hub/tree/780a111755f301a853826816c178a60dad7c33c9/Projects/EndraHome/run028) · [Draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10).

GitHub Actions passed the host tests and native build at commit `780a111755f301a853826816c178a60dad7c33c9`. The native artifact ID is `14364108177`. The preserved executable in the project is `Projects/EndraHome/builds/EndraHome_Run028_DynamicLauncher.tns`. [Build record](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44164319709).

## EndraHome design intent
TI-Nspire CX II CAS / Ndless lightweight customizable launcher. Preserve app shortcuts, permanent EndraHome_Apps.cfg.tns (never overwrite user settings), built-in Files browser, text preview, fast return/exit, and safe memory/CPU use. Planned file operations: rename, copy, move, delete; future integrated text editing, JPG/PNG viewing, PDF reading, configurable default apps. Do not modify OS hooks, ndless.cfg, or system files. Avoid device freezes and test changes incrementally.

## Other projects
Projects/EndraGBA, Projects/UltraGBA, Projects/UltraZip, Projects/UltimateCommander exist on main. Preserve existing functioning GBA emulator, ROM-local saves, RTC and GBZ paging. Consult each project's README before modifying it.

## Mandatory update protocol for ALL projects
1. At the start of each work session read this index, AGENTS.md, handoff/HYDRA-START-HERE.md, active branch, PR and project README.
2. After EVERY meaningful change, update this index with project name, branch, latest source directory, commit SHA, build/run and artifact ID, CI status, physical device test status, regressions, and next action.
3. Maintain a project-local README or STATUS.md with detailed change log and test steps. Preserve rollback builds.
4. Commit source and documentation to a project feature branch and use a draft PR; do not merge into main without user approval.
5. Never conflate CI PASS with physical-device PASS. Always provide exact downloadable artifacts or clearly state when connector access cannot download binary artifacts.

## Current next action
Use Run028 as the base for Run029. Follow the active branch’s [EndraHome project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md) for build status, verification and physical feedback.

## Historical Run019 update — 2026-11-15
User confirmed Run018 oversized JPEG error ESC returns safely. Run019 source Projects/EndraHome/run019/; commit e8c252d8b1fc7654223712e4e43ba164daa76785; Actions run 43753801159 PASSED host and native; artifact EndraHome_Run019_LargeImages id 14176934540. Run019 supports JPEG/PNG up to 2MP decoded (2048 max dimension), downsamples retained pixels to <=1MP. This is NOT true streaming or scaled JPEG decode; larger files >2MP still rejected and memory peak includes full 2MP RGB. Physical test pending; keep Run018 fallback. Next: test user me.jpg.tns and verify ESC, no freeze.
