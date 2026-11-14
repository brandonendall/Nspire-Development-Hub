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
| **EndraHome** | `Projects/EndraHome/` on feature branch | Launcher/UI in development; **no verified safe external app launching**. Runs 001/002 failed on hardware; 003–005 reported working UI; 006 user-reported success launching tiny companion; 007 was redone from a discarded ZIP-only experiment into a generic, configurable seven-icon launcher; hardware test pending. |
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

**Run007 — REDO: general configurable Ndless launcher (2026-11-14):** The previous ZIP-only Run007 was replaced **before any physical device test** because it incorrectly treated EndraHome as a ZIP-focused workflow. The current source at `Projects/EndraHome/run007/` creates `EndraHome_Run007_Universal.tns` and the editable `EndraHome_Apps007.cfg.tns`. Every one of seven icons can launch **any explicitly configured compatible Ndless executable under `/documents/`**, regardless of the icon's default appearance. No program is preassigned; no ZIP compression, file manager, ROM, app-discovery or background OS functionality is bundled. A read-only config parser validates paths/labels and disallows traversal. Before calling `nl_exec(path,0,NULL)`, the app restores LCD/timer/OS screen using the Run006-proven exit order. Reuses unchanged Run003 `gba-ui/platform.c` and unchanged Run005 `logic7.c`. The original Run006 tiny companion launch is the recommended first test; GBA may have different resource requirements and should not be tried first. The current universal Run007 is **not yet hardware-verified**. See the [current Run007 README](https://github.com/brandonendall/Nspire-Development-Hub/blob/feature/endrahome-run001/Projects/EndraHome/run007/README.md) and [CI workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-universal007.yml).

**Run008 — UI clarity and config diagnostic build (2026-11-14):** User photographed the Run007 universal Home but **did not report testing any configured application launch**. Run008 is an isolated UI/readability improvement: the previously overlapped dock labels become compact GBA / ZIP / Cmd / Files / Images / Tools / Favs; a new banner explains `CONFIG FILE MISSING`, `CHECK CONFIG FILE`, or `N / 7 READY`. Menu displays the actual `EndraHome_Apps007.cfg.tns` filename, which **remains shared between Runs 007 and 008**. Underlying Run003 `gba-ui/platform.c`, Run005 `logic7.c` and Run007 `registry.c` / `ui.c` / `overlay.c` are compiled unmodified. App launch/return order remains the same as Run006/007. New sources are in `Projects/EndraHome/run008/`; [CI workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-run008.yml). Native hardware test pending, and **universal app launch still unverified**.

## Architecture — why Run003 matters

- Reference emulator frontend: `vendor/upstream-gpsp/nspire_input.c`, `nspire_keys.c`, `nspire_video_ui.c`, `nspire/nspire.c` and `gui.c`.
- Key register/touchpad translation, edge-based action mapping, 320x240 logical UI and portrait screen buffer management are adapted from the existing working emulator.
- `Projects/EndraHome/gba-ui/platform.c`: GBA-derived display/keyboard layer. **Preserve** during visual-only changes.
- `Projects/EndraHome/gba-ui/logic.c`: pure view transitions, independent of UI rendering. **Preserve** during visual-only changes.
- `Projects/EndraHome/run004/render.c`: visual-only graphics, icons and layout. This is where appearance changes belong.
- `Projects/EndraHome/GBA-ARCHITECTURE-ANALYSIS.md` and `DEVICE-TEST-RESULTS.md`: detailed investigation on the feature branch.

## What to do next

1. Keep original user-tested Run006 and other known-working programs unchanged; do not touch UltraGBA ROM/saves.
2. Install Run008 along with `EndraHome_Apps007.cfg.tns` in the same TI Documents folder. Run008 **reuses Run007's config filename**. Keep the binary under its own name.
3. Verify Run008 ESC exit, all seven readable labels, Menu help, and clear status reporting for missing/empty/configured paths. At this point there is only proof of UI.
4. Configure one slot to the real path of the small `EndraHome_Test006.tns` (Run006 known working), then test the full confirmation/launch/return sequence. If that works, test one other small Ndless program, never everything at once.
5. Log outcomes with the exact affected build; no GBA or hardware-level success claim without explicit user confirmation. No config writes, hooks or OS replacement.


## How Hydra can use this handoff

On any new session: inspect this handoff and `AGENTS.md`, open the active PR and its latest commits, check CI artifacts and the feature branch's `Projects/EndraHome/README.md`, then continue from the last verified state. The chronological description is source-controlled but **is not automatically updated** unless an assistant commits updates. Keep a dated record per run.

## Preservation and security

- Avoid any automatic write to `ndless.cfg.tns`, system hooks, or OS partitions. No background launcher/system replacement is currently implemented.
- Do not add copyrighted ROM files, proprietary BIOS assets, passwords, or personal saved games to the repository.
- Preserve existing releases and source. Prefer additive feature branches and recoverable, non-destructive experiments.
- Original gpSP components are GPL-2.0-or-later; derivative sources must honor the original copyright/license. Public-domain pixel font resides in `Projects/UltimateCommander/vendor/`.
