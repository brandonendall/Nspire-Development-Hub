# PROJECT_MEMORY.md — Nspire Development Hub
Updated: 2026-11-15

## READ THIS FIRST IN EVERY NEW CHAT
This file is the durable project index. Read this file, then AGENTS.md, handoff/HYDRA-START-HERE.md, and the active project's README and latest GitHub Actions runs. Check ALL branches and open draft PRs: the default branch may NOT contain current work. Treat the repository and its commits as the source of truth, not chat summaries. Do not claim builds are device-tested until user confirms.

## Repository and active development
Repository: brandonendall/Nspire-Development-Hub
EndraHome active branch: feature/endrahome-run001
EndraHome draft PR: #10
EndraHome source: Projects/EndraHome/run018/ on the feature branch (NOT main).
Latest known EndraHome CI: GitHub Actions run 43744290186; artifact EndraHome_Run018_ImageViewer, artifact ID 14173860704; source commit 03288c754cdd3dd9248ae7bb24600263cd56a7a4. Run018 image viewer compiled; physical calculator test pending. Run017_BuiltInFiles.tns is user-provided known reference; preserve functionality. Earlier user uploaded Run014_SaveFix.tns and Run015_AppManager.tns.

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
Retrieve EndraHome Run018 ImageViewer artifact and transfer .tns to calculator for physical testing. If binary artifact download cannot be accessed through tools, direct user to GitHub Actions artifact without inventing a sandbox file. After Run018 device test, continue Run019 file management improvements.
