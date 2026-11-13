# Nspire Development Hub

Private, multi-project development workspace for TI-Nspire CX II CAS applications using Ndless.

## Projects
- **UltraGBA** — preserved original work through Run007. The product rename to **EndraGBA** is planned; source/build names remain UltraGBA until changes are verified.
- **UltraZip** — compression utility and built-in file manager (source not present in the supplied UltraGBA handoff; awaiting separate import).
- **Ultimate Commander** — Ndless file manager and launcher (source not present in the supplied UltraGBA handoff; awaiting separate import).

## Shared Ndless build environment
Builds use a **shared external Ndless Docker toolchain**, not a copy of SDK binaries committed to GitHub. See `toolchain/README.md`. GitHub Actions can build `.tns` artifacts when the workflows and container image succeed. A successful CI run and device test are required before calling builds verified.

## UltraGBA preservation
The imported branch work comes from the provided `UltraGBA_COMPLETE_AI_REBUILD_HANDOFF` archive, originally from `OffCamera-Civilman/Nspire-Development-Hub`. The complete Git-history bundle and pinned upstream snapshot are retained in the uploaded ZIP; this repository's imported working files do **not** automatically restore the original history. No copyrighted ROMs, BIOS or personal saves should be added.

The Run007 baseline, including GBZ saves, custom RTC and preservation of ROM-local data, must be retained. **Run008 native Documents-browser dispatch is unresolved**; do not reintroduce automatic `ndless.cfg.tns` mutation.

**Target hardware and OS:** TI-Nspire CX II CAS with compatible Ndless installation. The supplied documents mention both OS 6.2.0.333 and 6.4.0.74 at different development stages; verify the target device and Ndless version before compatibility claims.

## Verified GitHub Actions builds (November 13, 2026)
The shared Ndless toolchain smoke test and UltraGBA Run007 build both **passed** in this private repository; see `handoff/BUILD-VERIFICATION.md` for artifact links. The original nine-branch Git history is archived at `handoff/history/UltraGBA-all-repo-branches.bundle`. Device testing, UltraZip and Ultimate Commander source import are still outstanding.
