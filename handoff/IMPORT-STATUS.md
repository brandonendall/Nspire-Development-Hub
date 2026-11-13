# Handoff import record — November 12, 2026

## Source inspected
User-supplied `UltraGBA_COMPLETE_AI_REBUILD_HANDOFF (1).zip`: **397 archived files**, approximately 6.6 MB uncompressed. The package contains legacy branch snapshots, historical Git bundle, 6 .tns binaries, pinned gpSP upstream source, patch scripts, CI workflows, documentation and checksum manifests.

## Imported into the hub
- Run007 branch's **25 text files** from original `OffCamera-Civilman/Nspire-Development-Hub`, with older documentation and CI recipes stored under `handoff/` so they do not misrepresent current status or trigger obsolete builds.
- **185 text files** from exact pinned gpSP upstream commit, under `vendor/upstream-gpsp/`. The 16 KiB GPL-licensed open replacement BIOS binary was skipped in the connector transfer; supported CI builds clone the full upstream source at the pinned SHA.
- Shared build wrapper, updated SDK smoke-test workflow, and project registration notes. All original Run003–Run007 scripts are preserved unchanged.

## Not imported, and why
- Original **full Git history bundle** (binary) and **6 historical .tns binaries**: retained in the original uploaded ZIP; they were not transferred to this repository by the GitHub text/API import.
- Historical snapshots of all branches: recoverable from the original Git bundle and source repository but not re-imported as dozens of duplicate directories here.
- UltraZip and Ultimate Commander actual source: not part of this UltraGBA archive; placeholders only.
- The full Ndless SDK binary distribution: not part of this archive; build via external Ndless Docker image.

## Verification required
1. Verify the repository tree and new commit.
2. Check the shared SDK smoke GitHub Action; no verified result recorded yet.
3. Check the original Run007 GitHub Action builds and uploads a usable .tns.
4. Test the .tns on the actual TI-Nspire; no new device test is claimed.
5. Restore the original full Git history separately if required, without rewriting the new hub's commits.

Never upload copyrighted ROMs, official BIOS, firmware, personal saves, or secrets.
