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
`bensuperpc/ndless:latest` is an **external SDK image pinned only by its moving tag**, not an immutable digest. Record the actual image digest on a successful run before claiming reproducibility. These tools have not been validated on the user's current machine or calculator as part of this import.

The supplied Run007 handoff reports successful historical CI and device launch. This newly created repository's Actions status is **not yet verified**. No source for UltraZip or Ultimate Commander was contained in the uploaded UltraGBA package.

Do not commit personal saves, copyrighted Nintendo ROMs, official GBA BIOS, firmware, passwords or signing secrets.
