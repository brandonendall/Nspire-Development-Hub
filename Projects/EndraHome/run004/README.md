# EndraHome Run004 — Visual redesign

Status: **experimental / awaiting physical CX II CAS test**. Run003 has been observed working on the user's actual calculator, including navigation and screen drawing. Run004 preserves **exactly the same** Run003 native display and input modules by compiling them directly from `../gba-ui/platform.c` and `../gba-ui/logic.c`.

Only `render.c` and a thin `main.c` are new. This makes Run004 a controlled visual-only change: no SDL, no file access, no Ndless app launch, no hooks, no configuration writes, no changes to display init or keyboard mapping.

## Design

320x240 logical desktop with subdued gradient background, compact branded header, hero area, four rounded dock tiles with distinct hand-drawn RGB565 icons, prominent mint selection frame and redesigned detail/about/help views. Uses the existing public-domain bitmap font to avoid additional dependencies.

## Build

From repo root: `bash toolchain/build.sh Projects/EndraHome/run004`.
Host navigation unit test: `make -C Projects/EndraHome/run004 test`.
Generate four offline PPM previews: `make -C Projects/EndraHome/run004 preview` (output `build/home.ppm`, `detail.ppm`, `about.ppm`, `help.ppm`).

GitHub Actions: `.github/workflows/endrahome-visual004.yml`.
Expected artifact: `EndraHome_Run004_Visual.tns`.

## Device test

First ensure Run003 can still exit to the normal TI documents interface. Run004 should be tested as an independent experimental executable (do not replace or overwrite Run003). Check the home view, menu navigation, icon selection, details, ESC return, and normal exit. **External app launch is still deliberately disabled**. Do not attempt to launch EndraGBA from this build.

Successful CI compilation is NOT proof of hardware stability. Keep Run003 untouched as the known-working recovery baseline.
