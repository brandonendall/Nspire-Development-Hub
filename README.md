# Nspire Development Hub

Private, multi-project development workspace for TI-Nspire CX II CAS applications using Ndless.

## Active development — EndraHome (updated 2026-11-14)

**Hydra / collaborators: start with [the current handoff](handoff/HYDRA-START-HERE.md) and [AGENTS.md](AGENTS.md).**

- **Active code branch:** [`feature/endrahome-run001`](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001), tracked by [draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10). The new EndraHome sources are **not merged to `main`**. This `main` README documents progress and where to find them.
- **Run001 — failed physical tests:** SDL launcher displayed, then completely froze on help/settings and while attempting GBA launch. **Do not use.**
- **Run002 — failed physical test:** Ndless framebuffer diagnostic displayed Home and Menu; device froze while in the Menu screen. **Do not use.**
- **Run003 — first user-reported working build:** rebuilt input/display using the established UltraGBA/gpSP architecture; the user photographed the home screen with another dock tile selected and reported that it **“works like a champ.”** External app launching was disabled. Documented success does **not** verify every exit/transition path independently.
- **Run004 — user-confirmed working on calculator:** photo shows polished four-card UI, and the user reported “It works,” but requested much closer visual matching to the original seven-icon concept. Run004 remains preserved with its GBA-derived platform.
- **Run005 — user-confirmed working on calculator:** new photo shows the seven bright app icons, cube logo and home screen. User says “It works”; some of the longer labels overlap at 320×240. Its UI is still **preview-only** (no app execution).
- **Run006 — controlled launcher probe, awaiting hardware test:** [source](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome/run006) with readable two-line dock captions and a **single separately confirmed `Tools` launch test**. Uses unchanged GBA-derived `gba-ui/platform.c` and unchanged Run005 seven-way navigation, restores the TI LCD before calling the documented zero-argument `nl_exec(path,0,NULL)`. Requires a tiny companion `EndraHome_Test006.tns` in the same Documents folder; **cannot launch GBA or other actual apps**. [GitHub Actions run 43601629126](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43601629126) passed host state tests, software previews, both native binaries and checksums. **Not yet proven on the physical calculator.**

**Next engineering decision:** preserve the reported-working Run003–Run005 binaries. Test **Run006's tiny companion program directly first**, then its launcher navigation/exit, and only then the Tools → Enter → Enter → Enter handoff. Stop if any freeze recurs. Do not attempt an actual GBA launch until this isolated probe succeeds.

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
