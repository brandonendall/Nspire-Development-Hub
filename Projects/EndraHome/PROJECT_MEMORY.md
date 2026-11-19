# EndraHome — PROJECT_MEMORY.md
Updated: 2026-11-19
Repository: brandonendall/Nspire-Development-Hub
Active source branch: feature/endrahome-run001
Draft PR: #10
Source: Projects/EndraHome/
Read alongside root PROJECT_MEMORY.md, AGENTS.md, handoff/HYDRA-START-HERE.md, this project's README, and latest Actions results.

## Confirmed baseline
- Run001/002 froze device; NEVER recommend.
- Run003–006 reported working on physical hardware; Run006 tested limited small-app handoff.
- Run008 small test app launched; ~10.04 seconds.
- Run010 established persistent EndraHome_Apps.cfg.tns (NO run number); never overwrite user's configured file.
- Run017 built-in Files browser, text preview, fast Esc reported working by user.
- Run018 source in run018/, image viewer built by CI run 43744290186, artifact ID 14173860704, source commit 03288c754cdd3dd9248ae7bb24600263cd56a7a4. Physical image viewer test not yet verified.
- Preserve working Run017 as rollback; no OS hooks, firmware edits, ndless.cfg changes, forced resets or unnecessary hardware stress.

## Requested next major build — pending until compiled and tested
1. **Always-visible Files desktop tile**: built-in Endra Files default, assign alternate compatible file manager, persistent choice, fallback to built-in if unset/unavailable. The existing Run018 slot 3 (Files) already opens built-in browser if no assigned path, but user wants this explicitly guaranteed on main desktop.
2. **Readable labels**: replace cramped/squished 5x7 rendering with a legible, correctly spaced font. Ensure names fit and no overlap.
3. **Separate saved preferences**: global UI theme color, folder icon color, and text/font color; selectable presets, high-contrast defaults and restore-default action; do not overwrite app config.
4. **Correct four-direction navigation**: left/right/up/down in desktop icon grid.
5. **Toast messages**: expire rather than linger.
6. **Application icons**: distinguish executables from folders; roadmap for app-provided PNG icons, not yet implemented.
7. **Ctrl+ON**: normal safe calculator sleep/off behavior only after verifying Ndless input/power APIs. Do not invent undocumented power-register writes.
8. **Native TI OS Home → 5 Settings → 9 EndraHome**: research feasibility; no firmware patch or hooks without explicit safe plan and user consent. Not implemented.
9. Preserve image viewing, text preview, built-in Files, shortcut manager, fast exit, persistent config and user data.

## Engineering and build procedure
- Each session: inspect latest branch source, PR, CI and memory. Build a substantial cohesive change while avoiding risky hardware experiments.
- Each meaningful commit/build: update this file and root PROJECT_MEMORY.md with run number, commit SHA, CI workflow/run/artifact, verified physical test, regressions and next action.
- Keep project README and tests current. Build native .tns and host tests in GitHub Actions; do not call CI success a physical-device success.
- Preserve rollback .tns, never overwrite EndraHome_Apps.cfg.tns. No automatic writes to calculator system files.
- User uses Termux command `push <local-path>` (no destination argument) and prefers `clear` first.

## Current task
Begin next EndraHome source/build iteration from run018, prioritizing permanent Files desktop behavior, readable font and customizable text colors; confirm implementation and CI before claiming built. Native OS menu integration and Ctrl+ON require API investigation before inclusion.

## Critical correction from owner (2026-11-19)
The owner's **currently installed and photographed build is EndraHome Run028 'Dynamic...'** (full suffix not yet confirmed), NOT Run018. Run018 is merely the latest version identified in this branch's older documentation; DO NOT start new work by overwriting/replacing the owner's Run028 or labeling Run019 the next release. Locate Run028's actual source, commit, branch and artifact (possibly another repository/branch), inspect it, and continue with Run029 or the actual next run only after verification. Preserve Run028 as the known user-current baseline; request exact filename/artifact if repo search cannot locate it. The user's photos refer to Run028.
