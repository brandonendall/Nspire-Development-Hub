# Verified GitHub Actions builds — November 13, 2026

**Private repository:** `brandonendall/Nspire-Development-Hub`

## Shared Ndless SDK smoke test
- Workflow: `.github/workflows/ndless-smoke-build.yml`
- GitHub Actions run: [43346931188](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43346931188)
- Conclusion: **SUCCESS**
- Generated artifact: `NspireSmoke_Run001` (download from run page)
- Compiled .tns SHA-256 logged: `a6ecc9557aae05f5b7efaa514e42aededad4ef36403439de1d386ebc1449e5f8`

## UltraGBA Run007 emulator
- Workflow: `.github/workflows/ultragba-run007.yml`
- GitHub Actions run: [43346931207](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43346931207)
- Conclusion: **SUCCESS**
- Generated artifact: `UltraGBA_Run007_BrowserClean` (download from run page)
- Artifact includes `UltraGBA_Run007_BrowserClean.tns` and `SHA256SUMS.txt`.
- The workflow clones and builds exact pinned gpSP commit `2ddeb23b565cab84e20278bb73ed76f3d91b25f5` and applies cumulative Run007 patch without modifying Ndless associations.

## Shared external compiler image
`bensuperpc/ndless:latest` pulled in both successful jobs; log records digest `sha256:fb11585d8f6aa43e69692911b713b41958fcf83aa465729210a2be5906e04093`. This **does not** mean the image tag is permanently pinned; future runs should record the current digest.

## Next acceptance test
A successful CI run confirms compiler toolchain and .tns artifact generation **only**. Brandon must test the new Run007 binary on the TI-Nspire with Ndless and safe backed-up ROM/save data. Preserve old emulator and all saves. Run008 native Documents browser associations remain unresolved.
