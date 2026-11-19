# PROJECT_MEMORY.md — Nspire Development Hub
Updated 2026-11-19

## READ THIS FIRST IN EVERY NEW CHAT
This file is the durable project index. Read this file, then AGENTS.md, handoff/HYDRA-START-HERE.md, and the active project's README and latest GitHub Actions runs. Check ALL branches and open draft PRs: the default branch may NOT contain current work. Treat the repository and its commits as the source of truth, not chat summaries. Do not claim builds are device-tested until user confirms.

## EndraHome on TI-Nspire CX II CAS

Brandon identified **Run028 Dynamic Launcher** as the version installed and photographed on his calculator. The source is in `Projects/EndraHome/run028/` on `feature/endrahome-run001`. The branch name dates from Run001. [Project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md) · [Run028 source at the passing build](https://github.com/brandonendall/Nspire-Development-Hub/tree/780a111755f301a853826816c178a60dad7c33c9/Projects/EndraHome/run028) · [Draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10).

GitHub Actions passed the host tests and native build at commit `780a111755f301a853826816c178a60dad7c33c9`. The native artifact ID is `14364108177`. The preserved executable in the project is `Projects/EndraHome/builds/EndraHome_Run028_DynamicLauncher.tns`. [Build record](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44164319709).

## EndraHome design intent
TI-Nspire CX II CAS / Ndless lightweight customizable launcher. Preserve app shortcuts, permanent EndraHome_Apps.cfg.tns (never overwrite user settings), built-in Files browser, text preview, fast return/exit, and safe memory/CPU use. Planned file operations: rename, copy, move, delete; future integrated text editing, JPG/PNG viewing, PDF reading, configurable default apps. Do not modify OS hooks, ndless.cfg, or system files. Avoid device freezes and test changes incrementally.

## EndraTask on TI-Nspire CX II CAS

Run001 starts on [`feature/endratask-run001`](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endratask-run001). Read [the EndraTask project memory](Projects/EndraTask/PROJECT_MEMORY.md) for the pinned nKits Task Manager source, EndraTermux Build 010 direct-path probe, Ndless revision check and one-session test plan. Brandon confirmed **Ndless r2023** on OS 6.4.0.74. [Run002](Projects/EndraTask/run002) is the current build. Run002 source commit `2f228cd4acdd4c3c5ea18098b3cd4e43da4e8801`. Host tests and the native ARM build passed off-device. The source archive reproduced the delivered TNS. Calculator testing and report retrieval are pending. Run001 remains available as the read-only baseline.

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
