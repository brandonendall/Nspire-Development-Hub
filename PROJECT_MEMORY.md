> **RUN033 BUILD VERIFIED — 2026-11-20**: Integrated RGB565 artwork and interactive launcher from source on `feature/endrahome-run001`. Final code fix commit `1df586e5e1238fff76962811cede61f9367125fa`. GitHub Actions run **44831808215**: host SUCCESS, native SUCCESS, report SUCCESS. Native artifact **EndraHome_Run033_Desktop**, ID **14677860274**; verified `EndraHome_Run033_Desktop.tns`, **205,407 bytes**, Zehn header, SHA-256 **04f211576b523e0f9b4e3a909e5428343df2e7dec9017f9196a96ae6451eae63** (matches SHA256SUMS.txt). Preview artifact ID **14677210930**: nine 320×240 host-rendered screenshots including Files-only desktop, configured-app grid and Files action menu were visually inspected in chat. **Device testing Run033 PENDING**. Run032 photos document working launch/Files/menu but severe cosmetic issues. Separate unresolved **Ctrl+Home native power-off** and **Home > 5 > 9 native Settings entry** must not be claimed fixed; preserving hardware means no unverified power register writes or OS hooking.

> **LATEST DEVICE / BUILD CORRECTION — 2026-11-20:** Run032 was transferred with verified `push <local .tns> /upload` and physically displayed on TI-Nspire CX II CAS; Files and File Actions popup render, configured shortcuts (GBA, ZIP, CXplorer, Files, Text) appear. User says card highlights/background and overall UI are inadequate. Run033 graphics source now committed (latest code includes the real Run031 RLE art, stable dark cards, cyan border-only highlight, dark Files/menu, extra host previews); Actions run **44831808215** host tests PASSED; native build pending until confirmed. **Unfixed and required:** physical Ctrl+Home must trigger **normal calculator power-off/standby** (existing handler only exits), and **native TI Home > 5 > 9** should open EndraHome like theme editor (not installed). These require OS 6.4.0.74-specific safe integration research; NO OS hooks/power-register experiments without explicit approval. Older notes saying `push` without `/upload` or treating Run029 as current are superseded. Preserve Run028/Run032 rollback. See `Projects/EndraHome/RUN033_NOTES.md`.

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

## Historical Run019 update — 2026-11-15
User confirmed Run018 oversized JPEG error ESC returns safely. Run019 source Projects/EndraHome/run019/; commit e8c252d8b1fc7654223712e4e43ba164daa76785; Actions run 43753801159 PASSED host and native; artifact EndraHome_Run019_LargeImages id 14176934540. Run019 supports JPEG/PNG up to 2MP decoded (2048 max dimension), downsamples retained pixels to <=1MP. This is NOT true streaming or scaled JPEG decode; larger files >2MP still rejected and memory peak includes full 2MP RGB. Physical test pending; keep Run018 fallback. Next: test user me.jpg.tns and verify ESC, no freeze.
