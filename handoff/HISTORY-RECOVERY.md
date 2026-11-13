# Git history recovery

The private hub's `main` branch holds an organized, non-destructive snapshot of the project. The original handoff ZIP also contains `history/UltraGBA-all-repo-branches.bundle`, which is the authoritative complete original repository history.

To preserve historical commit ancestry as GitHub branches without replacing `main`, `.github/workflows/restore-legacy-ultragba-history.yml` imports nine branch heads from the original public repository into a separate `legacy/` namespace.

The workflow validates the original Run007 commit SHA `e151b2ccb245f67b62ec8f20b9a2a51c4e55bc1f` against the handoff before pushing. It never force-pushes, and will reject branch conflicts rather than overwrite them. Only the **new private hub** needs GitHub Actions `contents:write` permission; the original is read-only.

This is not a transfer of the historical signed `.tns` artifacts or the ZIP file itself. Those remain in the uploaded handoff archive until imported separately.
