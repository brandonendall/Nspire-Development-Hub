# Nspire Development Hub

Private, multi-project development workspace for TI-Nspire CX II CAS applications using Ndless.

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
The shared Ndless toolchain smoke test and UltraGBA Run007 build both **passed** in this private repository; see `handoff/BUILD-VERIFICATION.md` for artifact links. The original nine-branch Git history is archived at `handoff/history/UltraGBA-all-repo-branches.bundle`. UltraZip and Ultimate Commander source imports are now complete. Their new builds are tracked in `handoff/SOURCE-RESTORATION-Run001.md`. Physical device testing remains outstanding.

## Restore the remaining projects
The November 13 complete backup supplied the UltraZIP and UltraCommander source absent from the earlier UltraGBA-only handoff. See [source restoration](handoff/SOURCE-RESTORATION-Run001.md) for exact provenance and verification.
