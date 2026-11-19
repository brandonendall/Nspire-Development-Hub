# PROJECT_MEMORY.md — Nspire Development Hub

> **Endra-wide memory hub (2026-11-17):** Start with [Endra-Memory/MASTER_MEMORY.md](Endra-Memory/MASTER_MEMORY.md) for the complete cross-repository knowledge base (Nspire + Android + Termux + Casio), plus repository inventory, active branches, build history, devices, command preferences, known bugs, development rules and backlog. This existing PROJECT_MEMORY.md remains the historic EndraHome-specific snapshot; check live PRs/Actions for updates.

Updated 2026-11-19

## READ THIS FIRST IN EVERY NEW CHAT
This file is the durable project index. Read this file, then AGENTS.md, handoff/HYDRA-START-HERE.md, and the active project's README and latest GitHub Actions runs. Check ALL branches and open draft PRs: the default branch may NOT contain current work. Treat the repository and its commits as the source of truth, not chat summaries. Do not claim builds are device-tested until user confirms.

## EndraHome build location

**Latest compiled version.** Run029 Desktop passed the host tests and native Ndless build. The source at this build is `feadc9783474de408ebb18c3b3419b1772d2983e`, the GitHub Actions run is `44597518309`, and the native artifact ID is `14566606768`. The first physical Run029 check is pending. Brandon identified Run028 Dynamic Launcher as the build installed on his calculator. [Run029 source](https://github.com/brandonendall/Nspire-Development-Hub/tree/feadc9783474de408ebb18c3b3419b1772d2983e/Projects/EndraHome/run029) · [Build record](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44597518309) · [Project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md).

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
Record Run029 physical feedback in the [active EndraHome project memory](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/PROJECT_MEMORY.md).
