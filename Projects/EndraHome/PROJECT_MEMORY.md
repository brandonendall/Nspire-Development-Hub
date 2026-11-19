# EndraHome project memory

Updated 2026-11-19. The project lives in `brandonendall/Nspire-Development-Hub`, on branch `feature/endrahome-run001`. [Repository index](../../PROJECT_MEMORY.md) · [Draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10).

## Run028 Dynamic Launcher

**Installed baseline.** Brandon identified Run028 as the version installed and photographed on his TI-Nspire CX II CAS. Its desktop shows assigned shortcuts and its manager opens Endra Files. [Run028 source](run028/) · [Preserved executable](builds/EndraHome_Run028_DynamicLauncher.tns).

The implementation began at [`cfafa950`](https://github.com/brandonendall/Nspire-Development-Hub/commit/cfafa950b19608cc2096ff17c92cb758bf3e0682). The host tests and native build passed at [`780a1117`](https://github.com/brandonendall/Nspire-Development-Hub/commit/780a111755f301a853826816c178a60dad7c33c9), Actions run `44164319709`, native artifact `14364108177`. The archived executable is 111,282 bytes. Its SHA-256 matches the checksum packaged by that build, `799d85eb778341ac6caee18dd0fa47489a6da561e3d16c610ee30215373eb857`. [Verified build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44164319709).

## Run029 Desktop

Run029 is based on the verified Run028 sources. The [new source](run029/) and [Run029 workflow](../../.github/workflows/endrahome-run029.yml) include the persistent Files tile, external-file-manager fallback, four-direction navigation, full bitmap captions, distinctly drawn apps and folders, independent UI/folder/text colors and three-second status messages. Nine local host suites passed. The native build is pending. Physical verification is pending. Ctrl+On exits to TI OS. The [Run029 notes](run029/README.md) document the optional Browse shortcut and native Settings API investigation.

## Earlier builds
- Run001/002 froze device; NEVER recommend.
- Run003–006 reported working on physical hardware; Run006 tested limited small-app handoff.
- Run008 small test app launched; ~10.04 seconds.
- Run010 established persistent EndraHome_Apps.cfg.tns (NO run number); never overwrite user's configured file.
- Run017 built-in Files browser, text preview, fast Esc reported working by user.
- Run018 source in run018/, image viewer built by CI run 43744290186, artifact ID 14173860704, source commit 03288c754cdd3dd9248ae7bb24600263cd56a7a4. The owner later confirmed that Esc returned safely from the oversized JPEG error.
- Preserve working Run017 as rollback; no OS hooks, firmware edits, ndless.cfg changes, forced resets or unnecessary hardware stress.


## Future work
App-supplied PNG icons remain on the roadmap. Keep physical results and regressions with their specific run numbers.

## Engineering and build procedure
- Each session: inspect latest branch source, PR, CI and memory. Build a substantial cohesive change while avoiding risky hardware experiments.
- Each meaningful commit/build: update this file and root PROJECT_MEMORY.md with run number, commit SHA, CI workflow/run/artifact, verified physical test, regressions and next action.
- Keep project README and tests current. Build native .tns and host tests in GitHub Actions; do not call CI success a physical-device success.
- Preserve rollback .tns, never overwrite EndraHome_Apps.cfg.tns. No automatic writes to calculator system files.
- User uses Termux command `push <local-path>` (no destination argument) and prefers `clear` first.

## Current task
Record the Run029 CI result and wait for the owner's first physical Run029 check. Keep Run028 as the installed baseline until the owner reports a change.

## Run028 source recovery and Run029 visual refresh — 2026-11-19

**Verified recovery:** The authoritative Run028 editable source is `Projects/EndraHome/run028/` on branch `feature/endrahome-run001`, confirmed by fetching `run028/Makefile` (APP=`EndraHome_Run028_DynamicLauncher`) and `run028/render.c`. Run028 source/build commit `780a111755f301a853826816c178a60dad7c33c9`, Actions run `44164319709`, artifact `14364108177`. Do not mistakenly search only branch `feature/endrahome-run026-adaptive-files`: its root memory is stale and its latest EndraHome sources stop earlier. The user-current device build is Run028; preserve it as rollback and do not substitute Run025.

**Run029 UI approval:** Keep the Run028 working behavior, application labels, shortcuts and 3x2 launcher layout. Replace the six identical yellow folder graphics with **one identical universal dark-navy/cyan E app icon**, using a single reusable lightweight RGB565 drawing routine. Add the matching blue EndraHome E/cube badge to the left of the title. Keep selection highlight. App-specific PNG icons, clock and battery are future roadmap items, not requirements for this visual patch.

**Important branch correction:** An initial stand-alone prototype `Projects/EndraHome/shared/universal_icon.h` was committed at `72fca48170ca909aa14dbf1dbca06526500c560e` on the **older** branch `feature/endrahome-run026-adaptive-files`. It is **not yet integrated** into Run029. Port/adapt the routine to `feature/endrahome-run001` after reviewing its existing `Projects/EndraHome/run029/` source and workflow; avoid replacing newer Run029 features with older code. Existing Run029 host tests were reported passing, but the native Run029 build and device test were pending at last verified memory read. Recheck Actions before claiming success.

**Next actions:** inspect active `run029/render.c` and Makefile, integrate the approved universal icon and header badge while retaining all existing Run029 enhancements, run host/native CI, record commit/artifact/status here and in root memory, and provide a real TNS artifact only after verified build. Never overwrite the user's shortcut configuration or force device resets.

### Run029 visual integration checkpoint — 2026-11-19
The approved shared dark navy/cyan E badge is now **integrated** in `run029/render.c` for every desktop app tile (including Files) and for the top-left EndraHome logo. Commit `34c64e3f31c55d97af5876142d8418c849431f13` on `feature/endrahome-run001`. Existing Run029 launcher layout, labels, navigation, and file-manager logic were left unchanged. This is source integration, **not a verified native build**. The commit workflow lookup returned no PR-triggered runs at the time checked; host/native compilation and on-device testing remain pending. Preserve Run028 rollback.
