# AGENTS.md — assistant and collaborator instructions

This file is for Hydra and any AI collaborator inspecting this repository.

**Start with [handoff/HYDRA-START-HERE.md](handoff/HYDRA-START-HERE.md)** for the current project state, developer handoff, proof of test results, and the active work branch. Then read the project's own README. Do not infer that the default branch contains the newest work.

## Repository facts
- Owner's repo: `brandonendall/Nspire-Development-Hub`.
- Default branch `main`: preserved source baselines and high-level documentation.
- EndraHome's active work: branch `feature/endrahome-run001`, draft PR #10.
- The repository is private. Links may require the account's GitHub access.
- EndraHome uses TI-Nspire CX II CAS + Ndless. Report successful GitHub Actions **separately** from actual calculator results.

## Current EndraHome evidence (2026-11-14)
- Run001 and Run002 **hard-froze** on physical calculator; do NOT recommend running them.
- Run003, Run004 and Run005: user shared actual-device photographs and reported the home interfaces working. No real app launches were possible in these builds.
- Run005 seven-icon UI had overlapping captions on the small screen; fixed in Run006.
- Run006: `Projects/EndraHome/run006/` limited **Tools companion launch probe**. Host+native CI passed, and user reported **“Everything works”** after the prescribed Run006 tests. Treat as user-reported success for that exact tiny program, not proof that arbitrary Ndless/GBA apps launch.
- Run007 (REDO): the original ZIP-only experiment was discarded **before physical testing**, and the current `Projects/EndraHome/run007/` is a **generic seven-slot launcher**. Each icon has a user-supplied label/path in `EndraHome_Apps007.cfg.tns`. No ZIP functionality or hardcoded targets. Paths must be under `/documents/`; the launcher reads (never writes) config, requires a separate confirmation, restores the native LCD/OS state before `nl_exec`, then returns to TI Documents. **Unconfigured icons cannot launch.** Physical testing pending; first test with the already-working `EndraHome_Test006.tns`, not GBA. [Current CI](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-universal007.yml).

- Run008 (latest): [`Projects/EndraHome/run008/`](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome/run008) isolates caption readability and config diagnostics in a new renderer while using the same Run007 `EndraHome_Apps007.cfg.tns`, parser, launch path and Run003 display/input. [Run008 CI 43606891909](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43606891909) passed host and native jobs. **Hardware compatibility and universal app launching remain unverified.** Preserve Run006/007 binaries; test simple companion first.
- See the detailed [handoff](handoff/HYDRA-START-HERE.md) for exact test evidence, procedure and preservation requirements.

- Run008: user photographed the configured Tools shortcut, launch confirmation and `EndraHome_Test006.tns` success popup. **Measured 10.04 seconds from final Enter until target appeared**. The tiny program works; 10-second latency requires measured investigation.
- Run009: [source](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome/run009), [passing CI 43611250479](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43611250479); strict **`EndraHome_Apps009.cfg.tns`**, no fallback; eliminates redundant target `fopen/fclose` and 180-ms sleep; **not yet tested on calculator**. Do not assume a fixed 180 ms delay explains the full 10.04 s.
- **Permanent config rule (supersedes earlier versioned-file request):** Starting **Run010**, every new EndraHome build must load and package **`EndraHome_Apps.cfg.tns`** without any build number. Run007–009 remain historical and still read their older filenames. The user can manually copy an older configured file **once** to the permanent filename, then reuse it across future builds; never silently migrate, overwrite or delete their files. Both actual loader and Help UI must show the same filename. CI should test it.
- **Run010 current experiment:** Removes redundant `refresh_osscr()` calls from a **locally copied** platform shutdown and the launcher return, while preserving all LCD/timer restoration. This may address user's report that EndraHome takes **~10 seconds to close**. It is **not tested on the calculator**; test immediate Esc-only return first, stop on any frozen or stale display. Run010 also adds a local `render.h` to fix the stale Apps007 Help label that appeared on a Run009 photograph.

## Engineering guardrails
1. Preserve the working UltraGBA Run007 source, ROM-local saves, GBZ paging, RTC handling and existing user data paths. `EndraGBA` is a future rename, not license to replace the baseline.
2. Preserve Run003 display/input code for comparison; only add one new variable/feature group per diagnostic build.
3. Do not alter `ndless.cfg.tns`, hooks, ROM associations, system/root files or CPU settings automatically.
4. Do not claim anything is device-tested without a specific real-device user result, and include negative results prominently.
5. Keep new EndraHome source changes on a feature branch and present a draft PR. Do not silently merge to `main`.
6. Before suggesting the next binary, compile native `.tns`, run host tests where available, identify the exact artifact/run ID, then label physical compatibility as pending.
7. Record every significant build with date, commit, artifact, source paths, successes, failures, and next action in the project's README or handoff. Do not overstate “working” when a single screenshot only shows the initial page.
8. This file and the handoff are snapshots, **not** an automatic updater or guarantee of background coordination. Check the latest PR commits and workflow runs before doing new work.

## Useful entry points
- [Root README](README.md)
- [Hydra handoff](handoff/HYDRA-START-HERE.md)
- [Active EndraHome PR](https://github.com/brandonendall/Nspire-Development-Hub/pull/10)
- [Run003 original baseline](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome/gba-ui)
- [Run004 visual experiment](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome/run004)
- [UltraGBA preserved baseline](Projects/UltraGBA/README.md)
