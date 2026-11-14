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
| **EndraHome** | `Projects/EndraHome/` on feature branch | Launcher/UI in development; **no verified safe external app launching**. Runs 001/002 failed on hardware; 003 user-reported working visual/navigation foundation; 004 experimental visual update. |
| **UltraGBA → EndraGBA** | `Projects/UltraGBA/`, `vendor/upstream-gpsp/` | Keep Run007 known functionality (GBZ pages, RTC, ROM-local saves). Rename planned, not yet applied to the baseline. |
| **UltraZIP** | `Projects/UltraZip/` | Historical source restored, build validated; independent device results required. |
| **UltraCommander NX** | `Projects/UltimateCommander/` | Historical source restored with CI/host validation. Do not assume its device stability based only on CI. |
| **Shared build** | `toolchain/` | External Ndless Docker SDK, no SDK executable binaries checked in. |

## EndraHome device evidence — chronological

**Run001 — FAIL:** the SDL/nSDL launcher appeared, then the TI-Nspire CX II CAS completely froze at shortcut settings (photo) and again immediately after user attempted to launch a GBA shortcut; both required reset. Do not use.

**Run002 — FAIL:** a no-SDL `lcd_init/lcd_blit` diagnostic rendered Home and Help (photos show countdown at 20s and 05s). User reported full freeze after going to Menu; no crash log received. The screenshot does not establish the exact instruction or whether exit cleanup caused the hang. Do not use.

**Run003 — reported working:** rebuilt display and keypad using the established gpSP/UltraGBA architecture. The user posted a photo of the EndraHome UI with ZIP tile selected and said “Works like a champ but boy is it ugly.” Treat as a successful real-device screen + selection report, NOT proof that every exit/multi-screen path is covered. No `nl_exec`, folder listing, file writes or hooks in Run003.

**Run004 — experiment:** visual-only redesign of Run003 with own renderer in `Projects/EndraHome/run004/` and direct compilation of unmodified `Projects/EndraHome/gba-ui/platform.c` and `logic.c`. Native app launch is still off. Check [Run004 CI](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-visual004.yml) for status. Physical test is **pending**.

## Architecture — why Run003 matters

- Reference emulator frontend: `vendor/upstream-gpsp/nspire_input.c`, `nspire_keys.c`, `nspire_video_ui.c`, `nspire/nspire.c` and `gui.c`.
- Key register/touchpad translation, edge-based action mapping, 320x240 logical UI and portrait screen buffer management are adapted from the existing working emulator.
- `Projects/EndraHome/gba-ui/platform.c`: GBA-derived display/keyboard layer. **Preserve** during visual-only changes.
- `Projects/EndraHome/gba-ui/logic.c`: pure view transitions, independent of UI rendering. **Preserve** during visual-only changes.
- `Projects/EndraHome/run004/render.c`: visual-only graphics, icons and layout. This is where appearance changes belong.
- `Projects/EndraHome/GBA-ARCHITECTURE-ANALYSIS.md` and `DEVICE-TEST-RESULTS.md`: detailed investigation on the feature branch.

## What to do next

1. Verify Run004 GitHub Actions host tests, PPM previews and native Ndless compilation **without changing the original Run003 sources**.
2. Keep Run003 `.tns` as the working rollback; put Run004 in a separate Documents folder and test appearance, arrows, Menu, Enter, Esc and clean return to TI's browser. Stop after any hard freeze.
3. Update source README, this handoff, and the root index with the exact user-reported device result. Do not say “device-tested” merely because a GitHub Actions job passed.
4. Only after visual/navigation success, design an isolated **separate** app-launch test. Do not re-enable `nl_exec` casually or modify Ndless associations.

## How Hydra can use this handoff

On any new session: inspect this handoff and `AGENTS.md`, open the active PR and its latest commits, check CI artifacts and the feature branch's `Projects/EndraHome/README.md`, then continue from the last verified state. The chronological description is source-controlled but **is not automatically updated** unless an assistant commits updates. Keep a dated record per run.

## Preservation and security

- Avoid any automatic write to `ndless.cfg.tns`, system hooks, or OS partitions. No background launcher/system replacement is currently implemented.
- Do not add copyrighted ROM files, proprietary BIOS assets, passwords, or personal saved games to the repository.
- Preserve existing releases and source. Prefer additive feature branches and recoverable, non-destructive experiments.
- Original gpSP components are GPL-2.0-or-later; derivative sources must honor the original copyright/license. Public-domain pixel font resides in `Projects/UltimateCommander/vendor/`.
