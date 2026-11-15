# EndraDiagnostics Run018 — SINGLE-SCREEN Native Tachometer HUD

## Owner's exact instruction
**"Yes that's what I want not several pages. Just like the picture."**
Run018 is a **single-screen** complete dashboard, not six detail pages. The approved reference concept includes two large segmented needle tachometers (CPU, RAM), central system information, small battery/storage/file cards, neon cyan HUD and tab-style footer. All layout elements are actual Ndless C framebuffer primitives at 320x240; no static wallpaper image.

## What actually works
- CPU and RAM 270-degree analog tachometers show full green/yellow/orange/red scale, `0` at left and `100` at right. A white needle is drawn **only if the measured value is validated**. At present, the OS 6.4.0.74 CPU utilization and free/used RAM have not been safely measured: these two dials correctly display `N/A` and NO NEEDLE. A colorful scale is NOT a claim of high usage.
- Real CP15 CPU family, revision, I/D cache status and MMU status are displayed in the center.
- Real coarse OS battery percentage (as in tested Run017) and charge-task mode, plus live battery graphics.
- Documents card shows previously verified optional read-only scanned document *known kilobytes* and file/folder counts. Until the user scans, it reads `UP TO SCAN`. Total/available disk storage are still `N/A`; the decorative neutral bar does NOT pretend to be percentage used.
- A genuine lower 6-section tab strip is visual navigation *within the same screen*. Left/Right/Menu highlights tabs without changing pages. With RAM tab selected, Up performs the pre-existing, user-triggered 4 KiB allocator-owned memory integrity check. With DOCS selected, Up invokes the pre-existing bounded read-only 384-entry/48-folder Documents scanner. None occurs automatically.
- Esc and Enter use original Run017 `eh_platform_stop_mode(0)` to exit promptly. **No auto exit.**

## Hardware protection and status
- `platform.c` copied byte-identical from physically tested Run017.
- Existing battery resolver, read-only CPU register snapshots, controlled 4 KiB heap test and optional Documents size scan preserved.
- No OS writes, charger/ADC pokes, unknown RAM probing, CPU benchmarking or filesystem mutation.
- A full UI redesign can have rendering/UI bugs on device despite successful GitHub CI. Keep Run017 as hardware-tested fallback, test Run018 only once after full off-device build/validation.

## One-connection acceptance
Upload native `EndraDiagnostics_Run018.tns` once, unplug USB, launch. One big unified screen should show CPU and RAM N/A dials, known CPU details, working battery indicator, and storage/document counters. Left/Right/Menu only select footer tab, **no pages**. Optionally select DOCS and Up to scan. Esc/Enter fast exit. Report a fault if one occurs; user requested not to be asked on every build whether exit works.

**Do not use the concept render's fictional CPU 38%, RAM 62%, or total/free storage percentages.**
