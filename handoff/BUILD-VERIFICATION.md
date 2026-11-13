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

## UltraZIP 003 restoration
- Source import commit: `dc3675264d2e6ce0201caf9972435d91157b0564`.
- Workflow: `.github/workflows/ultrazip.yml`.
- Successful run: [43470855176](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43470855176).
- Artifact: `UltraZIP_003_Restored_Run001`, ID 14059734970.
- Output: `UltraZIP_003.tns`, 211408 bytes.
- SHA-256: `aa4400bbc9ed9d8b7e432b9ed9de1710f3ba252e362d6cb9c9497e329b1d07f2`.
- Exact byte match to the original saved UltraZIP 003 binary.

## UltraCommander NX Run001 restoration
- Source import commit: `dc3675264d2e6ce0201caf9972435d91157b0564`.
- Workflow: `.github/workflows/ultracommander.yml`.
- Successful run: [43470855438](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43470855438).
- Artifact: `UltraCommanderNX_Run001_Restored_Run001`, ID 14061651074.
- Output: `UltraCommanderNX_Run001.tns`, 193432 bytes.
- SHA-256: `1cfdac13eb0fe0c911f64093900fa7675e4b044fb2bde59b42c6533239e3458d`.
- Exact byte match to the original saved Run001 binary.
- Original host suite passed all 129 checks locally and on GitHub with
  AddressSanitizer and UndefinedBehaviorSanitizer.
- Uses the original GNU Arm build recipe and Ndless revision
  `c659d1691295dd5db79ab171fff680638ca6a4bf`.

The restored binary archives and their internal checksums were independently
verified after download. Physical calculator verification remains pending.
