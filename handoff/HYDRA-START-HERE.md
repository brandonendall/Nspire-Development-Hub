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
| **EndraHome** | `Projects/EndraHome/` on feature branch | Launcher/UI in development; **no verified safe external app launching**. Runs 001/002 failed on hardware; 003–005 reported working for displayed UI; 006 untested controlled handoff probe. |
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

**Run004 — experiment:** visual-only redesign of Run003 with own renderer in `Projects/EndraHome/run004/` and direct compilation of unmodified `Projects/EndraHome/gba-ui/platform.c` and `logic.c`. Native app launch is still off. Check [Run004 CI](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-visual004.yml) for status. Physical test is **pending**.

**Run006 — experimental Ndless handoff (2026-11-14):** source `Projects/EndraHome/run006/` on active branch; builds TWO binaries, `EndraHome_Run006_LaunchProbe.tns` and `EndraHome_Test006.tns`. User success for Run005 allowed an incremental test: Run006 fixes overlapping seven-card captions and only lets **Tools** request a tiny success-popup program after another Enter confirmation. It does not launch GBA or any other real app. It restores the original screen and timer before calling **`nl_exec(path, 0, NULL)`** (official zero-argument usage), unlike Run001's incorrect extra argv and SDL lifecycle. Original Run003 platform and Run005 seven-card logic are compiled unchanged. [Run006 GitHub Actions run 43601629126](https://github.com/brandonendall/Nspire-Development-Hub/actions/runs/43601629126) **passed** host + both native builds, but **physical calculator test is NOT REPORTED**. User must first open `EndraHome_Test006.tns` directly, then test the launcher Esc/navigation, then confirm Tools launch. Both .tns must live in the same Documents folder; stop on any freeze. See the [test procedure](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/run006/README.md).

## Architecture — why Run003 matters

- Reference emulator frontend: `vendor/upstream-gpsp/nspire_input.c`, `nspire_keys.c`, `nspire_video_ui.c`, `nspire/nspire.c` and `gui.c`.
- Key register/touchpad translation, edge-based action mapping, 320x240 logical UI and portrait screen buffer management are adapted from the existing working emulator.
- `Projects/EndraHome/gba-ui/platform.c`: GBA-derived display/keyboard layer. **Preserve** during visual-only changes.
- `Projects/EndraHome/gba-ui/logic.c`: pure view transitions, independent of UI rendering. **Preserve** during visual-only changes.
- `Projects/EndraHome/run004/render.c`: visual-only graphics, icons and layout. This is where appearance changes belong.
- `Projects/EndraHome/GBA-ARCHITECTURE-ANALYSIS.md` and `DEVICE-TEST-RESULTS.md`: detailed investigation on the feature branch.

## What to do next

1. Keep **Run003, Run004 and Run005 binaries** untouched as working UI rollback points.
2. Transfer the TWO `run006` .tns files into the same TI Documents folder. **First open `EndraHome_Test006.tns` directly**, ensuring its native popup works.
3. Then launch Run006 and immediately Esc. If that works, navigate all seven dock icons and Menu/help; then optionally try **Tools → Enter (details) → Enter (confirmation) → Enter (test)**.
4. If any freeze recurs, do not repeat the test. Record exactly which action failed. Only after this launcher handoff is proven should a later run consider actual Ndless apps (and GBA last).
5. Commit the user's device result to project test log, Hydra handoff and root README. CI alone never establishes hardware success.


## How Hydra can use this handoff

On any new session: inspect this handoff and `AGENTS.md`, open the active PR and its latest commits, check CI artifacts and the feature branch's `Projects/EndraHome/README.md`, then continue from the last verified state. The chronological description is source-controlled but **is not automatically updated** unless an assistant commits updates. Keep a dated record per run.

## Preservation and security

- Avoid any automatic write to `ndless.cfg.tns`, system hooks, or OS partitions. No background launcher/system replacement is currently implemented.
- Do not add copyrighted ROM files, proprietary BIOS assets, passwords, or personal saved games to the repository.
- Preserve existing releases and source. Prefer additive feature branches and recoverable, non-destructive experiments.
- Original gpSP components are GPL-2.0-or-later; derivative sources must honor the original copyright/license. Public-domain pixel font resides in `Projects/UltimateCommander/vendor/`.
