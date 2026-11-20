> **Cross-repository operating instructions (updated 2026-11-20):** Read [FANTASMA_ENDRA_WORKFLOW.md](FANTASMA_ENDRA_WORKFLOW.md) for Endra ecosystem scope, verified `/upload` Termux command, GitHub Actions/artifact retrieval, device safety, and project-specific continuity. This file supersedes older conflicting push syntax summaries; inspect active branch/source and user-confirmed tests before work.

# PROJECT_MEMORY.md — Nspire Development Hub
Updated 2026-11-19

## READ THIS FIRST IN EVERY NEW CHAT
This file is the durable project index. Read this file, then AGENTS.md, handoff/HYDRA-START-HERE.md, and the active project's README and latest GitHub Actions runs. Check ALL branches and open draft PRs: the default branch may NOT contain current work. Treat the repository and its commits as the source of truth, not chat summaries. Do not claim builds are device-tested until user confirms.

## EndraHome on TI-Nspire CX II CAS

**Latest compiled version.** Run029 Desktop passed the host tests and native Ndless build. The source at this build is `feadc9783474de408ebb18c3b3419b1772d2983e`, the GitHub Actions run is `44597518309`, and the native artifact ID is `14566606768`. The first physical Run029 check is pending. Brandon identified Run028 Dynamic Launcher as the build installed on his calculator. [Run029 source](https://github.com/brandonendall/Nspire-Development-Hub/tree/feadc9783474de408ebb18c3b3419b1772d2983e/Projects/EndraHome/run029) · [Build record](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44597518309) · [Project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md).

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
Record the owner’s first physical Run029 result in the [EndraHome project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md). Keep the latest compiled build and the owner’s installed version as separate entries.
