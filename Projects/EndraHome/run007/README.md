# EndraHome Run007 — first **real** application launch (ZIP only)

Date: 2026-11-14. **Experimental until physically tested.** User reported that every step of the prior **Run006** controlled companion launch test worked on the TI-Nspire CX II CAS. That is the first successful user report of an Ndless app handoff from EndraHome.

## Strictly limited new capability

Run007 retains the **unaltered Run003 GBA-native video/keypad code**, the **unaltered Run005 7-icon navigation**, the **Run006 corrected two-line icon captions**, and the same confirmed exit-before-`nl_exec(path,0,NULL)` sequence.

Only two icons have launch behavior:
- **Tools** repeats the proven test, opening the included `EndraHome_Test006.tns` after confirmation.
- **ZIP** can open a separate preexisting, working ZIP Ndless app **but only if you place a COPY of it in the same Documents folder, renamed `EndraHome_ZIP007.tns`**. The ZIP tool is not included because user projects may have different working versions. No zip data or user documents are bundled.

All other icons, **including EndraGBA**, are information-only. ZIP launch is a new risk that requires real-device validation. User action opens the ZIP app; this launcher does not perform compression, ZIP writes, or alter any other files.

## Files to put on the calculator

Keep the existing, working Run005 and Run006 binaries unchanged. In an ordinary `/documents/EndraHome/` folder, place:
1. `EndraHome_Run007_ZIPLauncher.tns` (from this build)
2. `EndraHome_Test006.tns` (provided for Tools regression test)
3. Optional **copy of your known-working ZIP executable**, named exactly `EndraHome_ZIP007.tns`. Make a **copy**, not a move; don't overwrite your working ZIP program.

The launcher resolves both paths relative to its own `argv[0]`; it refuses non-Documents and traversal paths. A missing ZIP executable produces an error message after the launcher exits safely. It does **not** search the calculator root or change `ndless.cfg.tns`.

## Step-by-step test

1. Open the original ZIP app directly from the TI Documents browser to confirm that it works on its own, then exit; do not try the launcher if it fails.
2. Open Run007; **immediately Esc** to confirm a clean exit back to Documents.
3. Reopen Run007; test arrow navigation, Menu/Help and preview-only icons.
4. Select **Tools → Enter (details) → Enter (confirm) → Enter (run)**. The tiny test program should show the known success message and return to Documents.
5. Only if all prior steps pass, select **ZIP → Enter (details) → Enter (confirm) → Enter (run)**. The copied ZIP app should open. Exit the ZIP app normally and verify it returns to TI Documents, not a frozen or blank screen.
6. If any freeze occurs, stop. Report exactly which action triggered it. Never test on irreplaceable ZIP archives; the launcher does not perform file writes itself, but the launched ZIP app might.

Do not claim that this proves GBA launching works. EndraGBA may use different memory, ROM-loading, or graphics behavior. A successful ZIP app handoff will be a second incremental proof toward a future general launcher.

## Build / verify

- Host logic: `make -C Projects/EndraHome/run007 test`
- Host previews: `make -C Projects/EndraHome/run007 preview`
- Native: `bash toolchain/build.sh Projects/EndraHome/run007`
- Workflow: `.github/workflows/endrahome-zip007.yml`, produces both launcher and test TNS binaries plus checksum.

No SDL, filesystem traversal, system writes, hooks, clock changes or persistent app processes are introduced.
