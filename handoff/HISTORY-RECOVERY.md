# Original Git history preservation

The complete UltraGBA commit ancestry and branch heads are being preserved in a **portable Git bundle** at `handoff/history/UltraGBA-all-repo-branches.bundle`.

The workflow `.github/workflows/restore-legacy-ultragba-history.yml` fetches the original public repository, verifies that its Run007 branch equals the head recorded in the uploaded handoff (`e151b2ccb245f67b62ec8f20b9a2a51c4e55bc1f`), creates a bundle of *all* branch refs, and commits only this binary archive and checksums to the private hub.

**Reason for this method:** GitHub blocks the GitHub Actions token from importing branches that contain `.github/workflows/` changes without additional workflow permissions. This archive preserves the original commits without widening access and without rewriting the new hub's `main`.

Once the workflow succeeds, on a development computer:

```sh
git clone handoff/history/UltraGBA-all-repo-branches.bundle UltraGBA-history
git -C UltraGBA-history branch -a
```

A restored branch can then be inspected or cherry-picked into future feature branches. Do not force-push it over the new hub's `main`. Historical Run002–Run007 built `.tns` binaries remain in the original uploaded ZIP, while a newly built Run007 artifact is available from this hub's GitHub Actions.
