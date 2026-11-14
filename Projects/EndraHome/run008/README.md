# EndraHome Run008 — clean captions + visible shortcut status

Date: 2026-11-14. This is a **new experimental user-interface build**. The owner has shown that Run007 Universal renders on the TI-Nspire CX II CAS; this is **not evidence that configured app launching has succeeded**. Earlier Run006 successfully launched its specific tiny companion program according to the owner's “Everything works” report.

Run008 focuses on the two concrete problems from the Run007 device photo:
1. Default dock labels were too long and wrapped awkwardly (especially `Favorites`). Run008 uses readable **GBA / ZIP / Cmd / Files / Images / Tools / Favs**, and splits longer *custom* names into at most two six-character rows. A label's complete configured name remains visible on its detail page.
2. Run007 showed gray unconfigured markers but did not make the cause obvious. Run008 reports **CONFIG FILE MISSING**, **CHECK CONFIG FILE**, or **N / 7 READY** on the Home screen. Menu help shows config load status and the correct filename.

## No risky changes to the launcher engine

Run008 builds these original modules **unchanged**:
- `../gba-ui/platform.c` — native GBA-style keypad, touchpad, triple buffers and display/clock restore (working Run003–Run007 family).
- `../run005/logic7.c` — original seven-way navigation.
- `../run007/registry.c`, `../run007/ui.c`, `../run007/overlay.c` — read-only seven-icon config, path validation, confirmation before launching, status overlay.
- `main.c` duplicates the Run007 release-and-`nl_exec(path,0,NULL)` sequence with Run008 message titles. No new launcher code paths, no SDL, hooks, OS/root filesystem browsing, CPU tuning, background tasks or file writes.

The config file is deliberately **the same as Run007:** `EndraHome_Apps007.cfg.tns`. This makes upgrading nondestructive and allows both versions to be installed side by side. **Do not rename it to Apps008**.

## Set up using your Android phone

Place these files together in a Documents folder on the calculator:
- `EndraHome_Run008_UIPolish.tns` (this build)
- `EndraHome_Apps007.cfg.tns` (same format as Run007)

To perform the first app-launch test, keep all seven other paths empty and set exactly one slot to the **actual known path** of the previously working `EndraHome_Test006.tns`:

```ini
0=EndraGBA|
1=EndraZIP|
2=EndraCmd|
3=Files|
4=Images|
5=Tools|/documents/EndraHome/EndraHome_Test006.tns
6=Favorites|
```

The path above is **an example**, not a verified location. If the test app is in a different folder, edit accordingly. The launcher never modifies this text file.

## Physical test order

1. Keep Run006 and Run007 in place as rollback; do not overwrite any existing working .tns apps.
2. Launch Run008, then **immediately ESC** to verify return to TI Documents.
3. Reopen; verify new caption abbreviations and whether the status says CONFIG FILE MISSING, CHECK CONFIG FILE, or N / 7 READY. Use Menu to inspect the config help.
4. Set Tools to a previously working tiny companion app path and verify one configured shortcut shows. Navigate to Tools → Enter (details) → Enter (confirmation) → Enter (launch).
5. Confirm that the tiny app opens and returns to TI Documents. Do **not** test GBA yet; the universal launch path was not demonstrated for high-memory programs. Stop after any hard freeze.

## Builds

Run `make -C Projects/EndraHome/run008 test` for caption and universal parser/state tests. Run `make -C Projects/EndraHome/run008 preview` to render four reference screens. Build native using `bash toolchain/build.sh Projects/EndraHome/run008`. GitHub Actions workflow: `.github/workflows/endrahome-run008.yml`.

GPL-2.0-or-later obligations from the adapted gpSP native platform remain; public-domain bitmap font preserved.
