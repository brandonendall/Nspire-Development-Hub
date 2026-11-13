# UltraGBA — preserved Run007 baseline

This folder holds the original cumulative Run003–Run007 patch scripts. Do not remove or overwrite Run007 behavior: working GBZ page compression, ROM-local saves, custom RTC persistence, and the existing user data locations.

## Build (known historical recipe, not yet verified in this new repo)
GitHub Actions > **UltraGBA Run007 - Custom RTC Layout Fix Rev2** > **Run workflow**.

It fetches the exact pinned `andymcca/lr-gpsp-nspire` source SHA `2ddeb23b565cab84e20278bb73ed76f3d91b25f5`, runs `python3 Projects/UltraGBA/run007_patch.py`, builds with the Ndless Docker compiler, and uploads `UltraGBA_Run007_BrowserClean.tns`.

The `vendor/upstream-gpsp` folder is a source reference snapshot, *not* the full standalone SDK. The build workflow uses the official pinned upstream clone to obtain the complete source, including its GPL-licensed open replacement BIOS asset.

## Next investigation
Run008: native Documents-browser associations for `.gbz.tns` / `.gba.tns` remain unresolved. Avoid automatic `ndless.cfg.tns` mutations, especially because existing image-viewer mappings must be preserved. Create a new branch for further changes; keep Run007 intact.

## Branding

Current development uses **[EndraGBA](../EndraGBA/README.md)**. These cumulative
scripts preserve the historical baseline and their original data paths.
