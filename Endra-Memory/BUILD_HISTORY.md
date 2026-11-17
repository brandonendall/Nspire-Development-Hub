# Endra build history and current checkpoints
_Compiled from owner repos, their docs/open PR metadata, and prior user test reports through 2026-11-17. Check current Actions for newer work._

| Project | Source-of-truth checkpoint | CI or artifact evidence | Actual device evidence / next gate |
| --- | --- | --- | --- |
| EndraZIP | Run005 user-working; Run006 recursive folders PR #6; Run007 UI fix PR #37 (`080fea4`) | Run006 Actions `43494449893`; Run007 Actions **success `43812008118`**, artifact `EndraZIP_007` ID `14201036451` | Run005 user-working; Run007 on-device complete-folder/archive/scroll test pending in this record. |
| EndraHome | PR #10 branch `feature/endrahome-run001`; main `PROJECT_MEMORY.md` snapshot Run018 ImageViewer | Run018 Actions `43744290186`, artifact `14173860704` | Runs001/002 froze; Runs003–006 UI/tiny companion works to varying scopes; Run010 quick exit reportedly fixed; newer build device proof must be checked. |
| EndraGBA | FastExit Run009 PR #17; folder-left Run010 PR #36 (`bc4b3d9`) | Check `endragba-run010` workflow / PR head | User said FastExit launches/exits immediately. Run010 layout device test pending. |
| EndraCommander | Runs003–006 in PR #3–7, frozen on multiple navigation/text paths | Check PR #7 and workflows | Root/CFG hard freezes; favor Endra SDL stable restart, not unsafe Commander binary. |
| Endra SDL | Run001 PR #8, Run002 PR #9 | Check feature branches/workflows | User explicitly said clean SDL Run001 worked perfectly; Run002 read-only text preview acceptance pending. |
| EndraDiagnostics | Runs001–024 chained PRs; Run024 #38 (`e6436ce`) | Check Run024 workflow | User screenshots show CPU/RAM gauges unrecognizable/overlapping prior; Run024 device acceptance pending. |
| EndraLink | Main README v0.1.8; newer 0.1.9–0.1.25 experiments in other branches/builds | Owner repo Android Actions; inspect release before choosing APK | fx-CG50 read-only FAT16 tests verified per README; newer Nspire USB transfer status must be inspected per exact APK. |
| EndraTermux | Run004 on `main` (archive, code, ARM64 NLink, Push v2) | Inspect `import-run004.yml` | User already uses `push` successfully; fresh install on other devices not yet established. |
| EndraClean | Main README Run039 consolidated APK and checksum | Repo downloadable `EndraClean.apk`; historical source may lag | Real-device final acceptance pending; user-app cache only. |
| EndraSpell | Main README signed Build041 candidate; source Checkpoint B Build034 | Build029 repo download, Build041 signed release link in README | Build034 worksheet flow approved; Build041 visual and in-place upgrade verification pending. |
| EndraCanasta | Flutter Prototype0.1 repo | Actions `android.yml` | Prototype interactions to test; multiplayer production not complete. |
| EndraMath | Planning and repository setup | No tested APK | Continue prototype only after approved scope. |
| PythonUltra | Independent Casio repo; Run3 migration, historical Runs131–135 | See `PYTHONULTRA_ROADMAP.md` and workflow history | Run131 color-key user hardware success; separate migration/current features need per-version acceptance. |

### Don't confuse source age with product age
- Nspire `main` still has `Projects/UltraZip` while Run007 uses `Projects/EndraZIP` on a feature branch.
- EndraLink and EndraClean main README may describe published binaries newer than checked-in source. EndraSpell signed test builds may have separate private source and signer.
- Use full commit SHA and artifact ID in new entries. The short hashes above are recognition hints, not immutable full build receipts.
