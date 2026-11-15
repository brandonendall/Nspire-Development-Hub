# EndraHome Run018 — JPEG/PNG Viewer + Zoom & Pan (device test pending)

The user confirmed Run017 Files text preview and fast Esc return were **perfect** on real TI-Nspire CX II CAS hardware. This build retains the tested launcher, browser, shortcut management, and text preview unchanged in behavior, adding on-demand image display.

## Source and architecture
We directly inspected the user's uploaded `mviewer.zip`: `mviewer_cx/readme.txt`, `source/mviewer.c`, `source/image.c`, `source/readjpeg.c`, and `source/Makefile`. mViewer CX Edition 4.1 provides scrolling/panning, zoom-in/out with Multiply / Divide, and Esc to return to browser. Its 2013 image decoder uses legacy IJG libjpeg, and its own browser/TSR/display code assumes older Ndless/hardware. We **adopt the viewing interaction model**, not the old browser, OS hooks or legacy compatibility-mode executable; **no mViewer GPLv2 code is copied**. The image decompressor is the repository's existing `Projects/UltimateCommander/vendor/stb_image.h` (MIT/public-domain), compiled specifically for JPEG and PNG. The source continues to carry the repository's preexisting third-party license obligations. No OS popup should be required by our current genzehn hardware metadata, although actual hardware must confirm it.

## Controls and file types
- Home → Files → browse your own Documents with existing native EndraHome picker. **ONE Enter on a JPEG or PNG opens it immediately**, including files with `.jpg.tns`, `.jpeg.tns`, or `.png.tns` endings.
- Image viewer: **Multiply (`*`) or `+` zoom IN; Divide (`/`) or `-` zoom OUT; arrows pan; Enter fit/reset; Esc or Menu instantly returns to the *same Files directory***. Repeated key presses zoom progressively (mViewer-inspired).
- Native 320×240 RGB565 dark UI; no SDL, no second file browser, no external app launch. Loading JPG/PNG is read-only and on-demand; selected image memory is freed on exit.
- Baseline JPEG and PNG supported; max **1,048,576 image pixels, 2048 pixels on either dimension, and 5 MiB encoded filesize**. Oversized, truncated or malformed data show a nonfatal error on File Information rather than attempting unlimited memory allocation. Typical mViewer included samples `forest.jpg.tns` (471×353) and `sunset.jpg.tns` (800×600) are within the limit. Images larger than limit are intentionally refused to protect hardware. Image quality/zoom are nearest-neighbor, not filtered.
- Text preview, QWERTY shortcut renaming, external Files override, transactional shortcut config save, Run011 return-home and Run010 no-extra-refresh exit are retained. PDF rendering and editing remain future, not present in this build.

## Single-pass physical test — safeguard hardware
1. **Keep the known-working Run017 .tns**, and back up `EndraHome_Apps.cfg.tns`. Upload ONLY `EndraHome_Run018_ImageViewer.tns` in the same documents folder; never upload the blank config template.
2. Check fast Home → Esc and immediate Files opening. Open one **small known-good JPEG** (e.g. mViewer's supplied `forest.jpg.tns`) by one Enter from Files. Check that a full image appears (no compatibility alert), Multiply/Divide zoom, arrows pan, Enter resets fit, and Esc returns immediately to **your original Files browser**.
3. Open `sunset.jpg.tns` (800×600) and a PNG sample (e.g. 600×450) to confirm the broader decoder. Exit without freeze and launch another assigned app to check return-home remains reliable. Do not run repeated stress tests or disconnect/reconnect solely for cosmetic checks.
4. For unrecognized/oversized/bad images, File Info should report an error without resetting the calculator. Stop and revert to Run017 if any lockup, LCD corruption, or unexpected multi-second shutdown is observed.

**Host/CI success is not a substitute for physical hardware validation.** Source files are in `Projects/EndraHome/run018`, kept separate from Run017 for safe rollback.
