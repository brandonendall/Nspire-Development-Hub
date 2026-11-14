# EndraHome — current development status

Updated 2026-11-14. Repository: `brandonendall/Nspire-Development-Hub`. Active branch: `feature/endrahome-run001`; draft [PR #10](https://github.com/brandonendall/Nspire-Development-Hub/pull/10). Do not assume these sources are merged into `main`.

**Hydra / other AI collaborators:** read `/AGENTS.md` and `/handoff/HYDRA-START-HERE.md` at repository root, then this file and `DEVICE-TEST-RESULTS.md`. Check the latest PR commit and Actions run before continuing.

## Physical device test status

| Build | Architecture | Real-device observation | Recommendation |
| --- | --- | --- | --- |
| Run001 | SDL/nSDL launcher + direct Ndless keyboard | Full calculator freeze on Settings, then another on attempted GBA launch | **FAILED. Do not use.** |
| Run002 | Ndless lcd_init/lcd_blit UI with Menu and software countdown | User photographed Home and Menu, reported full freeze after entering Menu | **FAILED. Do not use.** |
| **Run003** | UltraGBA/gpSP-derived native display + keypad + pure page state | User photographed home screen with ZIP tile selected and said "Works like a champ" | **Preserve this baseline.** Other exit/transition paths not independently confirmed. |
| **Run004** | New visual renderer + **unchanged Run003 platform/input/logic** | Native compilation and host tests have passed; physical device test pending | **Experimental visual test only.** |

## Current target: Run004 visual-only

Source: `run004/`. Separate renderer with styled header, rich hero, rounded cards, custom RGB565 pixel icons, selection accent and polished detail/help/about views. Build uses **exact same** `gba-ui/platform.c` and `gba-ui/logic.c` as user-reported working Run003. No SDL, no file I/O, no external app launch (`nl_exec` is absent), no hooks, no settings writes, and no CPU clock modifications.

- CI: [EndraHome Run004 workflow](https://github.com/brandonendall/Nspire-Development-Hub/actions/workflows/endrahome-visual004.yml)
- Local native compile: `bash toolchain/build.sh Projects/EndraHome/run004`
- Host preview generation: `make -C Projects/EndraHome/run004 preview` (four `.ppm` screen renders)
- Host state tests: `make -C Projects/EndraHome/run004 test`
- Output: `Projects/EndraHome/run004/EndraHome_Run004_Visual.tns` (in GitHub Actions artifact, not checked into source control)

**First physical test:** keep Run003 untouched, install Run004 alongside it, verify opening and immediate Esc exit, then arrows, Enter/details, Menu/help, and exiting back to the TI documents interface. Stop if the calculator freezes. No GBA launch can happen from Run004.

## Preserved diagnostic source and explanation

- `src/` — original Run001 SDL experiment (failed real-device tests).
- `recovery/` — Run002 native diagnostic (failed real-device test).
- `gba-ui/` — Run003 GBA-inspired tested UI and platform, **baseline to preserve**.
- `run004/` — isolated visual redesign using the Run003 platform and logic source.
- [Architecture analysis](GBA-ARCHITECTURE-ANALYSIS.md) and [device failures/success log](DEVICE-TEST-RESULTS.md).

## Next after Run004 physical confirmation

Plan an independent app-launch prototype with explicit exit/restore semantics; do not copy Run001's `nl_exec` call indiscriminately. Validate launches with one disposable program before considering EndraGBA, preserve all emulator saves and use existing `ndless.cfg.tns` mappings without modifying them.

This documentation is versioned with source changes; it does **not** auto-update without future commits.
