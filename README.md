# Nspire Development Hub

Private, multi-project development workspace for TI-Nspire CX II CAS applications using Ndless.

## Active development — EndraHome (updated 2026-11-14)

**Hydra / collaborators: start with [the current handoff](handoff/HYDRA-START-HERE.md) and [AGENTS.md](AGENTS.md).**

- **Active code branch:** [`feature/endrahome-run001`](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001), tracked by [draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10). The new EndraHome sources are **not merged to `main`**. This `main` README documents progress and where to find them.
- **Run001 — failed physical tests:** SDL launcher displayed, then completely froze on help/settings and while attempting GBA launch. **Do not use.**
- **Run002 — failed physical test:** Ndless framebuffer diagnostic displayed Home and Menu; device froze while in the Menu screen. **Do not use.**
- **Run003 — first user-reported working build:** rebuilt input/display using the established UltraGBA/gpSP architecture; the user photographed the home screen with another dock tile selected and reported that it **“works like a champ.”** External app launching was disabled. Documented success does **not** verify every exit/transition path independently.
- **Run004 — visual-only redesign (experimental):** new rounded dock icons, refined layout and screen styling. It directly compiles Run003's **unchanged** `gba-ui/platform.c` and `gba-ui/logic.c`; no app launches, settings writes or filesystem access. Sources: [run004/](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome/run004). Check its [CI workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-visual004.yml) for the current build status, then require separate device testing.

**Next engineering decision:** retain Run003 as a recovery baseline, establish Run004 display/navigation stability on hardware, and only afterward investigate external Ndless launches as a separately isolated experiment. Do not bring back Run001's launcher unchanged.

**Scope and naming:** `EndraHome` is the new desktop; `UltraGBA` is the retained emulator baseline; `EndraGBA` is its future product name. Do not rename or overwrite the proven emulator as part of launcher work.

## Projects
- **UltraGBA** — preserved original work through Run007. The product rename to **EndraGBA** is planned; source/build names remain UltraGBA until changes are verified.
- **UltraZip** — UltraZIP 003 source and pinned miniz dependency restored under `Projects/UltraZip`; dedicated calculator build workflow included.
- **Ultimate Commander** — complete UltraCommander NX Run001 source restored under `Projects/UltimateCommander`, with host tests and a dedicated calculator build workflow. The original executable name is retained.

## Shared Ndless build environment
Builds use a **shared external Ndless Docker toolchain**, not a copy of SDK binaries committed to GitHub. See `toolchain/README.md`. GitHub Actions can build `.tns` artifacts when the workflows and container image succeed. A successful CI run and device test are required before calling builds verified.

## UltraGBA preservation
The imported branch work comes from the provided `UltraGBA_COMPLETE_AI_REBUILD_HANDOFF` archive, originally from `OffCamera-Civilman/Nspire-Development-Hub`. The complete original branch history is retained in the portable bundle under `handoff/history/`; imported working files do **not** rewrite the private hub's main history. No copyrighted ROMs, BIOS or personal saves should be added.

The Run007 baseline, including GBZ saves, custom RTC and preservation of ROM-local data, must be retained. **Run008 native Documents-browser dispatch is unresolved**; do not reintroduce automatic `ndless.cfg.tns` mutation.

**Target hardware and OS:** TI-Nspire CX II CAS with compatible Ndless installation. The supplied documents mention both OS 6.2.0.333 and 6.4.0.74 at different development stages; verify the target device and Ndless version before compatibility claims.

## Verified GitHub Actions builds (November 13, 2026)
The shared Ndless toolchain smoke test and UltraGBA Run007 build both **passed** in this private repository; see `handoff/BUILD-VERIFICATION.md` for artifact links. The original nine-branch Git history is archived at `handoff/history/UltraGBA-all-repo-branches.bundle`. UltraZip and Ultimate Commander source imports are now complete. Their new builds are tracked in `handoff/SOURCE-RESTORATION-Run001.md`. Physical calculator test claims for these imported app builds remain separate; see the active EndraHome section for its specific results.

## Restore the remaining projects
The November 13 complete backup supplied the UltraZIP and UltraCommander source absent from the earlier UltraGBA-only handoff. See [source restoration](handoff/SOURCE-RESTORATION-Run001.md) for exact provenance and verification.

### Restored project builds — passing

- [UltraZIP 003](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43470855176): calculator build passed.
- [UltraCommander NX Run001](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43470855438): 129 host checks and calculator build passed.

Both generated `.tns` files match the original saved binaries byte for byte.
Physical calculator testing remains separate. The source import and exact
checksums are documented in the restoration handoff.
