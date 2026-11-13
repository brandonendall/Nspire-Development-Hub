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
- GitHub calculator builds: pending first run after this import commit.
- Physical calculator tests: not performed for these restored builds.

## Workflows

- .github/workflows/ultrazip.yml produces UltraZIP_003_Restored_Run001.
- .github/workflows/ultracommander.yml produces UltraCommanderNX_Run001_Restored_Run001.
- The pre-existing UltraGBA Run007 workflow and original-history bundle remain
  the foundation for emulator work. Its EndraGBA rename remains planned.

No account permissions or repository visibility changes are part of this import.
