# Shared Ndless build environment

The **compiler/SDK is not embedded in this Git repository or the supplied ZIP**. Reuse a known Ndless Docker toolchain for all calculator applications, rather than reinstalling the SDK per app.

## GitHub Actions
- **Shared Ndless SDK - Smoke Build** (`.github/workflows/ndless-smoke-build.yml`) checks `nspire-gcc`, `nspire-ld`, and `genzehn`, compiles a minimal program, and uploads a `.tns` artifact if it succeeds.
- **UltraGBA Run007** (`.github/workflows/ultragba-run007.yml`) clones the pinned source commit `2ddeb23b565cab84e20278bb73ed76f3d91b25f5`, applies `Projects/UltraGBA/run007_patch.py`, builds in `bensuperpc/ndless:latest`, and uploads its distinct `.tns` artifact if successful.

Open GitHub > Actions, select a workflow, and choose **Run workflow** to trigger manually. Actions may require enabling in repository settings. Download the artifact only after the run succeeds.

## Local Docker
From the repository root, with Docker installed:

```sh
bash toolchain/build.sh tools/ndless-smoke
```

For a new Ndless application, create its own `Projects/<App>/Makefile` with the required `nspire-gcc`, `nspire-ld`, and `genzehn` commands, then invoke `bash toolchain/build.sh Projects/<App>`. The script shares one Docker image across all projects.

## Pinning, verification and limitations
`bensuperpc/ndless:latest` is an **external SDK image pinned only by its moving tag**, not an immutable digest. Record the actual image digest on a successful run before claiming reproducibility. GitHub Actions compilation succeeded in the new private hub on November 13, 2026. This does **not** establish local-machine or calculator hardware verification.

The supplied Run007 handoff reports successful historical CI and device launch. The new hub's Actions builds are **verified passing**: shared SDK smoke run 43346931188 and Run007 emulator run 43346931207. The subsequent complete-backup import restored UltraZip and Ultimate Commander sources.

Do not commit personal saves, copyrighted Nintendo ROMs, official GBA BIOS, firmware, passwords or signing secrets.

## Restored applications

- UltraZIP 003: run `(cd Projects/UltraZip && sh get_miniz.sh)`, then
  `bash toolchain/build.sh Projects/UltraZip`. Its new CI workflow pins the
  previously verified Ndless image by digest.
- UltraCommander NX Run001: follow `Projects/UltimateCommander/README.md`.
  Its unchanged Makefile uses GNU Arm GCC and an explicit `NDLESS_SDK` path.
  The dedicated workflow installs its documented prerequisites and runs the
  original SDK script pinned to Ndless c659d1691295dd5db79ab171fff680638ca6a4bf.
  Host filesystem/ZIP checks run before the calculator build.

The preserved source packages include matching SDK runtime source and licenses,
not standalone compiler binaries.
