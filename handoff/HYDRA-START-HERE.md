# Hydra / AI collaborator handoff — Nspire Development Hub
_Last documented: 2026-11-14 • Maintained in GitHub source control._

## Read this before making any changes

This is Brandon's private, multi-project TI-Nspire CX II CAS / Ndless development repo. **Active EndraHome code is on `feature/endrahome-run001` (draft PR #10), not the default branch `main`.** This handoff is on `main` so assistants scanning the repository can discover the branch.

Links:
- [Repository](https://github.com/brandonendall/Nspire-Development-Hub)
- [Current EndraHome feature branch](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001)
- [Draft PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10)
- [EndraHome source and documentation](https://github.com/brandonendall/Nspire-Development-Hub/tree/feature/endrahome-run001/Projects/EndraHome)
- [Latest workflows](https://github.com/brandonendall/Nspire-Development-Hub/actions)

## Project inventory

| Project | Source | Status / non-negotiable notes |
| --- | --- | --- |
| **EndraHome** | `Projects/EndraHome/` on feature branch | Launcher/UI in development; **no verified safe external app launching**. Runs 001/002 failed on hardware; 003–005 reported working UI; 006 user-reported success launching tiny companion; 007 native-built ZIP app handoff experimental, hardware not tested. |
| **UltraGBA → EndraGBA** | `Projects/UltraGBA/`, `vendor/upstream-gpsp/` | Keep Run007 known functionality (GBZ pages, RTC, ROM-local saves). Rename planned, not yet applied to the baseline. |
| **UltraZIP** | `Projects/UltraZip/` | Historical source restored, build validated; independent device results required. |
| **UltraCommander NX** | `Projects/UltimateCommander/` | Historical source restored with CI/host validation. Do not assume its device stability based only on CI. |
| **Shared build** | `toolchain/` | External Ndless Docker SDK, no SDK executable binaries checked in. |

## EndraHome device evidence — chronological

**Run001 — FAIL:** the SDL/nSDL launcher appeared, then the TI-Nspire CX II CAS completely froze at shortcut settings (photo) and again immediately after user attempted to launch a GBA shortcut; both required reset. Do not use.

**Run002 — FAIL:** a no-SDL `lcd_init/lcd_blit` diagnostic rendered Home and Help (photos show countdown at 20s and 05s). User reported full freeze after going to Menu; no crash log received. The screenshot does not establish the exact instruction or whether exit cleanup caused the hang. Do not use.

**Run003 — reported working:** rebuilt display and keypad using the established gpSP/UltraGBA architecture. The user posted a photo of the EndraHome UI with ZIP tile selected and said “Works like a champ but boy is it ugly.” Treat as a successful real-device screen + selection report, NOT proof that every exit/multi-screen path is covered. No `nl_exec`, folder listing, file writes or hooks in Run003.

**Run004 — reported working on device:** user photographed the revamped four-icon home screen and said “It works,” but explicitly compared it to a separate original concept with seven glossy icons and a big EndraHome cube logo. The device screenshot is evidence of UI rendering, not automatic proof of every exit or launch code path. App launches remain disabled.

**Run005 — closest new visual target (2026-11-14):** source `Projects/EndraHome/run005/` on `feature/endrahome-run001`. Recreates the original concept with seven icons, cube wordmark, charcoal palette, neon selection and glossy-style dock. Uses **unchanged Run003 `gba-ui/platform.c`** for framebuffer, keyboard and lifecycle; new seven-card navigation in `run005/logic7.c`. Host tests, native build and PPM screenshots passed in [GitHub Actions run 43599465872](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43599465872). **User photographed Run005 on the calculator and reported “It works.”** Seven-card UI renders; long captions overlap. Actual app execution, filesystem operations and hooks are all disabled. The battery pictogram is decorative, not measured.

**Run006 — user-reported successful Ndless test handoff (2026-11-14):** source `Projects/EndraHome/run006/` on active branch; builds TWO binaries, `EndraHome_Run006_LaunchProbe.tns` and `EndraHome_Test006.tns`. User success for Run005 allowed an incremental test: Run006 fixes overlapping seven-card captions and only lets **Tools** request a tiny success-popup program after another Enter confirmation. It does not launch GBA or any other real app. It restores the original screen and timer before calling **`nl_exec(path, 0, NULL)`** (official zero-argument usage), unlike Run001's incorrect extra argv and SDL lifecycle. Original Run003 platform and Run005 seven-card logic are compiled unchanged. [Run006 GitHub Actions run 43601629126](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43601629126) **passed** host + both native builds. After the complete test procedure, user reported **“Everything works”**, taken as success of the limited tiny companion-program handoff, not a general-purpose launcher proof. User must first open `EndraHome_Test006.tns` directly, then test the launcher Esc/navigation, then confirm Tools launch. Both .tns must live in the same Documents folder; stop on any freeze. See the [test procedure](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/run006/README.md).

**Run007 — first real-app ZIP-only launch experiment (2026-11-14):** source at `Projects/EndraHome/run007/`. Takes the **Run006 successful handoff ordering** and adds one explicitly confirmed existing ZIP executable. Both launcher and tiny Tools test app are shipped in [Run007 CI artifact](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43603146050) (host and native jobs passed). The ZIP app is NOT included: user must take a **copy of an already-working ZIP .tns**, rename that copy exactly `EndraHome_ZIP007.tns`, and place it alongside the Run007 launcher in a Documents folder. Run007 only launches ZIP or the existing Tools test, and only after a confirmation. All other icons, notably **EndraGBA**, remain preview-only. **Physical ZIP app test not yet performed**; do not call it stable. Keep Run006 unchanged and do not risk original save data. Procedure: [run007/README.md](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/run007/README.md).

## Architecture — why Run003 matters

- Reference emulator frontend: `vendor/upstream-gpsp/nspire_input.c`, `nspire_keys.c`, `nspire_video_ui.c`, `nspire/nspire.c` and `gui.c`.
- Key register/touchpad translation, edge-based action mapping, 320x240 logical UI and portrait screen buffer management are adapted from the existing working emulator.
- `Projects/EndraHome/gba-ui/platform.c`: GBA-derived display/keyboard layer. **Preserve** during visual-only changes.
- `Projects/EndraHome/gba-ui/logic.c`: pure view transitions, independent of UI rendering. **Preserve** during visual-only changes.
- `Projects/EndraHome/run004/render.c`: visual-only graphics, icons and layout. This is where appearance changes belong.
- `Projects/EndraHome/GBA-ARCHITECTURE-ANALYSIS.md` and `DEVICE-TEST-RESULTS.md`: detailed investigation on the feature branch.

## What to do next

1. Preserve the user-reported working **Run003, 004, 005 and 006** binaries and the stable emulator/ZIP programs.
2. First verify the **exact ZIP app** selected for testing works independently from Documents; do not copy or rename the original, only an extra duplicate.
3. Place the copy beside Run007 named `EndraHome_ZIP007.tns`. Verify Run007 ESC/back and navigation, then Tools → Enter → Enter → Enter to regress the known test launch.
4. Only then try **ZIP → Enter → Enter → Enter**, and exit the ZIP app to TI Documents normally. Stop after any freeze; record the screen/key and update the test log.
5. App launch success for ZIP would *not* guarantee EndraGBA compatibility, as memory/graphics behavior differs. No app configs, hooks, ROM associations, root files or operating-system settings are to be modified.


## How Hydra can use this handoff

On any new session: inspect this handoff and `AGENTS.md`, open the active PR and its latest commits, check CI artifacts and the feature branch's `Projects/EndraHome/README.md`, then continue from the last verified state. The chronological description is source-controlled but **is not automatically updated** unless an assistant commits updates. Keep a dated record per run.

## Preservation and security

- Avoid any automatic write to `ndless.cfg.tns`, system hooks, or OS partitions. No background launcher/system replacement is currently implemented.
- Do not add copyrighted ROM files, proprietary BIOS assets, passwords, or personal saved games to the repository.
- Preserve existing releases and source. Prefer additive feature branches and recoverable, non-destructive experiments.
- Original gpSP components are GPL-2.0-or-later; derivative sources must honor the original copyright/license. Public-domain pixel font resides in `Projects/UltimateCommander/vendor/`.
