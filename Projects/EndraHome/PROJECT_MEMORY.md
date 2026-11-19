# EndraHome project memory

Updated 2026-11-19. The project lives in `brandonendall/Nspire-Development-Hub`, on branch `feature/endrahome-run001`. [Repository index](../../PROJECT_MEMORY.md) · [Draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10).

## Run029 Desktop

**Latest compiled build, 2026-11-19.** [Source](run029/) · [Calculator executable](builds/EndraHome_Run029_Desktop.tns) · [GitHub Actions](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44597518309).

Run029 keeps Files on the desktop, opens built-in Files if an assigned manager fails, and navigates by the actual rows and columns. All desktop tiles use the navy-and-cyan E. The blue cube beside the title carries the same mark. Browser folders follow the chosen folder color. Full-width captions wrap on the desktop. Menu → Settings saves independent interface, folder, and text colors. Status messages clear after three seconds. Ctrl+On exits through normal cleanup into TI OS. The native Settings shortcut investigation and the reversible Browse route are documented in the [Run029 guide](run029/README.md).

| Build evidence | Verified record |
| --- | --- |
| Source branch | `feature/endrahome-run001`, draft PR #10 |
| Run directory | `Projects/EndraHome/run029/` |
| Visual integration | Prototype `72fca481` on the older Run026 branch, initial integration `34c64e3f`, final shared renderer [`b7cfed52`](https://github.com/brandonendall/Nspire-Development-Hub/commit/b7cfed5276d2428ae1f6923e9d5fc8d26601c0f5) |
| Build commit | [`feadc9783474de408ebb18c3b3419b1772d2983e`](https://github.com/brandonendall/Nspire-Development-Hub/commit/feadc9783474de408ebb18c3b3419b1772d2983e) |
| Native CI and artifacts | [Run 44597518309](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44597518309) passed host and native jobs. Calculator artifact `14566606768`, seven screen previews `14565832610`. |
| Executable identity | `EndraHome_Run029_Desktop.tns`, 108,999 bytes, SHA-256 `e9297d9a5fd924480b48a20ee6edf4e55af426452fe3a0996021f6fcddcb9168` |
| Independent checks | All nine host suites passed, including all 128 shortcut configurations and a 33 MiB folder copy. Seven CI images matched local rendering byte for byte. The preceding [successful build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44597459075) produced the identical executable. |
| Hardware | Run029 physical verification is pending. Brandon identified Run028 as his installed version. |

## Run028 Dynamic Launcher

**Installed baseline.** Brandon identified Run028 as the version installed and photographed on his TI-Nspire CX II CAS. Its desktop shows assigned shortcuts and its manager opens Endra Files. [Run028 source](run028/) · [Preserved executable](builds/EndraHome_Run028_DynamicLauncher.tns).

The implementation began at [`cfafa950`](https://github.com/brandonendall/Nspire-Development-Hub/commit/cfafa950b19608cc2096ff17c92cb758bf3e0682). The host tests and native build passed at [`780a1117`](https://github.com/brandonendall/Nspire-Development-Hub/commit/780a111755f301a853826816c178a60dad7c33c9), Actions run `44164319709`, native artifact `14364108177`. The archived executable is 111,282 bytes. Its SHA-256 matches the checksum packaged by that build, `799d85eb778341ac6caee18dd0fa47489a6da561e3d16c610ee30215373eb857`. [Verified build](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/44164319709).

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

Record the first physical Run029 result when the owner reports it. For a new build, use the verified Run029 source, retain numbered builds in this project, and update the build record and installed-device result independently.
