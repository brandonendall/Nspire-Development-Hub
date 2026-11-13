# Remaining Nspire source restoration — Run001

Destination: brandonendall/Nspire-Development-Hub.
Backup: Nspire_Repo_Complete_Backup_Run001_2026-11-13.zip, SHA-256
`db97eb0d84626f114082c9481b9882cd71aee797cdca21d6d9a92853d2832ede`.

## Restored source

- UltraZIP 003: original main revision 47e9b6d9da2fe6e6df80a16c4d2e1d9b6ae84b1a,
  now under Projects/UltraZip, with pinned miniz 3.0.2 source and license.
- UltraCommander NX Run001: all 42 original source-package files restored
  under Projects/UltimateCommander. Existing app and output names remain unchanged.
- Pinned gpSP snapshot: restored its missing GPL open replacement BIOS.

SOURCE-RESTORATION-MANIFEST.json records the original hashes and destinations.
Application C sources, Makefiles, patch scripts and original dependency files
were not changed. UltraZIP's old README is retained as README.original.md;
its new README describes the existing 003 build correctly.

## Validation

- All 92 imported source-package files matched their backup bytes.
- UltraCommander original host suite passed all 129 checks under AddressSanitizer
  and UndefinedBehaviorSanitizer in the local Linux environment.
- Both GitHub calculator builds passed at import commit
  `dc3675264d2e6ce0201caf9972435d91157b0564`.
- Both downloaded calculator binaries matched their original backup byte for byte.
- Original host suite also passed all 129 checks on the GitHub runner.
- Physical calculator tests: not performed for these restored builds.

## Workflows

- .github/workflows/ultrazip.yml produces UltraZIP_003_Restored_Run001.
- .github/workflows/ultracommander.yml produces UltraCommanderNX_Run001_Restored_Run001.
- The pre-existing UltraGBA Run007 workflow and original-history bundle remain
  the foundation for emulator work. Its EndraGBA rename remains planned.

No account permissions or repository visibility changes are part of this import.

## Successful build evidence

| Application | Workflow run | Artifact | Bytes | Original binary match |
|---|---|---|---:|---|
| UltraZIP 003 | [43470855176](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43470855176) | UltraZIP_003_Restored_Run001 (14059734970) | 211408 | Exact |
| UltraCommander NX Run001 | [43470855438](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43470855438) | UltraCommanderNX_Run001_Restored_Run001 (14061651074) | 193432 | Exact |

Binary SHA-256:

```text
aa4400bbc9ed9d8b7e432b9ed9de1710f3ba252e362d6cb9c9497e329b1d07f2  UltraZIP_003.tns
1cfdac13eb0fe0c911f64093900fa7675e4b044fb2bde59b42c6533239e3458d  UltraCommanderNX_Run001.tns
```

Both GitHub artifact archives were downloaded, their GitHub SHA-256 digests
verified, their ZIP CRCs checked, and their internal binary checksums verified.
The binaries were then compared with the original files in the complete backup.
This confirms faithful source restoration and reproduction of the existing
binaries; it does not add physical calculator test coverage.
