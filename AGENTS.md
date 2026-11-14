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
- Run007: `Projects/EndraHome/run007/` controlled **ZIP app only** test (and Tools regression), built successfully in [workflow run 43603146050](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43603146050). Zip target is deliberately NOT bundled; user must copy a separately verified ZIP app into the launcher's Documents folder as `EndraHome_ZIP007.tns`. **Not physically tested**; do not state that real ZIP app launching works yet. EndraGBA icon is still disabled.
- See the detailed [handoff](handoff/HYDRA-START-HERE.md) for exact test evidence, procedure and preservation requirements.

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
